CC = clang

STRICT_FLAGS = -Weverything -Wno-jump-misses-init -Wno-unsafe-buffer-usage -Wno-declaration-after-statement -Wno-c++-keyword -Wno-padded
OPT = -O3 -march=native -mprefer-vector-width=512
CFLAGS = -std=c23 $(OPT) -D_POSIX_C_SOURCE=200809L -DLOG_LEVEL=2 -xc $(STRICT_FLAGS)
LDFLAGS = -flto -static

SRC := $(shell find src -name "*.c")

OBJ := $(patsubst src/%.c,build/%.o,$(SRC))
OBJ_ASM := $(patsubst src/%.c,build/asm/%.s,$(SRC))

OBJ_DYN := $(patsubst src/%.c,build_dyn/%.o,$(SRC))
OBJ_DYN_ASM := $(patsubst src/%.c,build_dyn/asm/%.s,$(SRC))

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Iinclude -MMD -MP -c $< -o $@

build/asm/%.s: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Iinclude -MMD -MP -c $< -S -masm=intel -o $@

build_dyn/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -Iinclude -MMD -MP -fPIC -c $< -o $@

lib_static: $(OBJ) $(OBJ_ASM)
	@mkdir -p lib
	ar rcs lib/libcord.a $(OBJ)

lib_dyn: $(OBJ_DYN)
	@mkdir -p lib
	$(CC) $(LDFLAGS) -shared -o lib/libcord.so $(OBJ_DYN)

static:
	bear -- make lib_static

dyn: clean
	bear -- make lib_dyn

bin/cdc: test/main.c
	@mkdir -p bin
	$(CC) $(CFLAGS) test/main.c -Iinclude -Llib -lcord -o bin/cdiscord -flto -static

bin:
	make clean
	make static
	bear -- make bin/cdc
#	@strip bin/cdiscord

tester:
	rustc -C opt-level=3 -C lto=thin -C target-cpu=native -C codegen-units=1 utils/tester/src/main.rs -o bin/tester

clean:
	@rm -rf build/*
	@rm -rf build_dyn
	@rm -rf lib/*
	@rm -rf bin/*
	@rm -rf compile_commands.json

.PHONY: bin bin/cdc clean