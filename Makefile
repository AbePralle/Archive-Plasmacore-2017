ROGUE_LIBRARIES = $(shell find Libraries/Rogue | grep "\.rogue$$")

ios: override TARGET := iOS

all: build run compile_images

ios: build run compile_images

build: Build/BuildScript/buildscript

compile_images:
	./am compile iOS

Build/BuildScript:
	mkdir -p Build/BuildScript

Build/BuildScript/buildscript: Build/BuildScript Build.rogue $(ROGUE_LIBRARIES)
	Programs/Mac/roguec Build.rogue --libraries=Libraries/Rogue --output=Build/BuildScript/BuildScript --main
	c++ -std=c++11 -fno-strict-aliasing Build/BuildScript/BuildScript.cpp -o Build/BuildScript/buildscript

run:
	Build/BuildScript/buildscript $(TARGET)

clean:
	rm -rf Build
	rm -rf Platform/iOS/Build

clean_harfbuzz:
	make -C Libraries/HarfBuzz clean

clean_am:
	make -C Libraries/AssetManager clean

xclean: clean clean_harfbuzz clean_am

update: run_local_mk prepare_update

run_local_mk:
	@# Run the local makefile if it exists
	@[ -f Local.mk ] && make -f Local.mk update || true

prepare_update:
	@[ ! -e Build/Update/Plasmacore ] && echo "Cloning Plasmacore master branch into Build/Update/" && mkdir -p Build/Update && cd Build/Update && git clone git@github.com:AbePralle/Plasmacore.git || true
	@echo "Pulling latest Build/Update/Plasmacore/"
	@[ -e Build/Update/Plasmacore ] && cd Build/Update/Plasmacore && git pull
	@rsync -a --out-format="Updating %n%L" Build/Update/Plasmacore/Makefile Makefile
	@make -f Makefile continue_update

continue_update:
	@rsync -a --exclude=".*" --delete --out-format="Updating %n%L" Build/Update/Plasmacore/BuildCore.rogue .

	@#mkdir -p Build/Update
	@#https://github.com/AbePralle/Rogue

#------------------------------------------------------------------------------
# Sample optional Local.mk file - do not commit to repo
#
#PLASMACORE          = ../../Plasmacore
#ROGUE_STANDARD_LIB = ../../Rogue/Source/Libraries/Standard
#
#update:
#	@rsync -a --exclude=".*" --delete --out-format="Updating %n%L" $(ROGUE_STANDARD_LIB) Libraries/Rogue
#	@rsync -a --exclude=".*" --delete --out-format="Updating %n%L" $(PLASMACORE)/Build.rogue .
#	@rsync -a --exclude=".*"          --out-format="Updating %n%L" $(PLASMACORE)/Libraries/AssetManager Libraries
#	@rsync -a --exclude=".*" --delete --out-format="Updating %n%L" $(PLASMACORE)/Libraries/Rogue/Plasmacore Libraries/Rogue
#	@rsync -a --exclude=".*" --delete --out-format="Updating %n%L" $(PLASMACORE)/Libraries/ImageIO Libraries


