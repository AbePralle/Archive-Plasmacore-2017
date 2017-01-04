# Plasmacore

          | Current Release
----------|-----------------------
Version   | v0.6.4
Date      | December 16, 2016
Platforms | macOS
Targets   | macOS, iOS


## Notes
- Requires a Mac with Xcode and the iOS SDK.
- Plasmacore comes with an embedded copy of the Rogue language; you do not need to install Rogue separately.


## Bootstrap Command

To bootstrap a new Plasmacore-based project, open a Terminal in your new project folder and copy and paste the following command:

    curl -O https://raw.githubusercontent.com/AbePralle/Plasmacore/master/Bootstrap.mk && make -f Bootstrap.mk

The command will fetch a bootstrap makefile which in turn will `git clone` the latest Plasmacore repo in a temporary folder and copy all the files into the current folder.


## Documentation and Resources

There is some Rogue documentation here: [https://github.com/AbePralle/Rogue/wiki](https://github.com/AbePralle/Rogue/wiki)

There is no Plasmacore documentation yet.  You can manually browse the `Libraries/Rogue/Plasmacore` files.

A sample Plasmacore game project is available here: [https://github.com/AbePralle/PlasmacoreDemos](https://github.com/AbePralle/PlasmacoreDemos)


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

###v0.7.0 - January 3, 2016
- [emscripten] Plasmacore now supports an "empscripten" asm.js target through "make emscripten".
- [TextEvent] Separated the concepts of keypresses and text input.  `KeyEvent` no longer contains `unicode` values.  A new `TextEvent` has been added that contains `character:Character`, `text:Characters`, and a character `count()->Int32`.  Displays, Views, Layers, and States now have both `KeyEvent` and `TextEvent` handlers.
- [Layer] Added a "Layer" system to `View`.  Views now have a list of layers; each layer is automatically hooked into the event chain when it is added to a view with `View.add(Layer)`.  Most events such as `on_update` and `on_draw` are sent to layers in a first-added order; pointer and key events are dispatched in reverse order.
- [Layer] Layers have an `is_visible` property which defaults to true and can be affected by `Layer.hide()` and `Layer.show()`.  A hidden layer does not receive update, draw, or input events.
- [GameGUI] Added base classes for the "GameGUI" user interface system.  A `GameGUI` class is a Layer that various GGComponents can be added to.
- [CompositeImage] Restored CompositeImage class (was commented out).
- [Image] Images can now have a drawing callback: `Image.on_draw(Function(Box))` is used to set the callback.  The callback function is called after the basic image is drawn and while various transformations and settings are still in effect.  Particularly useful for composite images.
- [Image] Fixed premultiplied alpha issue with images (premultiplication was happening twice on iOS and macOS).
- [ProgressFn] Added the full set of standard easing functions to `ProgressFn.rogue`.
- [DefaultFont] Added new built-in font called `DefaultFont`.  The font is a 34 pixel high Arial Bold.
- [XY] Added `XY.or_larger(XY)` and `XY.or_smaller(XY)` the return the largest/smallest `x` and largest/smallest `y` between the context value and the parameter value.
- [Box] Added `Box.relative_to(Anchor)->Box`.
- [Messaging] Changed `Application` message names to `Application.on_launch`, `Application.on_start`, `Application.on_stop`, and `Application.on_save`.  Renamed message `Display.render` to `Display.on_render`.
- [Display] Added `Display.push_clipping_region(Box)` and `Display.pop_clipping_region()`.  Pushed clipping regions operate as an intersection with the current clipping region.
- [State] Modified behavior of `queue_state()` to perform the state chaining itself if its view is null rather than having the current Display perform the chaining.
- [ProgressFn] Added `ProgressFn.BOUNCE_OUT`.
- [Image Compiler] Added optional `monospace` and `monospace:numbers` attributes to `create image:...font:` directive.
- [Build System] Actually fixed IDE flag to be passed properly.

###v0.6.4 - December 16, 2016
- [Libraries] Deleted now-unused embedded FreeType and HarfBuzz libraries.

###v0.6.3 - December 15, 2016
- [Build System] Fixed IDE flag to be passed properly to roguec from Makefile and BuildScriptCore.rogue.
- [Build System] Fixed ImageCompiler BuildScript to use project `roguec` instead of global `roguec`.
- [View] Added a `state` setter than performs a `change_state()` call to avoid an incongruent error.
- [State] Added a `change_view()` method for symmetry with `View`'s `change_state()`.
- [Rogue] Updated Rogue to v1.1.14.

###v0.6.2 - December 13, 2016
- Fixed command line error invoking the Sound Compiler.

###v0.6.1 - December 13, 2016
- [Rogue] Rogue program now compiles to `.mm` for iOS and macOS which allows inline ObjectiveC to be used in addition to inline C++.  See `Libraries/Rogue/Plasmacore/iOS.rogue` for an example.
- [iOS] Added `iOS` class with global method `status_bar_height()->Int32` (pixels).

###v0.6.0 - December 12, 2016
- [ImageCompiler] Added command to generate font strip images from local or system fonts.  Syntax: `create image:<image_name.png> font:<"Font Name.ttf"> height:<pixels> [characters:"ABC..." unicode:[32-127] ...]`.  The image compiler will find the font by searching folders in the following order: `Assets/Fonts/`, `/Library/Fonts/`, `/System/Library/Fonts/`.
- [ImageCompiler] Moved Image Compiler off of vendored (local) PNG, JPEG, and FreeType libraries.  Now uses Brew-installed versions of those libraries.  Added companion `Libraries/ImageCompiler/BuildScript.rogue` to automatically manage the Brew-installed libraries during build.
- [BuildScript] Renamed `Build.rogue` to `BuildScript.rogue` and `BuildCore.rogue` to `BuildScriptCore.rogue`.
- [Bitmap] Converted `Bitmap(width:Int32,height:Int32)` to be a global constructor to make it easy to call from the native layer.
- [Bitmap] The default fill color of a Bitmap is now transparent black instead of opaque black.
- [ImageIO] Removed the "ImageIO" library from both iOS and Mac projects.  Now using Core Graphics to load images for a 25% speed boost.
- [Rogue] Updated to Rogue v1.1.13.

###v0.5.5 - December 8, 2016
- [OverlayLog] Fixed overlay log messages to be single spaced instead of double spaced.
- [Swift] Added `PlasmacoreMessage.getDictionary(name:String)`.
- [Messaging] Added `Message.send_rsvp(response_message_type:String,callback:Function(Message))`.  Rather than awaiting a formal reply to this specific message like `Message.send_rsvp(callback)` does, this version installs a global one-time message listener to call the callback; the listener is not actually associated with this message at all.
- [Rogue] Updated to Rogue v1.1.12.
- [Makefile] Created variables `REPO` and `BRANCH` that can be overridden during an update, e.g. `make update BRANCH=develop`.

###v0.5.4 - December 6, 2016
- [Message] Added helper methods for sending and receiving JSON-encoded `Value` objects using the message passing system: Message.send(name,Value), .value(name), .table(name), and .list(name).

###v0.5.3 - December 4, 2016
- [Rogue] Detailed Rogue error messages now show up in Xcode and can be jumped to by clicking on the error.
- [XY] Fixed error in `XY.magnitude()` (was incorrectly made into a `[macro]`).
- [Image] Added constructors for `Image(Color,[size=Display.size:XY])` and `Image(Colors,[XY])`.
- [Box] Added `Box.fill(Colors)` to complement existing `Box.fill(Color)`.
- [Plasmacore] Removed `Plasmacore.launch_window_name`.
- [ImageCompiler] Fixed bug that in some cases prevented image sheets from being reduced in pixel size as much as possible.
- [System] Augmented `System` class with global methods `System.is_desktop()->Logical` and `System.is_mobile()->Logical`.
- [Colors] Removed ambiguous constructor `Colors(Color*4)` that was already covered by `Colors(Color*4,Int32=4)`.
- [Rogue] Added appropriate `Desktop` or `Mobile` target to each compile for use with conditional compilation directives.  E.g. `$if ("Mobile") ...`.
- [Rogue] Updated to Rogue v1.1.11.
- [Swift] Renamed `PlasmacoreMessageHandler` to `PlasmacoreMessageListener` to match Rogue-side name.
- [Swift] Converted `Plasmacore.singleton.addMessageListener(...)` to `Plasmacore.addMessageListner(...)`; same for `removeMessageListener`.

###v0.5.2 - December 2, 2016
- [Mac Project] Added missing `Build/Mac/Assets` folder reference to Mac Project.

###v0.5.1 - December 2, 2016
- [Plasmacore] Added `on_resize()` and `after_resize()` to `View` and `State`, called before `update` and `draw` events when the Display.size has changed.
- [Plasmacore] Improved and streamlined the update and draw event API, removing the `on(UpdateEvent)->on_update` chain in favor of calling `on_update` directly; same with `on_draw`.

###v0.5.0 - December 1, 2016
- [Mac] Added Mac project.
- [OverlayLog] Overlay text now never scales below 1.0.
- [Makefile] Changed `make update` to use HTTPS instead of SSH for the `git clone` operation.

###v0.4.7 - November 24, 2016
- [Image] Added `original_size` property that tracks the image's original display size.

###v0.4.6 - November 24, 2016
- [Plasmacore] Fixed `Display.clipping_region` to work on iPhone Plus with regular and zoomed displays.

###v0.4.5 - November 15, 2016
- [Image Compiler] A non-"." image group that does not have a wildcard in the path now compiles correctly.

###v0.4.4 - November 15, 2016
- [iOS Project] An `Application.save` message now gets sent on `applicationWillResignActive` instead of `applicationDidEnterBackground` and `applicationWillTerminate`.
- [Rogue] Updated to Rogue v1.1.4

###v0.4.3 - November 13, 2016
- [Rogue] Fixed bug in `Value.to_json()` where `false` would print as `null`.

###v0.4.2 - November 13, 2016
- [Rogue] Fixed fatal compile error bug in 0.4.1 release.

###v0.4.1 - November 13, 2016
- [Rogue] Updated Rogue to v1.1.3.

###v0.4.0 - November 12, 2016
- [Plasmacore] Image compiler meta-info output format and corresponding Plasmacore image info loading system reworked for massive savings.  File sizes are reduced by half and runtime object counts are reduced by a factor of 10.
- [Box] Creating a `Box` from a `Value` with `Box(Value)` now works with value lists - `Box(@[3,4])` is equivalent to `Box(&size=XY(3,4))` and `Box(@[1,2,3,4])` is equivalent to `Box(&position=XY(1,2),&size=XY(3,4))`.
- [iOS] Now displays correctly on iPhone Plus - glViewport needs physical pixels (1080x1920) while the rest of the rendering system needs the internal display buffer size (1242x2208).

###v0.3.5 - November 7, 2016
- [App Lifecycle] Added State/View `on_app_start/stop/save` methods based on global `Application.start/stop/save` events.  Timers and such should be stopped and started with those methods.  `save()` is called when the app may or may not resume execution and should save its state.

###v0.3.4 - November 4, 2016
- [Image Compiler] Fixed bug where the same image being copied+resized more than once starts with the original image each time.

###v0.3.3 - November 3, 2016
- [Rogue] Updated to Rogue v1.1.2.

###v0.3.2 - November 1, 2016
- [Display] Added `Display.clipping_region:Box?` which sets up the "scissor test" that clips any drawn images to the given bounds.  Assign a valid box to define and enable the clipping region or `null` to disable it.
- [Image System] Removed kludgy `Plasmacore.default_asset_folder` in favor of a global property `Image.search_folders:String[]`.  Modify the folders as you like by adding e.g. `iPadRetina` to the `search_folders` list, allowing `Image("Name.png")` to be used rather than `Image("iPadRetina/Name.png")` etc.
- [Image System] Any path-shortened form of an image can now be used to load the image.  For example, `Image("iOS/BG.png")` can now be loaded as `Image("BG.png")`.

###v0.3.1 - October 31, 2016
- [Sound] Added `Sound.duration()->Real64` that returns the duration in seconds.
- [Sound] Added `Sound.is_finished()->Logical` that returns `true` if a sound is not playing and not paused.
- [Sound] Added `Sound.is_playing()->Logical`.
- [Sound] Added `Sound.is_repeating:Logical`.
- [Sound] Added `Sound.pause()` that stop a sound without rewinding it.  `play()` or `resume()` will play the sound again.
- [Sound] Added `Sound.play(is_repeating:Logical)` in addition to `Sound.play()`.
- [Sound] Added `Sound.position:Real64` that specifies a sound's playback position in seconds.
- [Sound] Added `Sound.resume()` that resumes playing a sound only if it has been previously paused.
- [Sound] Added `Sound.set_volume(Real64)` that accepts an argument between 0.0 and 1.0.
- [Sound] Added `Sound.stop()` that stops a sound and rewinds it to the beginning.
- [SoundGroup] Created new `SoundGroup` class to manage a set of related sounds, like multiple copies of a single sound effect or multiple variations of a sound.
- [SoundGroup] Added constructor `SoundGroup.init(sound_name:String,channels=1:Int32,&is_music)`.
- [SoundGroup] Added constructor `SoundGroup.init(&random,&random_order,&autoplay)`.  The `&random` flag picks a sound randomly each time.  `&random_order` is similar but it plays all sounds in a random order before repeating any of them, reshuffling each time before another pass.  `&autoplay` automatically plays another sound when each sound is finished - note `SoundGroup.play()` must be called once to start the process.
- [SoundGroup] Added `SoundGroup.add(Sound)` which adds another sound to the group.
- [SoundGroup] Added `SoundGroup.play()` that plays the next sound from the group.
- [SoundGroup] Added `SoundGroup.pause()` that pauses all sounds in the group.
- [SoundGroup] Added `SoundGroup.resume()` that resumes all paused sounds in the group.
- [SoundGroup] Added `SoundGroup.stop()` that halts playback of the group.

###v0.3.0 - October 31, 2016
- [Sound Compiler] Added new Sound Compiler that works similar to the Image Compiler.  It is automatically invoked during an Xcode build or you can manually `./scom iOS` to run it.  It uses the `Assets/Sounds/SoundConfig.txt` build script, creating a default version necessary.
- [Sound] Added `Sound` class.  Create a sound with `Sound(name:String,[channels=1:Int32])`.  `name` should be a filename in `Assets/Sounds/`; you can omit `Assets/Sounds/` as well as the extension.  `channels` is the number of times that sound can play concurrently - each additional number ends up creating a new sound player underneath so don't use more than 1 channel if you don't need to.
- [Sound] Call `Sound.play()` to play a sound.  Additional sound API calls will be added later.
- [AVFoundation Framework] You must manually add the `AVFoundation` framework to any existing projects.
- [Image Compiler] Renamed `imageCopy` command to `copy`.

###v0.2.3 - October 29, 2016
- [Image Compiler] Improved wildcard matching for `group` specifications.
- [Image Compiler] Changing `Assets/Images/ImageConfig.txt` no longer causes an image sheet recompile due to timestamp difference.  If you need to recompile existing images with new settings, do a `make clean` first.
- [TargetValue] Changed original initializer to default to `ProgressFn.QUADRATIC` instead of `LINEAR`.
- [TargetValue] Added new initializer `init(initial_value,duration_fn,progress_fn=ProgressFn.QUADRATIC)`.  `duration_fn` should be of type `Function($DataType)->(Real64)`, accepts a delta interval, and should return the desired duration for that interval.  The `TargetValue` will automatically adjust the duration as new target values are set, effectively allowing a rate-limited rather than time-based `TargetValue`.

###v0.2.2 - October 26, 2016
- [Image Compiler] Changed convention of Image Compiler to use `camelCase` commands with `serpent_case` arguments, like Rogue.
- [Image Compiler] Added `resize:WxH->WxH` option to Image Compiler's `imageCopy` command.
- [Image Compiler] Removed vestigial reference to 'freetype-2.6.3' folder in Image Compiler's Makefile.

###v0.2.1 - October 21, 2016
- [iOS Xcode Project] Added missing Swift files to iOS template project.

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
