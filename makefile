# -------- Toolchain defaults (native) --------
CC       ?= g++
AR       ?= ar
ARFLAGS  ?= rcs
OSTYPE   := $(shell uname)

# -------- Project sources --------
SRC = AbstractParticleSystem.cpp Image.cpp bmp.cpp png.cpp GLApp.cpp GLBuffer.cpp \
GLEnv.cpp GLProgram.cpp GLArray.cpp GLTexture1D.cpp GLTexture2D.cpp \
GLTexture3D.cpp GLTextureCube.cpp GLDebug.cpp Grid2D.cpp FontRenderer.cpp \
Rand.cpp GLFramebuffer.cpp GLDepthBuffer.cpp CommandInterpreter.cpp \
ImageLoader.cpp OBJFile.cpp \
Tesselation.cpp ArcBall.cpp Compression.cpp Base64Url.cpp

# -------- Native flags --------
ifeq ($(OSTYPE),Linux)
	CFLAGS_NATIVE   = -c -Wall -std=c++20 -Wunreachable-code -fopenmp
	INCLUDES_NATIVE = -I. -I../Utils
else
	HOMEBREW_PREFIX ?= $(shell brew --prefix 2>/dev/null || echo /opt/homebrew)
	CFLAGS_NATIVE   = -c -Wall -std=c++20 -Wunreachable-code -Xclang -fopenmp
	INCLUDES_NATIVE = -I. -I../Utils -I ../../openmp/include -I $(HOMEBREW_PREFIX)/include
endif

# -------- Emscripten toolchain + flags --------
EMCC     ?= em++
EMAR     ?= emar
EMSDK_ENV ?= $(HOME)/emsdk/emsdk_env.sh
EMSDK_ACTIVATE = if command -v $(EMCC) >/dev/null 2>&1; then :; elif [ -f "$(EMSDK_ENV)" ]; then EMSDK_QUIET=1; export EMSDK_QUIET; . "$(EMSDK_ENV)" >/dev/null 2>&1; else echo "Emscripten is not active and $(EMSDK_ENV) was not found."; exit 1; fi

# Keep it minimal here; main project should supply any WebGL/GLFW settings at link time.
CFLAGS_EM        = -c -Wall -std=c++20 -Wunreachable-code -D__EMSCRIPTEN__=1
INCLUDES_EM      = -I. -I../Utils

# -------- Output names / build dirs --------
TARGET_NATIVE = libutils.a
TARGET_EM     = libutils_emscripten.a

OBJDIR_NATIVE = build/native
OBJDIR_EM     = build/emscripten

OBJ_NATIVE = $(patsubst %.cpp,$(OBJDIR_NATIVE)/%.o,$(SRC))
OBJ_EM     = $(patsubst %.cpp,$(OBJDIR_EM)/%.o,$(SRC))

# -------- Default targets --------
all: $(TARGET_NATIVE)

release: CFLAGS_NATIVE += -O3 -DNDEBUG
release: $(TARGET_NATIVE)

emscripten: $(TARGET_EM)

emscripten_release: CFLAGS_EM += -O3 -DNDEBUG
emscripten_release: $(TARGET_EM)

# -------- Archive rules --------
$(TARGET_NATIVE): $(OBJ_NATIVE)
	$(AR) $(ARFLAGS) $@ $^

$(TARGET_EM): $(OBJ_EM)
	@echo "EMAR $@"
	@$(EMSDK_ACTIVATE); $(EMAR) $(ARFLAGS) $@ $^

# -------- Compile rules (separate object dirs) --------
$(OBJDIR_NATIVE)/%.o: %.cpp
	@mkdir -p $(OBJDIR_NATIVE)
	$(CC) $(CFLAGS_NATIVE) $(INCLUDES_NATIVE) $< -o $@

$(OBJDIR_EM)/%.o: %.cpp
	@mkdir -p $(OBJDIR_EM)
	@echo "EMCC $<"
	@$(EMSDK_ACTIVATE); $(EMCC) $(CFLAGS_EM) $(INCLUDES_EM) $< -o $@

# -------- Housekeeping --------
clean:
	-rm -rf $(OBJDIR_NATIVE) $(OBJDIR_EM) $(TARGET_NATIVE) $(TARGET_EM) docs core

docs:
	doxygen Doxyfile

.PHONY: all release emscripten emscripten_release clean docs
