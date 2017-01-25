PLASMACORE_VERSION = v0.7.0.21

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

-include Local.mk

export CC
export CXX

ios: override TARGET := iOS

macos: override TARGET := macOS

emscripten: override TARGET := emscripten

linux: override TARGET := Linux

all: build run compile_images compile_sounds

ios: build run compile_images compile_sounds

macos: build run compile_images compile_sounds

emscripten: build run compile_images compile_sounds
	make -C Platforms/emscripten

linux: build run compile_images compile_sounds
	make -C Platforms/Linux

build: Build/BuildScript/buildscript

compile_images:
	env TARGET=$(TARGET) $(ENV_IDE_FLAG) ./icom $(TARGET)

compile_sounds:
	env TARGET=$(TARGET) $(ENV_IDE_FLAG) ./scom $(TARGET)

Build/BuildScript:
	mkdir -p Build/BuildScript

Build/BuildScript/buildscript: Build/BuildScript BuildScript.rogue BuildScriptCore.rogue $(ROGUE_LIBRARIES)
	$(ROGUEC) $(ROGUE_IDE_FLAG) BuildScript.rogue BuildScriptCore.rogue --libraries=Libraries/Rogue --output=Build/BuildScript/BuildScript --main
	$(CXX) -std=c++11 -DROGUEC=$(ROGUEC) -fno-strict-aliasing Build/BuildScript/BuildScript.cpp -o Build/BuildScript/buildscript

run:
	env Build/BuildScript/buildscript $(TARGET) $(ROGUE_IDE_FLAG)

clean:
	rm -rf Build
	rm -rf Platform/iOS/Build
	rm -rf Libraries/SoundCompiler/Build
	rm -rf Libraries/HarfBuzz/Build
	rm -rf Libraries/ImageCompiler/Build

clean_harfbuzz:
	make -C Libraries/HarfBuzz clean

clean_icom:
	make -C Libraries/ImageCompiler clean

clean_scom:
	make -C Libraries/SoundCompiler clean

xclean: clean clean_harfbuzz clean_icom clean_scom

update: prepare_update

prepare_update:
	@[ ! -e Build/Update/Plasmacore ] && echo "Cloning Plasmacore into Build/Update/" && mkdir -p Build/Update && cd Build/Update && git clone $(REPO) Plasmacore || true
	@echo "Pulling latest Build/Update/Plasmacore/"
	@[ -e Build/Update/Plasmacore ] && cd Build/Update/Plasmacore && git checkout $(BRANCH) && git pull
	@rsync -a -c --out-format="Updating %n%L" Build/Update/Plasmacore/Makefile .
	@make -f Makefile continue_update

continue_update:
	@rsync -a -c --exclude=".*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/BuildScriptCore.rogue .
	@rsync -a -c --exclude=".*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/icom .
	@rsync -a -c --exclude=".*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/scom .
	@rsync -a -c --exclude=".*" --exclude="Build/*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/Libraries/ImageCompiler Libraries
	@rsync -a -c --exclude=".*" --exclude="Build/*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/Libraries/SoundCompiler Libraries
	@rm -rf Libraries/FreeType
	@rm -rf Libraries/HarfBuzz
	@rsync -a -c --exclude=".*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/Libraries/ImageIO      Libraries
	@rsync -a -c --exclude=".*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/Libraries/Plasmacore   Libraries
	@rsync -a -c --exclude=".*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/Libraries/Rogue/Standard Libraries/Rogue
	@rsync -a -c --exclude=".*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/Libraries/Rogue/Plasmacore Libraries/Rogue
	@rsync -a -c --exclude=".*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/Programs/macOS/roguec Programs/macOS/
	@echo "Updated to Plasmacore $(PLASMACORE_VERSION)"


