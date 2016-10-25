#AM_CCFLAGS     += -O3
IMAGE_IO_CCFLAGS += -O3

ROGUEC        = ../../Programs/Mac/roguec --libraries=../../Libraries/Rogue
ROGUEC_FLAGS += --target="C++,Console"
ROGUEC_FLAGS += --debug

HARFBUZZ_FOLDER := harfbuzz-1.2.6
PNG_FOLDER      := ImageIO/Source
JPEG_FOLDER     := ImageIO/Source
IMAGE_IO_FOLDER := ImageIO/Source

BUILD_FOLDER          := Build
HARFBUZZ_BUILD_FOLDER := ../HarfBuzz/Build


ROGUE_LIBS = $(shell find ../Rogue | grep "\.rogue$$")

PNG_FILES := \
  $(PNG_FOLDER)/png.c \
  $(PNG_FOLDER)/pngerror.c \
  $(PNG_FOLDER)/pngget.c \
  $(PNG_FOLDER)/pngmem.c \
  $(PNG_FOLDER)/pngpread.c \
  $(PNG_FOLDER)/pngread.c \
  $(PNG_FOLDER)/pngrio.c \
  $(PNG_FOLDER)/pngrtran.c \
  $(PNG_FOLDER)/pngrutil.c \
  $(PNG_FOLDER)/pngset.c \
  $(PNG_FOLDER)/pngtrans.c \
  $(PNG_FOLDER)/pngwio.c \
  $(PNG_FOLDER)/pngwrite.c \
  $(PNG_FOLDER)/pngwtran.c \
  $(PNG_FOLDER)/pngwutil.c

JPEG_FILES := \
  $(JPEG_FOLDER)/jaricom.c \
  $(JPEG_FOLDER)/jcapimin.c \
  $(JPEG_FOLDER)/jcapistd.c \
  $(JPEG_FOLDER)/jcarith.c \
  $(JPEG_FOLDER)/jccoefct.c \
  $(JPEG_FOLDER)/jccolor.c \
  $(JPEG_FOLDER)/jcdctmgr.c \
  $(JPEG_FOLDER)/jchuff.c \
  $(JPEG_FOLDER)/jcinit.c \
  $(JPEG_FOLDER)/jcmainct.c \
  $(JPEG_FOLDER)/jcmarker.c \
  $(JPEG_FOLDER)/jcmaster.c \
  $(JPEG_FOLDER)/jcomapi.c \
  $(JPEG_FOLDER)/jcparam.c \
  $(JPEG_FOLDER)/jcprepct.c \
  $(JPEG_FOLDER)/jcsample.c \
  $(JPEG_FOLDER)/jctrans.c \
  $(JPEG_FOLDER)/jdapimin.c \
  $(JPEG_FOLDER)/jdapistd.c \
  $(JPEG_FOLDER)/jdarith.c \
  $(JPEG_FOLDER)/jdatadst.c \
  $(JPEG_FOLDER)/jdatasrc.c \
  $(JPEG_FOLDER)/jdcoefct.c \
  $(JPEG_FOLDER)/jdcolor.c \
  $(JPEG_FOLDER)/jddctmgr.c \
  $(JPEG_FOLDER)/jdhuff.c \
  $(JPEG_FOLDER)/jdinput.c \
  $(JPEG_FOLDER)/jdmainct.c \
  $(JPEG_FOLDER)/jdmarker.c \
  $(JPEG_FOLDER)/jdmaster.c \
  $(JPEG_FOLDER)/jdmerge.c \
  $(JPEG_FOLDER)/jdpostct.c \
  $(JPEG_FOLDER)/jdsample.c \
  $(JPEG_FOLDER)/jdtrans.c \
  $(JPEG_FOLDER)/jerror.c \
  $(JPEG_FOLDER)/jfdctflt.c \
  $(JPEG_FOLDER)/jfdctfst.c \
  $(JPEG_FOLDER)/jfdctint.c \
  $(JPEG_FOLDER)/jidctflt.c \
  $(JPEG_FOLDER)/jidctfst.c \
  $(JPEG_FOLDER)/jidctint.c \
  $(JPEG_FOLDER)/jmemansi.c \
  $(JPEG_FOLDER)/jmemmgr.c \
  $(JPEG_FOLDER)/jquant1.c \
  $(JPEG_FOLDER)/jquant2.c \
  $(JPEG_FOLDER)/jutils.c

IMAGE_IO_OFILES := \
  $(PNG_FILES:%.c=$(BUILD_FOLDER)/%.o) \
  $(JPEG_FILES:%.c=$(BUILD_FOLDER)/%.o)

AM_CCFLAGS += -I../$(PNG_FOLDER)
AM_CCFLAGS += -I../$(JPEG_FOLDER)
AM_CCFLAGS += -I../FreeType/Mac/Include
AM_CCFLAGS += -I../$(HARFBUZZ_FOLDER)
AM_CCFLAGS += -I../$(HARFBUZZ_FOLDER)/hb-ucdn

AM_CCFLAGS += -L$(HARFBUZZ_BUILD_FOLDER)
AM_CCFLAGS += -L../FreeType/Mac

AM_CCFLAGS += -lfreetype
AM_CCFLAGS += -lharfbuzz
AM_CCFLAGS += -lz
AM_CCFLAGS += -lbz2

all: update build

build: $(BUILD_FOLDER) $(BUILD_FOLDER)/$(IMAGE_IO_FOLDER) $(HARFBUZZ_BUILD_FOLDER)/libharfbuzz.a imagecompiler

$(BUILD_FOLDER):
	mkdir -p $(BUILD_FOLDER)

$(BUILD_FOLDER)/$(IMAGE_IO_FOLDER):
	mkdir -p $(BUILD_FOLDER)/$(IMAGE_IO_FOLDER)

$(BUILD_FOLDER)/%.o: ../%.cpp
	clang $(IMAGE_IO_CCFLAGS) -c $< -o $@

$(BUILD_FOLDER)/%.o: ../%.c
	clang $(IMAGE_IO_CCFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_FOLDER)

$(HARFBUZZ_BUILD_FOLDER)/libharfbuzz.a:
	make -C ../HarfBuzz

imagecompiler: $(BUILD_FOLDER)/imagecompiler

$(BUILD_FOLDER)/imagecompiler: ImageCompiler.rogue $(ROGUE_LIBS) $(IMAGE_IO_OFILES)
	$(ROGUEC) $(ROGUEC_FLAGS) ImageCompiler.rogue --main --output=$(BUILD_FOLDER)/ImageCompiler
	@echo Compiling ImageCompiler...
	@clang++ -Wall $(AM_CCFLAGS) -std=c++11 -fno-strict-aliasing $(BUILD_FOLDER)/ImageCompiler.cpp $(IMAGE_IO_OFILES) -o $(BUILD_FOLDER)/imagecompiler

update:
	@# Run the local makefile if it exists
	@[ -f Local.mk ] && make -f Local.mk update || true

#ABES_LIBS = ..
#
#update:
#	@rsync -a --exclude=".*" --delete --out-format="Updating %n%L" $(ABES_LIBS)/Plasmacore/Source/Plasmacore/Rogue/Plasmacore ../Rogue

