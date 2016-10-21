# Plasmacore
- v0.2.0
- October 20, 2016

## Requirements
- Mac with Xcode and iOS SDK.
- Plasmacore comes with an embedded copy of the Rogue language; you do not need to install Rogue separately.

## Supported Platforms

Currently the Plasmacore toolchain only runs on MacOS and only supports an iOS compile target.

Platform | Status | Notes
---------|--------|---------------------------------------------
iOS      | Alpha  | No sound or direct virtual keyboard support.

Other platforms will eventually be supported.


## Documentation and Resources

There is some Rogue documentation here: [https://github.com/AbePralle/Rogue/wiki](https://github.com/AbePralle/Rogue/wiki)

There is no Plasmacore documentation yet.  You can manually browse the `Libraries/Rogue/Plasmacore` files.

A sample Plasmacore game project is available here: [https://github.com/AbePralle/PlasmacoreDemos](https://github.com/AbePralle/PlasmacoreDemos)


## Bootstrap Command

To bootstrap a new Plasmacore-based project, open a Terminal in your new project folder and copy and paste the following command:

    curl -O https://raw.githubusercontent.com/AbePralle/Plasmacore/master/Bootstrap.mk && make -f Bootstrap.mk

The command will fetch a bootstrap makefile which in turn will `git clone` the latest Plasmacore repo in a temporary folder and copy all the files into the current folder.


## Starting a New Project

1.  Run the bootstrap command or manually clone the Plasmacore repo and copy everything except the `.git` folder into your project folder.
2.  At the command line run `make ios`.  The first build will take a while as intermediate files are compiled.
3.  Open `Platforms/iOS/iOS-Project.xcodeproj` in Xcode and run on the simulator or a device.  You should see a blue screen.
4.  Edit `Source/Main.rogue` and add more game code.
5.  Either run `make ios` again or just compile and run in Xcode again as a build phase automatically runs `make ios`.  If you get an error in Xcode and you can't tell what it is, run `make ios` on the command line and you will see the compiler error message.
6.  Add images to Assets/Images and load them by name - `Image("img.png")`, `Font("SomeFont.png")`, etc.


## Updating an Existing Project

`make update` will update your current project to the latest version of Plasmacore (via `git` and `rsync`) without touching any game-specific files.


## License
Plasmacore is released into the Public Domain under the terms of the [Unlicense](http://unlicense.org/).

## Change Log

###v0.2.0 - October 20, 2016
- [Image Compiler] Replaced complex Asset Manager (`AM`) with simple Image Compiler (`ICOM`) that works off the script `Assets/Images/ImageConfig.txt`.  ICOM will automatically create a default config script if that is missing.  ICOM is automatically run as part of the `make ios` build.

###v0.1.2 - October 3, 2016
- [Bootstrap] Instead of copying all hidden files, now copies only `.gitattributes` and `.gitignore` and uses the `--ignore-existing` rsync option.
- [ActionCmd] Added `ActionCmd(Function())` constructor that creates an action command that calls the given function when invoked.
- [Image] `Image(existing:Image,subset:Box)` now works correctly even when the `existing` image hasn't been loaded yet.
- [Image] Renamed `alpha` to `opacity` (controls master opacity, defaults to `1.0`).
- [Image] Fixed `opacity` to work correctly with opaque images (was previously not enabling blending).
- [Font]  Renamed `alpha` to `opacity`; opacity now actually applied to font drawing.
- [TargetValue] Consolidated `TimeLimitedTargetValue` and `RateLimitedTargetValue` into single class `TargetValue`.
- [TargetValue] A `start_time` of 0 now signals a finished state internally; once target value is reached then `start_time` is set to 0 to avoid subsequent duration changes "restarting" animation.
- [XY] Added `operator==(XY)` to provide a better implementation than the current Rogue-generated default.

###v0.1.1 - October 3, 2016
- [Bootstrap] Added bootstrap mechanism detailed in README.

###v0.1.0 - October 2, 2016
- Initial release.
