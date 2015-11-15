.PHONY = build

CPP_FLAGS := -I../../lpng1619

all: build run

build: Build/imagecompiler

Build/imagecompiler: Main.cpp Build/ImageCompiler.o
	cd Build && g++ $(CPP_FLAGS) ../Main.cpp ImageCompiler.o -o imagecompiler

Build/ImageCompiler.o: Build/ImageCompiler.cpp Build/ImageCompiler.h
	cd Build && g++ $(CPP_FLAGS) -c ImageCompiler.cpp

Build/ImageCompiler.cpp: ImageCompiler.rogue
	mkdir -p Build
	roguec ImageCompiler.rogue --output=Build

run:
	Build/imagecompiler
