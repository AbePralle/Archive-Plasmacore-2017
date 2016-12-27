BUILD_FOLDER := Build

ROGUEC        = roguec
ifeq ($(OS),Windows_NT)
else
  UNAME_S := $(shell uname -s)
  ifeq ($(UNAME_S),Darwin)
    ROGUEC = ../../Programs/Mac/roguec
  else
  endif
endif

ROGUEC_FLAGS += --target="C++,Console"
ROGUEC_FLAGS += --debug
ROGUEC_FLAGS += --libraries=../../Libraries/Rogue

ROGUE_LIBS = $(shell find ../Rogue | grep "\.rogue$$")

all: build

build: $(BUILD_FOLDER) soundcompiler

$(BUILD_FOLDER):
	mkdir -p $(BUILD_FOLDER)

clean:
	rm -rf $(BUILD_FOLDER)

soundcompiler: $(BUILD_FOLDER)/soundcompiler

$(BUILD_FOLDER)/soundcompiler: SoundCompiler.rogue $(ROGUE_LIBS)
	$(ROGUEC) $(ROGUEC_FLAGS) SoundCompiler.rogue --main --output=$(BUILD_FOLDER)/SoundCompiler
	@echo Compiling SoundCompiler...
	@clang++ -Wall $(AM_CCFLAGS) -std=c++11 -fno-strict-aliasing $(BUILD_FOLDER)/SoundCompiler.cpp -o $(BUILD_FOLDER)/soundcompiler
