CXX = em++
CC = emcc

TARGET = emscripten

# The project root directory
ROOT = ../..

OPT_CFLAGS += -O3 -s ASSERTIONS=0 -s PRECISE_I64_MATH=0 --closure 1

SDLCORE_FOLDER = $(ROOT)/Libraries/Plasmacore/SDL

JPEG_FOLDER     := $(ROOT)/Libraries/ImageIO/jpeg-9b

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


#$(JPEG_FOLDER)/jmemansi.c \
# .. instead of jmemnobs.c?

# Is the below using an integer DCT?  Might floating be faster
# under emscripten?

JPEG_FILES := \
  jerror.c \
  jutils.c \
  jcomapi.c \
  jmemmgr.c \
  jmemnobs.c \
  jdapimin.c \
  jdapistd.c \
  jdarith.c \
  jdatadst.c \
  jdatasrc.c \
  jdcoefct.c \
  jdcolor.c \
  jddctmgr.c \
  jdhuff.c \
  jdinput.c \
  jdmainct.c \
  jdmarker.c \
  jdmaster.c \
  jdmerge.c \
  jdpostct.c \
  jdsample.c \
  jdtrans.c \
  jidctflt.c \
  jidctfst.c \
  jidctint.c \
  jaricom.c \
  jquant1.c \
  jquant2.c

JPEG_OFILES := \
  $(JPEG_FILES:%.c=$(BUILD_FOLDER)/%.o)

all: build

build: $(BUILD_FOLDER) project

$(BUILD_FOLDER):
	mkdir -p $(BUILD_FOLDER)

$(BUILD_FOLDER)/%.o: $(JPEG_FOLDER)/%.c
	$(CC) $(CFLAGS) -O3 -c $< -o $@

clean:
	rm -f $(BUILD_FOLDER)/$(OUTPUT_NAME).*
	rm -rf $(BUILD_FOLDER)/Assets
	rm -f $(BUILD_FOLDER)/Source/RogueProgram.cpp
	rm -f $(BUILD_FOLDER)/Source/RogueProgram.h
	rm -f $(BUILD_FOLDER)/*.o

project: $(BUILD_FOLDER)/$(OUTPUT_NAME).html

$(BUILD_FOLDER)/$(OUTPUT_NAME).html: $(BUILD_FOLDER)/Source/RogueProgram.cpp $(JPEG_OFILES) $(SDLCORE_FILES)
	# -s FULL_ES2=1 ?
	$(CXX) -I$(BUILD_FOLDER)/Source \
	       -I$(JPEG_FOLDER) \
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
	       $(JPEG_OFILES) \
	       -o $(BUILD_FOLDER)/$(OUTPUT_NAME).html
