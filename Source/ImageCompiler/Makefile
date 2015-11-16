.PHONY = build

LIB_PNG_FOLDER := ../lpng1619

CCFLAGS := -I$(LIB_PNG_FOLDER) -Wall

LIB_PNG_SOURCE := \
  png.c \
  pngerror.c \
  pngget.c \
  pngmem.c \
  pngpread.c \
  pngread.c \
  pngrio.c \
  pngrtran.c \
  pngrutil.c \
  pngset.c \
  pngtrans.c \
  pngwio.c \
  pngwrite.c \
  pngwtran.c \
  pngwutil.c

LIB_PNG_O_FILES := $(LIB_PNG_SOURCE:%.c=Build/%.o)

all: build run

build: Build/imagecompiler

Build/imagecompiler: Main.cpp Build/ImageCompiler.o $(LIB_PNG_O_FILES)
	g++ $(CCFLAGS) Main.cpp Build/ImageCompiler.o $(LIB_PNG_O_FILES) -lz -o Build/imagecompiler

Build/ImageCompiler.o: Build/ImageCompiler.cpp Build/ImageCompiler.h
	g++ $(CCFLAGS) -c Build/ImageCompiler.cpp -o Build/ImageCompiler.o

Build/ImageCompiler.cpp: ImageCompiler.rogue
	mkdir -p Build
	roguec ImageCompiler.rogue --output=Build

Build/%.o: $(LIB_PNG_FOLDER)/%.c
	gcc $(CCFLAGS) -c $< -o $@

run:
	Build/imagecompiler

clean:
	rm -rf Build/*
