all: compile

compile: Build/RogueProgram.cpp

Build/RogueProgram.cpp: Main.rogue
	mkdir -p Build
	roguec Main.rogue --output=Build/RogueProgram

