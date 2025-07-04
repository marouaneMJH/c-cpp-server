# File: Makefile
CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic
LDFLAGS = -pthread -lm
TARGET = build/server
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# Find all source files in the src directory and its subdirectories
# Explicitly exclude the old socket_setup.c file
SRC = $(filter-out $(SRC_DIR)/socket_setup.c, $(shell find $(SRC_DIR) -name "*.c"))
# Generate object file names with proper directory structure in obj dir
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))
# Include directories for compilation
INC = -I$(INC_DIR)

.PHONY: all clean run test

all: $(TARGET)

# Create the target executable
$(TARGET): $(OBJ) | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@
	@echo "Build complete: $(TARGET)"

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

# Create the build directory structure
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)/server
	@mkdir -p $(OBJ_DIR)/http

# Run the server
run: $(TARGET)
	./$(TARGET)

# Clean up
clean:
	rm -rf $(BUILD_DIR)
	find $(SRC_DIR) -name "*.o" -type f -delete
