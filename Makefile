CC := gcc
CFLAGS := -Wall -Wextra -O2

compile: $(wildcard lib/*.c)
	ar rcs libtne.a $(patsubst lib/%.c,build/%.o,$?)

install: compile
	cp libtne.a /usr/lib/libtne.a
	cp tne.h /usr/include
	make clean

lib/*.c: check
	$(CC) $(CFLAGS) -c $@ -o build/$(patsubst %.c,%.o,$(@F))

uninstall:
	rm /usr/lib/libtne.a
	rm /usr/include/tne.h

clean:
	rm -r build
	rm libtne.a

check:
	mkdir -p build
