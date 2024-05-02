dist/cmpr: cmpr.c spanio.c
	(VER=6; D=$$(date +%Y%m%d-%H%M%S); sed 's/\$$VERSION\$$/'"$$VER"' (build: '"$$D"')/' <cmpr.c >cmpr-sed.c; echo "Version: $$VER (build: $$D)"; gcc -o dist/cmpr-$$D cmpr-sed.c siphash/siphash.c siphash/halfsiphash.c -fsanitize=address -Werror -g -lm -lcurl && rm -f dist/cmpr && ln -s cmpr-$$D dist/cmpr)

install: dist/cmpr
	sudo install -m 755 dist/cmpr /usr/local/bin/cmpr