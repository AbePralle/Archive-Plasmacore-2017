bootstrap_plasmacore:
	@[ -e Source ]    && echo "ERROR: Source/ folder exists; aborting bootstrap to prevent possible overwrite." && exit 1 || true
	@[ -e Platforms ] && echo "ERROR: Platforms/ folder exists; aborting bootstrap to prevent possible overwrite." && exit 1 || true
	@[ ! -e Bootstrap/Plasmacore ] && echo "Cloning Plasmacore master branch into Bootstrap/Plasmacore/" && mkdir -p Bootstrap && cd Bootstrap && git clone git@github.com:AbePralle/Plasmacore.git || true
	rm -rf Bootstrap/Plasmacore/.git
	@rsync -a --exclude="README.md" --exclude="UNLICENSE" --out-format="Copying %n%L" Bootstrap/Plasmacore/* .
	@rsync -a --out-format="Copying %n%L" --ignore-existing Bootstrap/Plasmacore/.gitattributes .
	@rsync -a --out-format="Copying %n%L" --ignore-existing Bootstrap/Plasmacore/.gitignore .
	rm -rf Bootstrap
	@echo "Done!"
