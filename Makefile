CC := gcc

.PHONY: all clean debug dev

all: dist/cmpr

CFLAGS := -O2 -Wall -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION
LDFLAGS := -lm

debug: CFLAGS := -g -O0 -Wall -fsanitize=address -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION
debug: dist/cmpr

dev: CFLAGS := -O2 -Wall -Werror -fsanitize=address -DSQLITE_THREADSAFE=0 -DSQLITE_OMIT_LOAD_EXTENSION
dev: dist/cmpr

dist/cmpr: cmpr.c fdecls.h spanio.c siphash/siphash.o siphash/halfsiphash.o libs/sqlite3.o
	mkdir -p dist
	(VER=8; D=$$(date +%Y%m%d-%H%M%S); GIT=$$(git log -1 --pretty="%h %f"); echo '#line 1 "cmpr.c"' >cmpr-sed.c; sed 's/\$$VERSION\$$/'"$$VER"' (build: '"$$D"' '"$$GIT"')/' <cmpr.c >>cmpr-sed.c; echo "Version: $$VER (build: $$D $$GIT)"; $(CC) -o dist/cmpr-$$D cmpr-sed.c siphash/siphash.o siphash/halfsiphash.o libs/sqlite3.o $(CFLAGS) $(LDFLAGS) && rm -f dist/cmpr && ln -s cmpr-$$D dist/cmpr)

siphash/siphash.o: siphash/siphash.c
	$(CC) -c siphash/siphash.c $(CFLAGS) -o siphash/siphash.o

siphash/halfsiphash.o: siphash/halfsiphash.c
	$(CC) -c siphash/halfsiphash.c $(CFLAGS) -o siphash/halfsiphash.o

libs/sqlite3.o: libs/sqlite3.c
	$(CC) -c libs/sqlite3.c $(CFLAGS) -o libs/sqlite3.o

fdecls.h: cmpr.c
	python3 extract_decls.py < cmpr.c > fdecls.h

install: dist/cmpr
	install -m 755 dist/cmpr /usr/local/bin/cmpr
