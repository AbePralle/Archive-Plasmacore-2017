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

