# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I./src
LDFLAGS = 
LIBS = -lm -lreadline

# Project structure
SRC_DIR = src
BUILD_DIR = build
TARGET = lispy

# Source files (all in src/ or subdirectories)
SRCS = $(wildcard $(SRC_DIR)/*.c) \
       $(wildcard $(SRC_DIR)/mpc/*.c) \
       $(wildcard $(SRC_DIR)/sexpr/*.c)

# Object files
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Default target
all: $(BUILD_DIR) $(TARGET)

# Create build directory structure
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)/mpc
	mkdir -p $(BUILD_DIR)/sexpr

# Link object files
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

# Compile C files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Debug build
debug: CFLAGS += -g -O0
debug: $(BUILD_DIR) $(TARGET)

# Release build
release: CFLAGS += -O2 -DNDEBUG
release: $(BUILD_DIR) $(TARGET)

.PHONY: all clean run debug release
