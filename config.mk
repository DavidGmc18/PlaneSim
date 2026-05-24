export BUILD_DIR := $(abspath build)
export SRC_DIR := $(abspath src)

export CXX := g++
export CXXFLAGS := -std=c++20 -g -O3 -march=native -Wall -Wextra

export INCLUDES := -I$(SRC_DIR)
export LDFLAGS := -lGL -lSDL2 -lassimp

export BINARY := $(BUILD_DIR)/sim.bin

BUILD_ON_RAM = 1
BUILD_ON_RAM_SIZE = 32M