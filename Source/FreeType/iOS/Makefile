IOS_SDK = iPhoneOS9.3.sdk
MIN_OS = 7.0
FREETYPE_FOLDER = ../freetype-2.6.3
OUTPUT_FOLDER = ../iOS

all:
	cd $(FREETYPE_FOLDER) && ./configure CFLAGS="-arch i386" --without-png --without-bzip2
	cd $(FREETYPE_FOLDER) && make clean
	cd $(FREETYPE_FOLDER) && make
	cd $(FREETYPE_FOLDER) && cp objs/.libs/libfreetype.a $(OUTPUT_FOLDER)/libfreetype-i386.a

#./configure CFLAGS="-arch x86_64"
#make clean
#make
#cp objs/.libs/libfreetype.a libfreetype-x86_64.a
#
#./configure --without-zlib --without-png --without-bzip2 '--prefix=/usr/local/iPhone' '--host=arm-apple-darwin' '--enable-static=yes' '--enable-shared=no' 'CC=/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang' 'CFLAGS=-arch armv7 -pipe -std=c99 -Wno-trigraphs -fpascal-strings -O2 -Wreturn-type -Wunused-variable -fmessage-length=0 -fvisibility=hidden -miphoneos-version-min=$(MIN_OS) -I/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/$(IOS_SDK)/usr/include/libxml2/ -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/$(IOS_SDK)/' 'AR=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/ar' 'LDFLAGS=-arch armv7 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/$(IOS_SDK)/ -miphoneos-version-min=$(MIN_OS)'
#make clean
#make
#cp objs/.libs/libfreetype.a libfreetype-armv7.a
#
#./configure --without-zlib --without-png --without-bzip2 '--prefix=/usr/local/iPhone' '--host=arm-apple-darwin' '--enable-static=yes' '--enable-shared=no' 'CC=/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang' 'CFLAGS=-arch arm64 -pipe -std=c99 -Wno-trigraphs -fpascal-strings -O2 -Wreturn-type -Wunused-variable -fmessage-length=0 -fvisibility=hidden -miphoneos-version-min=$(MIN_OS) -I/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/$(IOS_SDK)/usr/include/libxml2/ -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/$(IOS_SDK)/' 'AR=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/ar' 'LDFLAGS=-arch arm64 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/$(IOS_SDK)/ -miphoneos-version-min=$(MIN_OS)'
#make clean
#make
#cp objs/.libs/libfreetype.a libfreetype-arm64.a
#
#./configure --without-zlib --without-png --without-bzip2 '--prefix=/usr/local/iPhone' '--host=arm-apple-darwin' '--enable-static=yes' '--enable-shared=no' 'CC=/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang' 'CFLAGS=-arch armv7s -pipe -std=c99 -Wno-trigraphs -fpascal-strings -O2 -Wreturn-type -Wunused-variable -fmessage-length=0 -fvisibility=hidden -miphoneos-version-min=$(MIN_OS) -I/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/$(IOS_SDK)/usr/include/libxml2/ -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/$(IOS_SDK)/' 'AR=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/ar' 'LDFLAGS=-arch armv7s -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/$(IOS_SDK)/ -miphoneos-version-min=$(MIN_OS)'
#make clean
#make
#cp objs/.libs/libfreetype.a libfreetype-armv7s.a
#
#lipo -create -output libfreetype.a libfreetype-i386.a libfreetype-x86_64.a libfreetype-armv7.a libfreetype-armv7s.a libfreetype-arm64.a
