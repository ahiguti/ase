
all: loadtest reftest lib-testfoo.so

loadtest: loadtest.java
	gcj -O --main=loadtest loadtest.java -o loadtest

reftest: reftest.java
	gcj -O --main=reftest reftest.java -o reftest

lib-testfoo.so: testfoo.java
	gcj -O -shared -fPIC testfoo.java -o lib-testfoo.so

run:
	time env LD_LIBRARY_PATH=. ./loadtest

clean:
	rm -f *.o *.so *.class
	rm -f loadtest reftest
