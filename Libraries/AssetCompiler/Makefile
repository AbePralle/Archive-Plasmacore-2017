.PHONY: build

ROGUEC = roguec
ifeq ($(OS),Windows_NT)
else
  UNAME_S := $(shell uname -s)
  ifeq ($(UNAME_S),Darwin)
    ROGUEC = ../../Programs/macOS/roguec
  else
  endif
endif

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
	$(ROGUEC) --main --libraries=../Rogue BuildScript.rogue --output=Build/BuildScript --define=COMPILER_INVOCATION:"\"$(COMPILE)\"" --define=ROGUEC:"\"$(ROGUEC)\""

run:
	Build/BuildScript/buildscript

