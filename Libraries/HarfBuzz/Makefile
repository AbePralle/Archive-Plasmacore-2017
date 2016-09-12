HARFBUZZ_FOLDER := harfbuzz-1.2.6/src
FREETYPE_FOLDER := ../FreeType/Mac/Include

BUILD_FOLDER := Build/Intermediate
LIB_FILEPATH := $(BUILD_FOLDER:Intermediate=libharfbuzz.a)

CCFLAGS += -Wall
CCFLAGS += -O3
CCFLAGS += -I$(HARFBUZZ_FOLDER)
CCFLAGS += -I$(HARFBUZZ_FOLDER)/hb-ucdn
CCFLAGS += -I../FreeType/Mac/Include
CCFLAGS += -DHAVE_FREETYPE
CCFLAGS += -DHAVE_UCDN
CCFLAGS += -DHAVE_OT
CCFLAGS += -DHB_NO_MT

HARFBUZZ_CC_FILES := \
	hb-ot-shape-complex-use-table.cc \
	hb-ft.cc \
	hb-ot-shape-complex-use.cc \
	hb-ot-shape-complex-hangul.cc \
	hb-ot-shape-complex-hebrew.cc \
	hb-ot-shape-complex-tibetan.cc \
	hb-blob.cc \
	hb-buffer-serialize.cc \
	hb-buffer.cc \
	hb-common.cc \
	hb-face.cc \
	hb-fallback-shape.cc \
	hb-font.cc \
	hb-ot-font.cc \
	hb-ot-layout.cc \
	hb-ot-map.cc \
	hb-ot-shape-complex-arabic.cc \
	hb-ot-shape-complex-default.cc \
	hb-ot-shape-complex-indic-table.cc \
	hb-ot-shape-complex-indic.cc \
	hb-ot-shape-complex-myanmar.cc \
	hb-ot-shape-complex-thai.cc \
	hb-ot-shape-fallback.cc \
	hb-ot-shape-normalize.cc \
	hb-ot-shape.cc \
	hb-ot-tag.cc \
	hb-set.cc \
	hb-shape-plan.cc \
	hb-shape.cc \
	hb-shaper.cc \
	hb-ucdn.cc \
	hb-unicode.cc

HARFBUZZ_C_FILES := \
	hb-ucdn/ucdn.c

HARFBUZZ_O_FILES := $(HARFBUZZ_CC_FILES:%.cc=$(BUILD_FOLDER)/%.o) $(HARFBUZZ_C_FILES:%.c=$(BUILD_FOLDER)/%.o)

all: build

clean:
	rm -rf $(BUILD_FOLDER)
	rm -rf $(LIB_FILEPATH)

cleanlib:
	rm -rf $(LIB_FILEPATH)

build: create_folders $(LIB_FILEPATH)

create_folders:
	@mkdir -p $(BUILD_FOLDER)/hb-ucdn

$(LIB_FILEPATH): $(HARFBUZZ_O_FILES)
	ar rcs $@ $(BUILD_FOLDER)/*.o $(BUILD_FOLDER)/hb-ucdn/*.o

$(BUILD_FOLDER)/%.o: $(HARFBUZZ_FOLDER)/%.cc
	clang $(CCFLAGS) -c $< -o $@

$(BUILD_FOLDER)/%.o: $(HARFBUZZ_FOLDER)/%.c
	clang $(CCFLAGS) -c $< -o $@


