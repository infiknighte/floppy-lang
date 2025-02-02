# Project Settings
CC := clang
CFLAGS := -Iinc -Wall -Wextra -std=c23 -g -O0 -DDEBUG -fsanitize=address,undefined -fno-omit-frame-pointer
SRC_DIR := src
BIN_DIR := bin
INC_DIR := inc
TARGET := $(BIN_DIR)/floppy

# Find all source files
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(SRC_FILES))

# Rules
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR)