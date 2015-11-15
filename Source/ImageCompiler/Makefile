.PHONY = build

all: build run

build: Build/imagecompiler

Build/imagecompiler: Main.cpp Build/ImageCompiler.o
	cd Build && g++ ../Main.cpp ImageCompiler.o -o imagecompiler

Build/ImageCompiler.o: Build/ImageCompiler.cpp Build/ImageCompiler.h
	cd Build && g++ -c ImageCompiler.cpp

Build/ImageCompiler.cpp: ImageCompiler.rogue
	mkdir -p Build
	roguec ImageCompiler.rogue --output=Build

run:
	Build/imagecompiler
