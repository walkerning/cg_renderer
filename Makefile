UNAME := $(shell uname -s)
ifeq ($(UNAME), Darwin)
	CXX = clang++
else
	CXX = g++
endif

INCLUDE_DIRS = include
SRC_DIR = src
BUILD_DIR = build
CXX_FLAGS = --std=c++11

SRCS := $(shell find $(SRC_DIR) -name "*.cpp")

all: $(BUILD_DIR)/main

$(BUILD_DIR):
	@mkdir -p build

build/main: $(SRCS) | $(BUILD_DIR)
	$(CXX) $(CXX_FLAGS) $(foreach includedir,$(INCLUDE_DIRS),-I$(includedir)) -o $@ $^

clean:
	@rm -rf $(BUILD_DIR)
