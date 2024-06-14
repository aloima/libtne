CC := gcc
CFLAGS := -Wall -Wextra -O2 $(shell pkg-config --cflags --libs openssl)

compile: $(wildcard lib/*.c)
	$(CC) -shared -o libtne.so $(patsubst lib/%.c,build/%.o,$?)

install: compile
	cp libtne.so /usr/lib/libtne.so
	cp tne.h /usr/include
	make clean

lib/*.c: check
	$(CC) $(CFLAGS) -fPIC -c $@ -o build/$(patsubst %.c,%.o,$(@F))

uninstall:
	rm /usr/lib/libtne.so
	rm /usr/include/tne.h

clean:
	rm -r build
	rm libtne.so

check:
	mkdir -p build
