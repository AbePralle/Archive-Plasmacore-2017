.PHONY: build

COMPILE = $(CXX) -Wall -std=gnu++11 -fno-strict-aliasing -Wno-invalid-offsetof

all: build run

build: Build/BuildScript/buildscript

clean:
	rm -rf Build

Build/BuildScript/buildscript: Build/BuildScript/BuildScript.cpp
	cd Build/BuildScript && $(COMPILE) BuildScript.cpp -o buildscript
	chmod u+x Build/BuildScript/buildscript

Build/BuildScript/BuildScript.cpp: BuildScript.rogue Makefile
	@mkdir -p Build/BuildScript
	../../Programs/macOS/roguec --main --libraries=../Rogue BuildScript.rogue --output=Build/BuildScript --define=COMPILER_INVOCATION:"\"$(COMPILE)\""

run:
	Build/BuildScript/buildscript

