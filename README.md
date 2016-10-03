# Plasmacore
- v0.1.0
- October 2, 2016

## Installation (Mac, maybe Linux)
    git clone git@github.com:Plasmaworks/Rogue.git
    cd Rogue
    make

**NOTES**

1. Running make will compile the `roguec` compiler from C++ source and create the executable script `/usr/local/bin/roguec`

2. Run `roguec` by itself for options.

3. Execute these shell commands for a simple test:

        echo println '"Hello World!"' > Hello.rogue
        roguec Hello.rogue --execute


## License
Rogue is released into the Public Domain under the terms of the [Unlicense](http://unlicense.org/).
