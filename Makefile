UNAME := $(shell uname -s)
ifeq ($(UNAME), Darwin)
	CXX = clang++
else
	CXX = g++
endif

# flags
CXX_FLAGS = --std=c++11
INCLUDE_DIRS = include

# main
SRC_DIR = src
SRCS := $(shell find $(SRC_DIR) -name "*.cpp")
BUILD_DIR = build

# for test
TEST_DIR = tests
TEST_SRCS := $(shell find $(TEST_DIR) -name "*.cpp" | xargs -I{} basename {})
SRCS_NOMAIN := $(shell find $(SRC_DIR) ! -name "main.cpp" -name "*.cpp")
TEST_BUILD_DIR = $(BUILD_DIR)/tests
TEST_BIN := $(addprefix $(TEST_BUILD_DIR)/,${TEST_SRCS:.cpp=.bin})

# all build dirs
ALL_BUILD_DIR := $(BUILD_DIR) $(TEST_BUILD_DIR)

all: $(BUILD_DIR)/main test

test: $(TEST_BIN) | $(ALL_BUILD_DIR)

$(ALL_BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(TEST_BUILD_DIR)

$(TEST_BUILD_DIR)/%.bin: $(TEST_DIR)/%.cpp $(SRCS_NOMAIN)
	$(CXX) $(CXX_FLAGS) $(foreach includedir,$(INCLUDE_DIRS),-I$(includedir)) -o $@ $^

runtest: test
	@for f in `find $(TEST_BUILD_DIR) -name "*.bin"`	; do\
		./$$f; \
	done

build/main: $(SRCS) | $(ALL_BUILD_DIR)
	$(CXX) $(CXX_FLAGS) $(foreach includedir,$(INCLUDE_DIRS),-I$(includedir)) -o $@ $^

clean:
	@rm -rf $(BUILD_DIR)
