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

docs: $(patsubst %,%.gz,$(wildcard doc/man3/*))
docs-clean: $(patsubst %,%.gz-clean,$(wildcard doc/man3/*))

$(patsubst %,%.gz-clean,$(wildcard doc/man3/*)):
	rm -rf /usr/share/man/man3/$(patsubst %.gz-clean,%.gz,$(@F))

$(patsubst %,%.gz,$(wildcard doc/man3/*)):
	gzip -c $(patsubst %.gz,%,$@) > $@
	mv $@ /usr/share/man/man3/$(@F)

uninstall:
	rm /usr/lib/libtne.so
	rm /usr/include/tne.h

clean:
	rm -r build
	rm libtne.so

check:
	mkdir -p build
