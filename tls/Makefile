CC = clang

STRICT_FLAGS = -Weverything -Wno-unsafe-buffer-usage Wno-declaration-after-statement -Wno-padded
CFLAGS = -std=c99 -O3 -march=native -ffast-math $(STRICT_FLAGS)

SRC := $(shell find src -name "*.c")
OBJ := $(patsubst src/%.c,build/%.c,$(SRC))

build/%.o: src/%.c
	$(CC) $(CFLAGS) -Iinclude -MMD -MP -c $< -o $@
