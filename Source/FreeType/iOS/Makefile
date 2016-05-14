IOS_SIMULATOR_SDK = /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator9.3.sdk
IOS_SDK = /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS9.3.sdk
MIN_OS = -mios-simulator-version-min=9.3
FREETYPE_FOLDER = ../freetype-2.6.3
BUILD_FOLDER = ../iOS/Build
OUTPUT_FOLDER = ../iOS
CC = clang
AR_EXE = /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ar

test:
	cd $(FREETYPE_FOLDER) && ./configure --without-zlib --without-png --without-bzip2 '--enable-static=yes' '--enable-shared=no' 'CFLAGS=-arch x86_64 -pipe -std=c99 -Wno-trigraphs -fpascal-strings -O2 -Wreturn-type -Wunused-variable -fmessage-length=0 -fvisibility=hidden $(MIN_OS) -I$(IOS_SIMULATOR_SDK)/usr/include/libxml2/ -isysroot $(IOS_SIMULATOR_SDK)/' 'AR=$(AR_EXE)' 'LDFLAGS=-arch x86_64 -isysroot $(IOS_SIMULATOR_SDK)/ $(MIN_OS)' 'CC=$(CC)'
	#cd $(FREETYPE_FOLDER) && ./configure CFLAGS="-arch x86_64" --without-png --without-bzip2 '--prefix=/usr/local/iPhone'
	cd $(FREETYPE_FOLDER) && make clean
	cd $(FREETYPE_FOLDER) && make
	cd $(FREETYPE_FOLDER) && cp objs/.libs/libfreetype.a $(BUILD_FOLDER)/libfreetype-x86_64.a

all:
	mkdir -p $(BUILD_FOLDER)
	cd $(FREETYPE_FOLDER) && ./configure CFLAGS="-arch i386" --without-png --without-bzip2
	cd $(FREETYPE_FOLDER) && make clean
	cd $(FREETYPE_FOLDER) && make
	cd $(FREETYPE_FOLDER) && cp objs/.libs/libfreetype.a $(BUILD_FOLDER)/libfreetype-i386.a
	\
	cd $(FREETYPE_FOLDER) && ./configure CFLAGS="-arch x86_64" --without-png --without-bzip2
	cd $(FREETYPE_FOLDER) && make clean
	cd $(FREETYPE_FOLDER) && make
	cd $(FREETYPE_FOLDER) && cp objs/.libs/libfreetype.a $(BUILD_FOLDER)/libfreetype-x86_64.a
	\
	cd $(FREETYPE_FOLDER) && ./configure --without-zlib --without-png --without-bzip2 '--prefix=/usr/local/iPhone' '--host=arm-apple-darwin' '--enable-static=yes' '--enable-shared=no' 'CC=/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang' 'CFLAGS=-arch armv7 -pipe -std=c99 -Wno-trigraphs -fpascal-strings -O2 -Wreturn-type -Wunused-variable -fmessage-length=0 -fvisibility=hidden -miphoneos-version-min=$(MIN_OS) -I/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/$(IOS_SDK)/usr/include/libxml2/ -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/$(IOS_SDK)/' 'AR=$(AR_EXE)' 'LDFLAGS=-arch armv7 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/$(IOS_SDK)/ -miphoneos-version-min=$(MIN_OS)'
	cd $(FREETYPE_FOLDER) && make clean
	cd $(FREETYPE_FOLDER) && make
	cd $(FREETYPE_FOLDER) && cp objs/.libs/libfreetype.a $(BUILD_FOLDER)/libfreetype-armv7.a
	\
	cd $(FREETYPE_FOLDER) && ./configure --without-zlib --without-png --without-bzip2 '--prefix=/usr/local/iPhone' '--host=arm-apple-darwin' '--enable-static=yes' '--enable-shared=no' 'CC=/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang' 'CFLAGS=-arch arm64 -pipe -std=c99 -Wno-trigraphs -fpascal-strings -O2 -Wreturn-type -Wunused-variable -fmessage-length=0 -fvisibility=hidden -miphoneos-version-min=$(MIN_OS) -I/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/$(IOS_SDK)/usr/include/libxml2/ -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/$(IOS_SDK)/' 'AR=$(AR_EXE)' 'LDFLAGS=-arch arm64 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/$(IOS_SDK)/ -miphoneos-version-min=$(MIN_OS)'
	cd $(FREETYPE_FOLDER) && make clean
	cd $(FREETYPE_FOLDER) && make
	cd $(FREETYPE_FOLDER) && cp objs/.libs/libfreetype.a $(BUILD_FOLDER)/libfreetype-arm64.a
	\
	cd $(FREETYPE_FOLDER) && ./configure --without-zlib --without-png --without-bzip2 '--prefix=/usr/local/iPhone' '--host=arm-apple-darwin' '--enable-static=yes' '--enable-shared=no' 'CC=/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang' 'CFLAGS=-arch armv7s -pipe -std=c99 -Wno-trigraphs -fpascal-strings -O2 -Wreturn-type -Wunused-variable -fmessage-length=0 -fvisibility=hidden -miphoneos-version-min=$(MIN_OS) -I/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/$(IOS_SDK)/usr/include/libxml2/ -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/$(IOS_SDK)/' 'AR=$(AR_EXE)' 'LDFLAGS=-arch armv7s -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/$(IOS_SDK)/ -miphoneos-version-min=$(MIN_OS)'
	cd $(FREETYPE_FOLDER) && make clean
	cd $(FREETYPE_FOLDER) && make
	cd $(FREETYPE_FOLDER) && cp objs/.libs/libfreetype.a $(BUILD_FOLDER)/libfreetype-armv7s.a
	\
	lipo -create -output $(OUTPUT_FOLDER)/libfreetype.a $(BUILD_FOLDER)/libfreetype-i386.a $(BUILD_FOLDER)/libfreetype-x86_64.a $(BUILD_FOLDER)/libfreetype-armv7.a $(BUILD_FOLDER)/libfreetype-armv7s.a $(BUILD_FOLDER)/libfreetype-arm64.a
