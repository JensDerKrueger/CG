# -------- Toolchain defaults (native) --------
CC       ?= g++
AR       ?= ar
ARFLAGS  ?= rcs
OSTYPE   := $(shell uname)

# -------- Project sources --------
SRC = AbstractParticleSystem.cpp Image.cpp bmp.cpp OBJFile.cpp GLApp.cpp GLBuffer.cpp \
GLEnv.cpp GLProgram.cpp GLArray.cpp GLTexture2D.cpp GLTexture1D.cpp GLTexture3D.cpp \
GLDebug.cpp Grid2D.cpp FontRenderer.cpp Rand.cpp ImageLoader.cpp GLFramebuffer.cpp \
GLDepthBuffer.cpp GLTextureCube.cpp CommandInterpreter.cpp Tesselation.cpp ArcBall.cpp

# -------- Native flags --------
ifeq ($(OSTYPE),Linux)
	CFLAGS_NATIVE   = -c -Wall -std=c++20 -Wunreachable-code -fopenmp
	INCLUDES_NATIVE = -I. -I../Utils
else
	CFLAGS_NATIVE   = -c -Wall -std=c++20 -Wunreachable-code -Xclang -fopenmp
	INCLUDES_NATIVE = -I. -I../Utils -I ../../openmp/include -I /opt/homebrew/include
endif

# -------- Emscripten toolchain + flags --------
EMCC     ?= em++
EMAR     ?= emar

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
	$(EMAR) $(ARFLAGS) $@ $^

# -------- Compile rules (separate object dirs) --------
$(OBJDIR_NATIVE)/%.o: %.cpp
	@mkdir -p $(OBJDIR_NATIVE)
	$(CC) $(CFLAGS_NATIVE) $(INCLUDES_NATIVE) $< -o $@

$(OBJDIR_EM)/%.o: %.cpp
	@mkdir -p $(OBJDIR_EM)
	$(EMCC) $(CFLAGS_EM) $(INCLUDES_EM) $< -o $@

# -------- Housekeeping --------
clean:
	-rm -rf $(OBJDIR_NATIVE) $(OBJDIR_EM) $(TARGET_NATIVE) $(TARGET_EM) docs core

docs:
	doxygen Doxyfile

.PHONY: all release emscripten emscripten_release clean docs
