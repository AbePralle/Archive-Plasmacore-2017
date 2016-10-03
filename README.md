# Plasmacore
- v0.1.0
- October 2, 2016


## Supported Platforms

Platform | Notes
---------|--------------------------------------
iOS      | No sound or virtual keyboard support.

Other platforms will eventually be supported.


## Documentation and Resources

There is some Rogue documentation here: [https://github.com/AbePralle/Rogue/wiki](https://github.com/AbePralle/Rogue/wiki)

There is not yet any Plasmacore documentation.  You can manually browse the `Libraries/Rogue/Plasmacore` files.  A sample project is coming soon.


## Starting a New Project

1.  Clone the Plasmacore repo and either delete the `.git` folder or copy everything except the `.git` folder into a new folder.
2.  At the command line run `make ios`.  The first build will take a while as intermediate files are compiled.
3.  Open `Platforms/iOS/iOS-Project.xcodeproj` in Xcode and run on the simulator or a device.  You should see a blue screen.
4.  Edit `Source/Main.rogue` and add more game code.
5.  Either run `make ios` again or just compile and run in Xcode again as a build phase automatically runs `make ios`.  If you get an error in Xcode and you can't tell what it is, run `make ios` on the command line and you will see the compiler error message.
6.  Add images to Assets/Images and then run `./am` (Asset Manager).  In Asset Manager you should create a platform `iOS` and a group `General` (or any name).  In `am`, `cd Images` and then `add *`.  Those images will be available to your program the next time you compile.


## Updating an Existing Project

`make update` will update your current project to the latest version of Plasmacore (via `git` and `rsync`) without touching any game-specific files.


## License
Plasmacore is released into the Public Domain under the terms of the [Unlicense](http://unlicense.org/).

