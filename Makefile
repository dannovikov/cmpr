CC := gcc

.PHONY: all clean debug dev

all: dist/cmpr

CFLAGS := -O2 -Wall
LDFLAGS := -lm

debug: CFLAGS := -g -O0 -Wall -fsanitize=address
debug: dist/cmpr

# TODO: we should put the different builds in different output directories; currently switching requires `make -B` as the .o files will be incompatible due to libasan

dev: CFLAGS := -g -O2 -Wall -Werror -fsanitize=address
dev: dist/cmpr

dist/cmpr: cmpr.c fdecls.h spanio.c siphash/siphash.o siphash/halfsiphash.o
	mkdir -p dist
	(VER=8; D=$$(date +%Y%m%d-%H%M%S); GIT=$$(git log -1 --pretty="%h %f"); echo '#line 1 "cmpr.c"' >cmpr-sed.c; sed 's/\$$VERSION\$$/'"$$VER"' (build: '"$$D"' '"$$GIT"')/' <cmpr.c >>cmpr-sed.c; echo "Version: $$VER (build: $$D $$GIT)"; $(CC) -o dist/cmpr-$$D cmpr-sed.c siphash/siphash.o siphash/halfsiphash.o $(CFLAGS) $(LDFLAGS) && rm -f dist/cmpr && ln -s cmpr-$$D dist/cmpr)

siphash/siphash.o: siphash/siphash.c
	$(CC) -c siphash/siphash.c $(CFLAGS) -o siphash/siphash.o

siphash/halfsiphash.o: siphash/halfsiphash.c
	$(CC) -c siphash/halfsiphash.c $(CFLAGS) -o siphash/halfsiphash.o

fdecls.h: cmpr.c
	python3 extract_decls.py < cmpr.c > fdecls.h

install: dist/cmpr
	install -m 755 dist/cmpr /usr/local/bin/cmpr
