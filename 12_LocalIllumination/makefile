CC=g++
OSTYPE := $(shell uname)

OUTDIR := build
OBJDIR := $(OUTDIR)/obj

ifeq ($(OSTYPE),Linux)
	CFLAGS=-c -Wall -std=c++20 -Wunreachable-code
	LFLAGS=-lglfw -lGLEW -lGL -lstdc++fs
	LIBS=
	INCLUDES=-I. -I../Utils
else
	HOMEBREW_PREFIX ?= $(shell brew --prefix 2>/dev/null || echo /opt/homebrew)
	CFLAGS=-c -Wall -std=c++20 -Wunreachable-code -Xclang
	LFLAGS=-lglfw -lGLEW -framework OpenGL
	LIBS=-L $(HOMEBREW_PREFIX)/lib
	INCLUDES=-I. -I../Utils -I $(HOMEBREW_PREFIX)/include
endif

SRC = main.cpp
SOLUTION_SRC = html/solution/main.cpp
OBJ = $(addprefix $(OBJDIR)/,$(SRC:.cpp=.o))

TARGET = local_illumination
TARGET_PATH = $(OUTDIR)/$(TARGET)

ASSET_DIRS := Shader Datasets
ASSET_FILES := $(shell find $(ASSET_DIRS) -type f 2>/dev/null)
ASSETS_STAMP := $(OUTDIR)/.assets_copied

WEBOUTDIR := web
WEBHTML := $(WEBOUTDIR)/Solution.js

EM_PRELOAD_ARGS := $(foreach f,$(ASSET_FILES),--preload-file $(f)@$(notdir $(f)))

UTILS_DIR    := ../Utils
UTILS_LIB    := $(UTILS_DIR)/libutils.a
UTILS_EM_LIB := $(UTILS_DIR)/libutils_emscripten.a

EMCC ?= em++
EM_CXXFLAGS := -std=c++20
EMSDK_ENV ?= $(HOME)/emsdk/emsdk_env.sh
EMSDK_ACTIVATE = if command -v $(EMCC) >/dev/null 2>&1; then :; elif [ -f "$(EMSDK_ENV)" ]; then EMSDK_QUIET=1; export EMSDK_QUIET; . "$(EMSDK_ENV)" >/dev/null 2>&1; else echo "Emscripten is not active and $(EMSDK_ENV) was not found."; exit 1; fi

EM_EXPORTED_FUNCTIONS := '["_main"]'
EM_RUNTIME_METHODS    := '["ccall"]'

EM_LINK_FLAGS := \
	$(EM_PRELOAD_ARGS) \
	-s EXPORTED_FUNCTIONS=$(EM_EXPORTED_FUNCTIONS) \
	-s EXPORTED_RUNTIME_METHODS=$(EM_RUNTIME_METHODS) \
	-s ALLOW_MEMORY_GROWTH=1 \
	-s USE_WEBGL2=1 -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 \
	-s USE_GLFW=3 -s FULL_ES3=1

EM_SOLUTION_PRELOAD_ARGS := \
	--preload-file Shader/vertexShader.vert@vertexShader.vert \
	--preload-file html/solution/fragmentShader.frag@fragmentShader.frag

EM_SOLUTION_LINK_FLAGS := \
	$(EM_SOLUTION_PRELOAD_ARGS) \
	-s EXPORTED_FUNCTIONS=$(EM_EXPORTED_FUNCTIONS) \
	-s EXPORTED_RUNTIME_METHODS=$(EM_RUNTIME_METHODS) \
	-s ALLOW_MEMORY_GROWTH=1 \
	-s USE_WEBGL2=1 -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 \
	-s USE_GLFW=3 -s FULL_ES3=1

EM_INCLUDES := -I. -I../Utils -D__EMSCRIPTEN__=1

.PHONY: all release clean mrproper emscripten emscripten_release \
        emscripten_solution_release utils_emscripten utils_emscripten_release

all: $(TARGET_PATH)

release: CFLAGS += -O3 -DNDEBUG
release: $(TARGET_PATH)

$(UTILS_LIB):
	cd $(UTILS_DIR) && make $(MAKECMDGOALS)

$(OUTDIR):
	mkdir -p $@

$(OBJDIR): | $(OUTDIR)
	mkdir -p $@

$(ASSETS_STAMP): $(ASSET_FILES) | $(OUTDIR)
	@if [ -d Shader ]; then find Shader -maxdepth 1 -type f -print0 2>/dev/null | xargs -0 -I{} sh -c 'cp -f "$$1" "$(OUTDIR)/$$(basename "$$1")"' _ {}; fi
	@if [ -d Datasets ]; then find Datasets -maxdepth 1 -type f -print0 2>/dev/null | xargs -0 -I{} sh -c 'cp -f "$$1" "$(OUTDIR)/$$(basename "$$1")"' _ {}; fi
	@touch $@

$(TARGET_PATH): $(OBJ) $(UTILS_LIB) $(ASSETS_STAMP) | $(OUTDIR)
	$(CC) $(INCLUDES) $(OBJ) $(UTILS_LIB) $(LFLAGS) $(LIBS) -o $@

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	-rm -rf $(OUTDIR) $(WEBOUTDIR) $(TARGET) core

mrproper: clean
	cd $(UTILS_DIR) && make clean

utils_emscripten:
	@$(EMSDK_ACTIVATE); $(MAKE) -C $(UTILS_DIR) emscripten

utils_emscripten_release:
	@$(EMSDK_ACTIVATE); $(MAKE) -C $(UTILS_DIR) emscripten_release

emscripten: utils_emscripten
	@mkdir -p $(WEBOUTDIR)
	@echo "EMCC $(WEBHTML)"
	@$(EMSDK_ACTIVATE); \
	$(EMCC) $(SRC) $(UTILS_EM_LIB) \
		$(EM_INCLUDES) \
		$(EM_LINK_FLAGS) \
		$(EM_CXXFLAGS) \
		-O2 \
		-o $(WEBHTML)

emscripten_release: utils_emscripten_release
	@mkdir -p $(WEBOUTDIR)
	@echo "EMCC $(WEBHTML)"
	@$(EMSDK_ACTIVATE); \
	$(EMCC) $(SRC) $(UTILS_EM_LIB) \
		$(EM_INCLUDES) \
		$(EM_LINK_FLAGS) \
		$(EM_CXXFLAGS) \
		-O3 -DNDEBUG \
		-o $(WEBHTML)

emscripten_solution_release: utils_emscripten_release
	@mkdir -p $(WEBOUTDIR)
	@echo "EMCC $(WEBHTML) with solution shader"
	@$(EMSDK_ACTIVATE); \
	$(EMCC) $(SOLUTION_SRC) $(UTILS_EM_LIB) \
		$(EM_INCLUDES) \
		$(EM_SOLUTION_LINK_FLAGS) \
		$(EM_CXXFLAGS) \
		-O3 -DNDEBUG \
		-o $(WEBHTML)
