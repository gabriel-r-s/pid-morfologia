CXX := g++
CXXFLAGS := -O3 -Wall -Werror

exec := -o build/main
libs := -lSDL2
src := src/main.cpp
objects := $(shell echo build/*.o)
includes := -Isrc/imgui

.PHONY: all
all: build/imgui.o
	$(CXX) $(CXXFLAGS) $(exec) $(src) $(objects) $(libs)  $(includes)

imgui_cpp := $(wildcard src/imgui/*.cpp src/imgui/backends/imgui_impl_sdl2.cpp src/imgui/backends/imgui_impl_sdlrenderer2.cpp)
imgui_include := -Isrc/imgui -Isrc/imgui/backends -I/usr/include/SDL2
build/imgui.o:
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(imgui_include) -c $(imgui_cpp)
	mv imgui*.o build

.PHONY: clean
clean:
	rm ./build/*
 
