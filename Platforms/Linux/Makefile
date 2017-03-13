TARGET = Linux

# The project root directory
ROOT = ../..

OPT_CFLAGS += -O3

SDLCORE_FOLDER = $(ROOT)/Libraries/Plasmacore/SDL

BUILD_FOLDER    := $(ROOT)/Build/$(TARGET)

# Name to use for the final output
# You can just set this to a value if you want; the default
# tries to guess based on directory name.
OUTPUT_NAME := $(shell basename $(shell cd $(ROOT) && pwd))


SDLCORE_FILES := \
  $(SDLCORE_FOLDER)/Plasmacore.cpp \
  $(SDLCORE_FOLDER)/PlasmacoreMessage.cpp \
  $(SDLCORE_FOLDER)/PlasmacoreView.cpp \
  $(SDLCORE_FOLDER)/RogueInterface.cpp \
  $(SDLCORE_FOLDER)/PlasmacoreSoundSDLMixer.cpp \
  $(SDLCORE_FOLDER)/PlasmacoreUtility.cpp
  #$(SDLCORE_FOLDER)/plasmacore_sound_dummy.cpp

all: build

build: $(BUILD_FOLDER) project

$(BUILD_FOLDER):
	mkdir -p $(BUILD_FOLDER)

clean:
	rm -f $(BUILD_FOLDER)/$(OUTPUT_NAME)
	rm -rf $(BUILD_FOLDER)/Assets
	rm -f $(BUILD_FOLDER)/Source/RogueProgram.cpp
	rm -f $(BUILD_FOLDER)/Source/RogueProgram.h

project: $(BUILD_FOLDER)/$(OUTPUT_NAME)

$(BUILD_FOLDER)/$(OUTPUT_NAME): $(BUILD_FOLDER)/Source/RogueProgram.cpp $(SDLCORE_FILES)
	$(CXX) -I$(BUILD_FOLDER)/Source \
	       -ljpeg \
	       -lpng \
	       -lSDL2 \
	       -lSDL2_mixer \
	       -lGL \
	       -std=c++11 \
	       -Wall \
	       -fno-strict-aliasing \
	       $(OPT_CFLAGS) \
	       $(CXXFLAGS) \
	       $(CFLAGS) \
	       $(BUILD_FOLDER)/Source/RogueProgram.cpp \
	       $(SDLCORE_FILES) \
	       -o $(BUILD_FOLDER)/$(OUTPUT_NAME)
