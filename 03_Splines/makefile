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

SRC = main.cpp
OBJ = $(addprefix $(OBJDIR)/,$(SRC:.cpp=.o))
TARGET = splmes

TARGET_PATH = $(OUTDIR)/$(TARGET)

ASSET_DIRS := Shader Datasets
ASSET_FILES := $(shell find $(ASSET_DIRS) -type f 2>/dev/null)
ASSETS_STAMP := $(OUTDIR)/.assets_copied

# Emscripten/Web build output directory
WEBOUTDIR := web
WEBHTML := $(WEBOUTDIR)/solution.js

# For Emscripten, preload every file in Shader/ and Datasets/ but map them into
# the virtual filesystem root so they appear next to the executable.
EM_PRELOAD_ARGS := $(foreach f,$(ASSET_FILES),--preload-file $(f)@$(notdir $(f)))

all: $(TARGET_PATH)

release: CFLAGS += -O3 -DNDEBUG
release: $(TARGET_PATH)

../Utils/libutils.a:
	cd ../Utils && make $(MAKECMDGOALS)

$(OUTDIR):
	mkdir -p $@

$(OBJDIR): | $(OUTDIR)
	mkdir -p $@

$(ASSETS_STAMP): $(ASSET_FILES) | $(OUTDIR)
	@# Copy contents of Shader/ and Datasets/ directly into $(OUTDIR)/ (no subdirectories)
	@if [ -d Shader ]; then find Shader -maxdepth 1 -type f -print0 2>/dev/null | xargs -0 -I{} sh -c 'cp -f "$$1" "$(OUTDIR)/$$(basename "$$1")"' _ {}; fi
	@if [ -d Datasets ]; then find Datasets -maxdepth 1 -type f -print0 2>/dev/null | xargs -0 -I{} sh -c 'cp -f "$$1" "$(OUTDIR)/$$(basename "$$1")"' _ {}; fi
	@touch $@

$(TARGET_PATH): $(OBJ) ../Utils/libutils.a $(ASSETS_STAMP) | $(OUTDIR)
	@# Note: $(ASSETS_STAMP) is an empty stamp file; do not pass it to the linker.
	$(CC) $(INCLUDES) $(OBJ) ../Utils/libutils.a $(LFLAGS) $(LIBS) -o $@

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	-rm -rf $(OUTDIR) $(WEBOUTDIR) $(TARGET) core

mrproper: clean
	cd ../Utils && make clean

emscripten:
	@mkdir -p $(WEBOUTDIR)
	em++ $(SRC) ../Utils/GLEnv.cpp ../Utils/GLApp.cpp ../Utils/GLDebug.cpp ../Utils/Image.cpp ../Utils/GLProgram.cpp ../Utils/GLArray.cpp ../Utils/GLBuffer.cpp ../Utils/GLTexture2D.cpp ../Utils/CommandInterpreter.cpp \
		$(EM_PRELOAD_ARGS) \
		-o $(WEBHTML) \
		-s ALLOW_MEMORY_GROWTH=1 -D__EMSCRIPTEN__=1 -s USE_WEBGL2=1 -s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -s USE_GLFW=3 -s FULL_ES3=1 -O2 -I. -I../Utils -O3 -DNDEBUG
