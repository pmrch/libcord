ARCH ?= native
CC = clang

DEBUG_FLAGS = -g -O0 -fsanitize=address,leak,undefined -fno-omit-frame-pointer
STRICT_FLAGS ?= -Weverything -Werror -Wno-unsafe-buffer-usage -Wno-declaration-after-statement -Wno-c++-keyword -Wno-padded
RFLAGS = -Rpass=regalloc -Rpass-missed=regalloc -Rpass-analysis=regalloc
OPT = -O3 -march=$(ARCH)

#CFLAGS = -std=c23 $(DEBUG_FLAGS) -D_POSIX_C_SOURCE=200809L -DLOG_LEVEL=2 -xc $(STRICT_FLAGS)
CFLAGS = -std=c99 $(OPT) -D_POSIX_C_SOURCE=200809L -DLOG_LEVEL=2 -xc $(STRICT_FLAGS)

#LDFLAGS = -fsanitize=address,leak,undefined
LDFLAGS = -flto -static
INCLUDE = -Iinclude -Isrc/internal

SRC := $(shell find src -name "*.c")

OBJ := $(patsubst src/%.c,build/%.o,$(SRC))
OBJ_ASM := $(patsubst src/%.c,build/asm/%.s,$(SRC))

OBJ_DYN := $(patsubst src/%.c,build_dyn/%.o,$(SRC))
OBJ_DYN_ASM := $(patsubst src/%.c,build_dyn/asm/%.s,$(SRC))

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

build/asm/%.s: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE) $< -S -masm=intel -o $@

build_dyn/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDE) -fPIC -c $< -o $@

asm: $(OBJ_ASM)

lib_static: $(OBJ)
	@mkdir -p lib
	ar rcs lib/libcord.a $(OBJ)

lib_dyn: $(OBJ_DYN)
	@mkdir -p lib
	$(CC) $(LDFLAGS) -shared -o lib/libcord.so $(OBJ_DYN)

static:
	@bear -- $(MAKE) lib_static

dyn: clean
	@bear -- $(MAKE) lib_dyn

bin/cdc: tests/main.c
	@mkdir -p bin
	$(CC) $(CFLAGS) tests/main.c $(INCLUDE) -Llib -lcord -o bin/cdiscord  $(LDFLAGS)

bin:
	$(MAKE) clean
	$(MAKE) static
	@bear -- $(MAKE) bin/cdc
	@strip bin/cdiscord

test:
	$(MAKE) static
	$(MAKE) -C tests test

tester:
	@rustc -C opt-level=3 -C lto=thin -C target-cpu=native -C codegen-units=1 utils/tester/src/main.rs -o bin/tester

get_donna:
	@mkdir -p deps/donna
	@git clone --depth 1 https://github.com/agl/curve25519-donna.git temp_donna
	@cp temp_donna/curve25519-donna-c64.c deps/donna/curve25519_donna.c 2>/dev/null || true
	@rm -rf temp_donna

clean_donna:
	@rm -rf deps/donna/*.c

clean:
	@rm -rf build/*
	@rm -rf build_dyn
	@rm -rf lib/*
	@rm -rf bin/*
	@rm -rf compile_commands.json
	@rm -rf deps/donna/*.o
	$(MAKE) -C tests clean

.PHONY: bin bin/cdc clean asm
