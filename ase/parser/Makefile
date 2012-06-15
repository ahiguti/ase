
all: test

aseparser: aseparser.y
	bison aseparser.y
	gcc -g -Wall -O0 aseparser.tab.c -lm -o aseparser # -lefence

clean:
	rm -f aseparser *.tab.c

test: aseparser
	./aseparser test1.txt
	./aseparser test2.txt
