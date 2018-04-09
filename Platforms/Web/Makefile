OPT_CFLAGS += -O3
OPT_CFLAGS += -s ASSERTIONS=0 -s PRECISE_I64_MATH=0 --closure 1

# 160 MiB by default (asm.js wants a multiple of 16 MiB)
RUNTIME_MEMORY = 167772160
RUNTIME_MEMORY = $(shell echo 16777216*4 | bc)

PROJECT_ROOT = ../..

# Overridden by "PROJECT_NAME := ..." definition in Settings.mk, if present
PROJECT_NAME := $(shell basename $(shell cd $(PROJECT_ROOT) && pwd))

TARGET = Web
CXX = em++
CC = emcc

SDLCORE_FOLDER = $(PROJECT_ROOT)/Libraries/Plasmacore/SDL

BUILD_FOLDER    := $(PROJECT_ROOT)/Build/$(TARGET)

#CFLAGS += --shell-file $(PROJECT_ROOT)/Source/shell.html

SDLCORE_FILES := \
  Main.cpp \
  $(SDLCORE_FOLDER)/Plasmacore.cpp \
  $(SDLCORE_FOLDER)/PlasmacoreMessage.cpp \
  $(SDLCORE_FOLDER)/PlasmacoreView.cpp \
  $(SDLCORE_FOLDER)/PlasmacoreSoundSDLMixer.cpp \
  $(SDLCORE_FOLDER)/PlasmacoreUtility.cpp \
  $(SDLCORE_FOLDER)/RogueInterface.cpp

-include Settings.mk

all: create_default_settings build

build: $(BUILD_FOLDER)/Intermediate project

$(BUILD_FOLDER)/Intermediate:
	mkdir -p $(BUILD_FOLDER)/Intermediate

clean:
	rm -f $(BUILD_FOLDER)/$(PROJECT_NAME).*
	rm -rf $(BUILD_FOLDER)/Assets
	rm -f $(BUILD_FOLDER)/Source/RogueProgram.cpp
	rm -f $(BUILD_FOLDER)/Source/RogueProgram.h
	rm -f $(BUILD_FOLDER)/Intermediate/*.o

project: $(BUILD_FOLDER)/$(PROJECT_NAME).html index_html html_assets

$(BUILD_FOLDER)/$(PROJECT_NAME).html: $(BUILD_FOLDER)/Source/RogueProgram.cpp $(SDLCORE_FILES)
	# -s FULL_ES2=1 ?
	$(CXX) -I$(BUILD_FOLDER)/Source \
	       -I$(SDLCORE_FOLDER) \
	       -s USE_SDL=2 \
	       -s USE_LIBPNG=1 \
	       -s EXPORTED_FUNCTIONS="['_main', '_malloc', '_launch_plasmacore']" \
               -s TOTAL_MEMORY=$(RUNTIME_MEMORY) \
	       -D LOCAL_FS='"/IDBFS"' \
	       -std=c++11 \
	       --preload-file $(BUILD_FOLDER)/Assets@/Assets \
	       -Wall \
	       -fno-strict-aliasing \
	       $(OPT_CFLAGS) \
	       $(CXXFLAGS) \
	       $(CFLAGS) \
	       $(BUILD_FOLDER)/Source/RogueProgram.cpp \
	       $(SDLCORE_FILES) \
	       -o $(BUILD_FOLDER)/$(PROJECT_NAME).html

index_html:
	@test -f index.html && make $(BUILD_FOLDER)/index.html || true

html_assets:
	@test -d HTMLAssets && rsync -a -c --exclude=".*" --delete --out-format="Updating Build/Web/%n%L" HTMLAssets $(BUILD_FOLDER) || true

$(BUILD_FOLDER)/index.html: index.html
	cp index.html $(BUILD_FOLDER)/

create_default_settings:
	@test -f Settings.mk || echo PROJECT_NAME := $(PROJECT_NAME) >> Settings.mk
