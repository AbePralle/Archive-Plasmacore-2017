.PHONY : build

COMPILE = $(CXX) -Wall -std=gnu++11 -fno-strict-aliasing -Wno-invalid-offsetof

all: build run

build: Build/RGB/rgb

clean:
	rm -rf Build

Build/RGB/rgb: Build/RGB/RGB.cpp
	cd Build/RGB && $(COMPILE) RGB.cpp -o rgb

Build/RGB/RGB.cpp: RGB.rogue Makefile
	@mkdir -p Build/RGB
	../../Programs/Mac/roguec --main --libraries=../Rogue RGB.rogue --output=Build/RGB --define=COMPILER_INVOCATION:"\"$(COMPILE)\""

run:
	chmod u+x Build/RGB/rgb
	Build/RGB/rgb

