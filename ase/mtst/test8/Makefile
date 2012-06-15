
TARGETS = cnitest jnitest jnitest2 sjnitest

SUNJDK = /usr/java/jdk1.5.0_09
ARCH = amd64
SUNJDKLIBPATH = $(SUNJDK)/jre/lib/$(ARCH)/server
SUNJDKLDOPT = -L$(SUNJDKLIBPATH) -Wl,-rpath=$(SUNJDKLIBPATH) -ljvm 

all: $(TARGETS)

TestClass.o: TestClass.java
	gcj -O -g -Wall -c -fPIC TestClass.java

TestClass.class: TestClass.java
	gcj -O -g -Wall -C TestClass.java
	gcjh TestClass

jnitest: jnitest.cpp TestClass.o
	g++ -O -g -Wall jnitest.cpp TestClass.o -o jnitest -lgcj

jnitest2: jnitest.cpp TestClass.class
	g++ -O -g -Wall jnitest.cpp -o jnitest2 -lgcj

sjnitest: jnitest.cpp TestClass.class
	g++ -O -g -Wall jnitest.cpp -o sjnitest $(SUNJDKLDOPT)

cnitest: cnitest.cpp TestClass.o TestClass.class
	g++ -O -g -Wall cnitest.cpp TestClass.o -o cnitest -lgcj

clean:
	rm -f *.o *.class TestClass.h $(TARGETS)


