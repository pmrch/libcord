CC = clang

STRICT_FLAGS = -Weverything -Wno-jump-misses-init -Wno-unsafe-buffer-usage -Wno-declaration-after-statement -Wno-c++-keyword -Wno-padded
CFLAGS = -std=c99 -O3 -march=native -ffast-math -D_POSIX_C_SOURCE=200809L -DLOG_LEVEL=1 -xc $(STRICT_FLAGS)

SRC := $(shell find src -name "*.c")
OBJ := $(patsubst src/%.c,build/%.o,$(SRC))
OBJ_DYN := $(patsubst src/%.c,build_dyn/%.o,$(SRC))

build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -Iinclude -MMD -MP -c $< -o $@

build_dyn/%.o: src/%.c
	@mkdir -p build_dyn
	$(CC) $(CFLAGS) -Iinclude -MMD -MP -fPIC -c $< -o $@

lib_static: $(OBJ)
	@mkdir -p lib
	ar rcs lib/libcord.a $(OBJ)

lib_dyn: $(OBJ_DYN)
	@mkdir -p lib
	$(CC) $(LDFLAGS) -shared -o lib/libcord.so $(OBJ_DYN)

static:
	bear -- make lib_static -j$(nproc)

dyn: clean
	bear -- make lib_dyn -j$(nproc)

bin/cdc: test/main.c
	@mkdir -p bin
	$(CC) $(CFLAGS) test/main.c -Iinclude -Llib -lcord -o bin/cdiscord -flto -static

bin:
	make clean
	make static
	bear -- make bin/cdc -j$(nproc)

clean:
	@rm -rf build/*
	@rm -rf lib/*
	@rm -rf bin/*
	@rm -rf compile_commands.json

.PHONY: bin bin/cdc clean