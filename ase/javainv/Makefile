
include ../MakefileCommon.mk

test: javainv.class
	env ASE_LIBEXEC=.. LD_LIBRARY_PATH=.. $(SUNJDK)/bin/java \
		-classpath ../asejni.jar:. javainv

javainv.class: javainv.java
	$(SUNJDK)/bin/javac -classpath ../asejni.jar -g javainv.java

clean:
	rm -f *.class

