PLASMACORE_VERSION = v0.8.10

# Repo and branch to update from - override with e.g. make update BRANCH=develop
REPO = https://github.com/AbePralle/Plasmacore.git
BRANCH = master

ROGUE_LIBRARIES = $(shell find Libraries/Rogue | grep "\.rogue$$")

.PHONY: build

ifeq ($(IDE),true)
  ROGUE_IDE_FLAG = --ide
  ENV_IDE_FLAG = IDE=true
else
  ROGUE_IDE_FLAG =
  ENV_IDE_FLAG =
endif


ROGUEC = roguec

ifeq ($(OS),Windows_NT)
else
  UNAME_S := $(shell uname -s)
  ifeq ($(UNAME_S),Darwin)
    ROGUEC = Programs/macOS/roguec
  else
  endif
endif

-include Custom.mk
-include Local.mk

export CC
export CXX

ios: override TARGET := iOS

macos: override TARGET := macOS

web: override TARGET := Web

linux: override TARGET := Linux

all: build run compile_assets

ios: build run compile_assets

macos: build run compile_assets

web: build run compile_assets
	make -C Platforms/Web

linux: build run compile_assets
	make -C Platforms/Linux

build: Build/BuildScript/buildscript

compile_assets:
	@env TARGET=$(TARGET) $(ENV_IDE_FLAG) make -C Libraries/AssetCompiler
	@env TARGET=$(TARGET) $(ENV_IDE_FLAG) Libraries/AssetCompiler/Build/assetcompiler $(TARGET)

Build/BuildScript:
	mkdir -p Build/BuildScript

Build/BuildScript/buildscript: Build/BuildScript BuildScript.rogue BuildScriptCore.rogue $(ROGUE_LIBRARIES)
	$(ROGUEC) $(ROGUE_IDE_FLAG) BuildScript.rogue BuildScriptCore.rogue --libraries=Libraries/Rogue --output=Build/BuildScript/BuildScript --main
	$(CXX) -std=c++11 -DROGUEC=$(ROGUEC) -fno-strict-aliasing Build/BuildScript/BuildScript.cpp -o Build/BuildScript/buildscript

run:
	env Build/BuildScript/buildscript $(TARGET) $(ROGUE_IDE_FLAG)

clean:
	rm -rf Build
	rm -rf Libraries/AssetCompiler/Build

clean_asset_compiler:
	make -C Libraries/AssetCompiler clean

xclean: clean clean_harfbuzz clean_asset_compiler

update: prepare_update

prepare_update:
	@[ ! -e Build/Update/Plasmacore ] && echo "Cloning Plasmacore into Build/Update/" && mkdir -p Build/Update && cd Build/Update && git clone $(REPO) Plasmacore || true
	@echo "Pulling latest Build/Update/Plasmacore/"
	@[ -e Build/Update/Plasmacore ] && cd Build/Update/Plasmacore && git checkout $(BRANCH) && git pull
	@rsync -a -c --out-format="Updating %n%L" Build/Update/Plasmacore/Makefile .
	@make -f Makefile continue_update

continue_update:
	@rsync -a -c --exclude=".*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/BuildScriptCore.rogue .
	@rsync -a -c --exclude=".*" --exclude="Build/*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/Libraries/AssetCompiler Libraries
	@rsync -a -c --exclude=".*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/Libraries/ImageIO      Libraries
	@rsync -a -c --exclude=".*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/Libraries/Plasmacore   Libraries
	@rsync -a -c --exclude=".*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/Libraries/Rogue/Standard Libraries/Rogue
	@rsync -a -c --exclude=".*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/Libraries/Rogue/Plasmacore Libraries/Rogue
	@rsync -a -c --exclude=".*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/Programs/macOS/roguec Programs/macOS/
	@echo "Updated to Plasmacore $(PLASMACORE_VERSION)"


