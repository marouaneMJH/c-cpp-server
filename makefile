# File: Makefile
CC = gcc
CFLAGS = -Wall -Wextra
TARGET = build/server
SRC = $(wildcard src/main.c  src/*.c src/**/*.c)
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ) | build
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
	
.PHONY: all run clean
