


ngcmds:
	asn1c -fskeletons-copy ngmodule.asn1
	gcc -g -O -DPDU=NGCmds -I. -o ngcmds *.c

clean:
	rm -f *.[ch] Makefile.am.sample ngcmds

