
include ../MakefileCommon.mk

all:
	if [ ! -e "/usr/share/java/js.jar" ]; then \
	echo "js.jar not found"; \
		exit 1; \
	fi
	$(SUNJDK)/bin/javac -g -source 1.4 -classpath \
		../asejni.jar:/usr/share/java/js.jar \
		asejni/*/*.java
	jar -cf ../libase-scr-Rhino-js.jar asejni/*/*.class

clean:
	rm -f asejni/*/*.class

