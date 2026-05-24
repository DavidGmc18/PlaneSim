export BUILD_DIR := $(abspath build)
export SRC_DIR := $(abspath src)

export CXX := g++
export CXXFLAGS := -std=c++20 -g -O0 -march=native

export INCLUDES := -I$(SRC_DIR)
export LDFLAGS := -lGL -lSDL2 -lassimp

BINARY := $(BUILD_DIR)/sim.bin