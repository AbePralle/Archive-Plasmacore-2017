PLASMACORE_ROGUE_SOURCE = $(shell find Source/Rogue | grep "\.rogue$$") $(shell find Source/Plasmacore/Rogue/Plasmacore | grep "\.rogue$$")

all: compile_rogue

compile_rogue: Source/Rogue/Build/RogueProgram.cpp

Source/Rogue/Build/RogueProgram.cpp: $(PLASMACORE_ROGUE_SOURCE)
	mkdir -p Source/Rogue/Build
	cd Source/Rogue/ && roguec Main.rogue Plasmacore --libraries="../Plasmacore/Rogue" --output=Build/RogueProgram

remake:
	mkdir -p Source/Rogue/Build
	cd Source/Rogue/ && roguec Main.rogue Plasmacore --libraries="../Plasmacore/Rogue" --output=Build/RogueProgram

clean:
	rm -rf Source/Rogue/Build

update:
	@# Run the local makefile if it exists
	@[ -f Local.mk ] && make -f Local.mk || true

#------------------------------------------------------------------------------
# Sample Local.mk file - do not commit to repo

#ABES_LIBS = ..
#
#all:
#	@rsync -a --exclude=".*" --delete --out-format="Updating %n%L" $(ABES_LIBS)/SuperCPP/Source/SuperCPP Source
#	@rsync -a --exclude=".*" --delete --out-format="Updating %n%L" $(ABES_LIBS)/ImageIO/Source/ImageIO Source
#	@rsync -a --exclude=".*" --delete --out-format="Updating %n%L" $(ABES_LIBS)/Starbright/Source/Starbright Source

