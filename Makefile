CC := gcc

.PHONY: all clean debug dev

all: dist/cmpr

CFLAGS := -O2 -Wall
LDFLAGS := -lm

debug: CFLAGS := -g -O0 -Wall -fsanitize=address
debug: dist/cmpr

dev: CFLAGS := -O2 -Wall -Werror -fsanitize=address
dev: dist/cmpr

dist/cmpr: fdecls.h cmpr.c spanio.c
	mkdir -p dist
	(VER=7; D=$$(date +%Y%m%d-%H%M%S); GIT=$$(git log -1 --pretty="%h %f"); echo '#line 2 "cmpr.c"' >cmpr-sed.c; sed 's/\$$VERSION\$$/'"$$VER"' (build: '"$$D"' '"$$GIT"')/' <cmpr.c >>cmpr-sed.c; echo "Version: $$VER (build: $$D $$GIT)"; $(CC) -o dist/cmpr-$$D cmpr-sed.c siphash/siphash.c siphash/halfsiphash.c $(CFLAGS) $(LDFLAGS) && rm -f dist/cmpr && ln -s cmpr-$$D dist/cmpr)

fdecls.h: cmpr.c
	python3 extract_decls.py < cmpr.c > fdecls.h

clean:
	rm -rf dist
