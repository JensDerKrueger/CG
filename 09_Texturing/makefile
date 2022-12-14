CC=g++
OSTYPE := $(shell uname)

ifeq ($(OSTYPE),Linux)
	CFLAGS=-c -Wall -std=c++17 -Wunreachable-code 
	LFLAGS=-lglfw -lGLEW -lGL -L../Utils -lutils
	LIBS=
	INCLUDES=-I. -I../Utils
else
	CFLAGS=-c -Wall -std=c++17 -Wunreachable-code -Xclang
	LFLAGS=-lglfw -lGLEW -framework OpenGL -L../Utils -lutils
	LIBS=-L /opt/homebrew/lib
	INCLUDES=-I. -I../Utils -I /opt/homebrew/include
endif

SRC = Camera.cpp IntersectableObject.cpp Intersection.cpp LightSource.cpp main.cpp Material.cpp Plane.cpp PointLight.cpp Ray.cpp Raytracer.cpp Scene.cpp Sphere.cpp Texture.cpp stb_image.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = texturing

all: $(TARGET)

release: CFLAGS += -O3 -DNDEBUG
release: $(TARGET)

../Utils/libutils.a:
	cd ../Utils && make $(MAKECMDGOALS)
	
$(TARGET): $(OBJ) ../Utils/libutils.a
	$(CC) $(INCLUDES) $^ $(LFLAGS) $(LIBS) -o $@
	
%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@
	
clean:
	-rm -rf $(OBJ) $(TARGET) core
	
mrproper: clean
	cd ../Utils && make clean
