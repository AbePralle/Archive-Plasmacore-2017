CXX = em++
CC = emcc

TARGET = Web

# The project root directory
ROOT = ../..

OPT_CFLAGS += -O3
OPT_CFLAGS += -s ASSERTIONS=0 -s PRECISE_I64_MATH=0 --closure 1

SDLCORE_FOLDER = $(ROOT)/Libraries/Plasmacore/SDL

BUILD_FOLDER    := $(ROOT)/Build/$(TARGET)

# Name to use for the final output
# You can just set this to a value if you want; the default
# tries to guess based on directory name.
# This is the base name -- ".html" and stuff is added.
OUTPUT_NAME := $(shell basename $(shell cd $(ROOT) && pwd))


SDLCORE_FILES := \
  $(SDLCORE_FOLDER)/plasmacore.cpp \
  $(SDLCORE_FOLDER)/plasmacore_message.cpp \
  $(SDLCORE_FOLDER)/plasmacore_view.cpp \
  $(SDLCORE_FOLDER)/plasmacore_sound_sdl_mixer.cpp \
  $(SDLCORE_FOLDER)/rogue_interface.cpp


all: build

build: $(BUILD_FOLDER)/Intermediate project

$(BUILD_FOLDER)/Intermediate:
	mkdir -p $(BUILD_FOLDER)/Intermediate

clean:
	rm -f $(BUILD_FOLDER)/$(OUTPUT_NAME).*
	rm -rf $(BUILD_FOLDER)/Assets
	rm -f $(BUILD_FOLDER)/Source/RogueProgram.cpp
	rm -f $(BUILD_FOLDER)/Source/RogueProgram.h
	rm -f $(BUILD_FOLDER)/Intermediate/*.o

project: $(BUILD_FOLDER)/$(OUTPUT_NAME).html custom_index_html

$(BUILD_FOLDER)/$(OUTPUT_NAME).html: $(BUILD_FOLDER)/Source/RogueProgram.cpp $(SDLCORE_FILES)
	# -s FULL_ES2=1 ?
	$(CXX) -I$(BUILD_FOLDER)/Source \
	       -s USE_SDL=2 \
	       -s USE_LIBPNG=1 \
	       -std=c++11 \
	       --preload-file $(BUILD_FOLDER)/Assets@/Assets \
	       -Wall \
	       -fno-strict-aliasing \
	       $(OPT_CFLAGS) \
	       $(CXXFLAGS) \
	       $(CFLAGS) \
	       $(BUILD_FOLDER)/Source/RogueProgram.cpp \
	       $(SDLCORE_FILES) \
	       -o $(BUILD_FOLDER)/$(OUTPUT_NAME).html

custom_index_html:
	@test -f index.html && make $(BUILD_FOLDER)/index.html || true

$(BUILD_FOLDER)/index.html: index.html
	@echo COPYING && cp index.html $(BUILD_FOLDER)/

