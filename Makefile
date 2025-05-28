# Ref.: https://makefiletutorial.com/#makefile-cookbook

BUILD_DIR := ./build
SRC_DIRS := ./src

CC = gcc
CXX = g++

SRCS = $(shell find -L $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.S')
OBJS := $(SRCS:%=./$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

# Every folder in ./src will need to be passed to GCC so that it can find header files
INC_DIRS := $(shell find $(SRC_DIRS) -type d) $(shell find $(SRC_DIRS) -type ld)
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

all: $(BUILD_DIR)/program

run: $(BUILD_DIR)/program
	$(BUILD_DIR)/program

clean:
	rm -rf $(BUILD_DIR)

$(BUILD_DIR)/%.S.o: %.S
	mkdir -p $(dir $@)
	${CC} -MMD -MP -c $< -o $@

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	${CC} -MMD -MP -g $(INC_FLAGS) $(shell sdl2-config --cflags) -c $< -o $@

$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	${CXX} -std=c++23 -MMD -MP -g $(INC_FLAGS) $(shell sdl2-config --cflags) -c $< -o $@

$(BUILD_DIR)/program: $(OBJS)
	mkdir -p $(dir $@)
	${CC} -g $(OBJS) -o $@ $(shell sdl2-config --libs) -lm -lstdc++

-include $(DEPS)

.PHONY: all clean run program
