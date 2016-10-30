BUILD_FOLDER := Build

ROGUEC        = ../../Programs/Mac/roguec --libraries=../../Libraries/Rogue
ROGUEC_FLAGS += --target="C++,Console"
ROGUEC_FLAGS += --debug

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
