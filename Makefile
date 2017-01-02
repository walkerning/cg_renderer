UNAME := $(shell uname -s)
ifeq ($(UNAME), Darwin)
	CXX = clang++
else
	CXX = g++
endif

INCLUDE_DIRS = include
SRC_DIR = src
CXX_FLAGS = --std=c++11

all: build/main

build/%: $(SRC_DIR)/%.cpp
	@mkdir -p build
	$(CXX) $(CXX_FLAGS) $(foreach includedir,$(INCLUDE_DIRS),-I$(includedir)) -o $@ $^

clean:
	@rm -rf build
