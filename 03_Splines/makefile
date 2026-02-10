CC=g++
OSTYPE := $(shell uname)

# Native build output directory (non-Emscripten builds)
OUTDIR := build
OBJDIR := $(OUTDIR)/obj

ifeq ($(OSTYPE),Linux)
	CFLAGS=-c -Wall -std=c++20 -Wunreachable-code
	LFLAGS=-lglfw -lGLEW -lGL -lstdc++fs
	LIBS=
	INCLUDES=-I. -I../Utils
else
	CFLAGS=-c -Wall -std=c++20 -Wunreachable-code -Xclang
	LFLAGS=-lglfw -lGLEW -framework OpenGL
	LIBS=-L /opt/homebrew/lib
	INCLUDES=-I. -I../Utils -I /opt/homebrew/include
endif

# Project sources
SRC = main.cpp
OBJ = $(addprefix $(OBJDIR)/,$(SRC:.cpp=.o))

TARGET = splines
TARGET_PATH = $(OUTDIR)/$(TARGET)

# Assets
ASSET_DIRS := Shader Datasets
ASSET_FILES := $(shell find $(ASSET_DIRS) -type f 2>/dev/null)
ASSETS_STAMP := $(OUTDIR)/.assets_copied

# Emscripten/Web build output directory
WEBOUTDIR := web
WEBHTML := $(WEBOUTDIR)/Solution.js

# For Emscripten, preload every file in Shader/ and Datasets/ but map them into
# the virtual filesystem root so they appear next to the executable.
EM_PRELOAD_ARGS := $(foreach f,$(ASSET_FILES),--preload-file $(f)@$(notdir $(f)))

# Utils libraries
UTILS_DIR    := ../Utils
UTILS_LIB    := $(UTILS_DIR)/libutils.a
UTILS_EM_LIB := $(UTILS_DIR)/libutils_emscripten.a

# Emscripten tools
EMCC ?= em++

# Common Emscripten link flags
EM_EXPORTED_FUNCTIONS := ''
EM_RUNTIME_METHODS    := ''

EM_LINK_FLAGS := \
	$(EM_PRELOAD_ARGS) \
	-s EXPORTED_FUNCTIONS=$(EM_EXPORTED_FUNCTIONS) \
	-s EXPORTED_RUNTIME_METHODS=$(EM_RUNTIME_METHODS) \
	-s ALLOW_MEMORY_GROWTH=1 \
	-s USE_WEBGL2=1 -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 \
	-s USE_GLFW=3 -s FULL_ES3=1

EM_INCLUDES := -I. -I../Utils -D__EMSCRIPTEN__=1

.PHONY: all release clean mrproper emscripten emscripten_release \
        utils_emscripten utils_emscripten_release

all: $(TARGET_PATH)

release: CFLAGS += -O3 -DNDEBUG
release: $(TARGET_PATH)

# ---- Build utils (native) when needed ----
$(UTILS_LIB):
	cd $(UTILS_DIR) && make $(MAKECMDGOALS)

# ---- Native build dirs ----
$(OUTDIR):
	mkdir -p $@

$(OBJDIR): | $(OUTDIR)
	mkdir -p $@

# ---- Copy assets into OUTDIR (native build convenience) ----
$(ASSETS_STAMP): $(ASSET_FILES) | $(OUTDIR)
	@# Copy contents of Shader/ and Datasets/ directly into $(OUTDIR)/ (no subdirectories)
	@if [ -d Shader ]; then find Shader -maxdepth 1 -type f -print0 2>/dev/null | xargs -0 -I{} sh -c 'cp -f "$$1" "$(OUTDIR)/$$(basename "$$1")"' _ {}; fi
	@if [ -d Datasets ]; then find Datasets -maxdepth 1 -type f -print0 2>/dev/null | xargs -0 -I{} sh -c 'cp -f "$$1" "$(OUTDIR)/$$(basename "$$1")"' _ {}; fi
	@touch $@

# ---- Native link ----
$(TARGET_PATH): $(OBJ) $(UTILS_LIB) $(ASSETS_STAMP) | $(OUTDIR)
	@# Note: $(ASSETS_STAMP) is an empty stamp file; do not pass it to the linker.
	$(CC) $(INCLUDES) $(OBJ) $(UTILS_LIB) $(LFLAGS) $(LIBS) -o $@

# ---- Native compile ----
$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

# ---- Cleaning ----
clean:
	-rm -rf $(OUTDIR) $(WEBOUTDIR) $(TARGET) core

mrproper: clean
	cd $(UTILS_DIR) && make clean

# ---- Emscripten utils build triggers ----
utils_emscripten:
	$(MAKE) -C $(UTILS_DIR) emscripten

utils_emscripten_release:
	$(MAKE) -C $(UTILS_DIR) emscripten_release

# ---- Emscripten builds (link against libutils_emscripten.a) ----
emscripten: utils_emscripten
	@mkdir -p $(WEBOUTDIR)
	$(EMCC) $(SRC) $(UTILS_EM_LIB) \
		$(EM_INCLUDES) \
		$(EM_LINK_FLAGS) \
		-O2 \
		-o $(WEBHTML)

emscripten_release: utils_emscripten_release
	@mkdir -p $(WEBOUTDIR)
	$(EMCC) $(SRC) $(UTILS_EM_LIB) \
		$(EM_INCLUDES) \
		$(EM_LINK_FLAGS) \
		-O3 -DNDEBUG \
		-o $(WEBHTML)
