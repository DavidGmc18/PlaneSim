export SOURCE_DIR := $(abspath .)
export BUILD_DIR := $(abspath build)
export SRC_DIR := $(abspath src)
export EXTERNAL_DIR := $(abspath external)

export CXX := g++
export CXXFLAGS_SRC := -std=c++20 -g -O3 -march=native -Wall -Wextra
export CXXFLAGS_EXTERNAL := -std=c++20 -O3 -march=native

export INCLUDES := -I. -I$(SRC_DIR) -isystem $(EXTERNAL_DIR)
export LDFLAGS := -lGL -lSDL2 -lassimp

export BINARY := $(BUILD_DIR)/sim.bin

BUILD_ON_RAM = 1
BUILD_ON_RAM_SIZE = 32M