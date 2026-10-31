CC = clang

DONNA_CFLAGS = -std=c99 -O2 -w
CFLAGS = -std=c23 -O2 -g -Weverything -Wno-jump-misses-init -Wno-unsafe-buffer-usage \
         -Wno-declaration-after-statement -Wno-c++-keyword -Wno-padded \
         -I../include -I../src/internal -I../deps/donna

LDFLAGS = -L../lib -lcord
TESTS_SRC := $(wildcard test_*.c)

TESTS := $(patsubst %.c,%,$(TESTS_SRC))

all: $(TESTS)

../deps/donna/curve25519_donna.o: ../deps/donna/curve25519_donna.c
	$(CC) $(DONNA_CFLAGS) -c $< -o $@

test_fe: test_fe.c ../deps/donna/curve25519_donna.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%: %.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

test: all
	@fail=0; for t in $(TESTS); do \
		echo "=== $$t ==="; ./$$t || fail=1; \
	done; exit $$fail

clean:
	@rm -f $(TESTS)

.PHONY: all test clean