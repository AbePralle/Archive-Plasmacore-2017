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

pull:
	@# Run the local makefile if it exists
	@[ -f Local.mk ] && make -f Local.mk pull || true

update:
	@# Run the local makefile if it exists
	@[ -f Local.mk ] && make -f Local.mk update || true

status:
	@# Run the local makefile if it exists
	@[ -f Local.mk ] && make -f Local.mk status || true

#------------------------------------------------------------------------------
# Sample Local.mk file - do not commit to repo

#ABES_LIBS = ..
#
#status:
#	@printf "SuperCPP   - "
#	@cd $(ABES_LIBS)/SuperCPP && git status | grep "\(Your branch is\)\|\(Changes not staged\|Untracked files\)"
#	@printf "ImageIO    - "
#	@cd $(ABES_LIBS)/ImageIO && git status | grep "\(Your branch is\)\|\(Changes not staged\|Untracked files\)"
#	@printf "Starbright - "
#	@cd $(ABES_LIBS)/Starbright && git status | grep "\(Your branch is\)\|\(Changes not staged\|Untracked files\)"
#	@printf "Plasmacore - "
#	@git status | grep "\(Your branch is\)\|\(Changes not staged\|Untracked files\)"
#
#pull:
# cd $(ABES_LIBS)/SuperCPP && git pull
# cd $(ABES_LIBS)/ImageIO && git pull
# cd $(ABES_LIBS)/Starbright && git pull
#
#update:
# @rsync -a --exclude=".*" --delete --out-format="Updating %n%L" $(ABES_LIBS)/SuperCPP/Source/SuperCPP Source
# @rsync -a --exclude=".*" --delete --out-format="Updating %n%L" $(ABES_LIBS)/ImageIO/Source/ImageIO Source
# @rsync -a --exclude=".*" --delete --out-format="Updating %n%L" $(ABES_LIBS)/Starbright/Source/Starbright Source

