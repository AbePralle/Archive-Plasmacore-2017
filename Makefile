PLASMACORE_VERSION = v0.5.4.0

ROGUE_LIBRARIES = $(shell find Libraries/Rogue | grep "\.rogue$$")

ios: override TARGET := iOS

mac: override TARGET := Mac

all: build run compile_images compile_sounds

ios: build run compile_images compile_sounds

mac: build run compile_images compile_sounds

build: Build/BuildScript/buildscript

compile_images:
	./icom $(TARGET)

compile_sounds:
	./scom $(TARGET)

Build/BuildScript:
	mkdir -p Build/BuildScript

Build/BuildScript/buildscript: Build/BuildScript Build.rogue BuildCore.rogue $(ROGUE_LIBRARIES)
	Programs/Mac/roguec Build.rogue --libraries=Libraries/Rogue --output=Build/BuildScript/BuildScript --main
	c++ -std=c++11 -fno-strict-aliasing Build/BuildScript/BuildScript.cpp -o Build/BuildScript/buildscript

run:
	Build/BuildScript/buildscript $(TARGET)

clean:
	rm -rf Build
	rm -rf Platform/iOS/Build

clean_harfbuzz:
	make -C Libraries/HarfBuzz clean

clean_icom:
	make -C Libraries/ImageCompiler clean

clean_scom:
	make -C Libraries/SoundCompiler clean

xclean: clean clean_harfbuzz clean_icom clean_scom

update: prepare_update

prepare_update:
	@[ ! -e Build/Update/Plasmacore ] && echo "Cloning Plasmacore master branch into Build/Update/" && mkdir -p Build/Update && cd Build/Update && git clone https://github.com/AbePralle/Plasmacore.git || true
	@echo "Pulling latest Build/Update/Plasmacore/"
	@[ -e Build/Update/Plasmacore ] && cd Build/Update/Plasmacore && git pull
	@rsync -a -c --out-format="Updating %n%L" Build/Update/Plasmacore/Makefile .
	@make -f Makefile continue_update

continue_update:
	@rsync -a -c --exclude=".*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/BuildCore.rogue .
	@rsync -a -c --exclude=".*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/icom .
	@rsync -a -c --exclude=".*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/scom .
	@rsync -a -c --exclude=".*" --exclude="Build/*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/Libraries/ImageCompiler Libraries
	@rsync -a -c --exclude=".*" --exclude="Build/*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/Libraries/SoundCompiler Libraries
	@rsync -a -c --exclude=".*" --exclude="Build/*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/Libraries/FreeType     Libraries
	@rsync -a -c --exclude=".*" --exclude="Build/*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/Libraries/HarfBuzz     Libraries
	@rsync -a -c --exclude=".*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/Libraries/ImageIO      Libraries
	@rsync -a -c --exclude=".*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/Libraries/Plasmacore   Libraries
	@rsync -a -c --exclude=".*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/Libraries/Rogue/Standard Libraries/Rogue
	@rsync -a -c --exclude=".*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/Libraries/Rogue/Plasmacore Libraries/Rogue
	@rsync -a -c --exclude=".*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/Programs/Mac/roguec Programs/Mac
	@echo "Updated to Plasmacore $(PLASMACORE_VERSION)"


