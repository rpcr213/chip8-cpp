CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LIBS := $(shell sdl2-config --libs)

TARGET := chip8
SRC := main.cpp Chip8.cpp CPU.cpp

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SDL_CFLAGS) $(SRC) -o $(TARGET) $(SDL_LIBS)

clean:
	rm -f $(TARGET) $(TARGET).exe
