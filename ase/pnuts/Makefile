
include ../MakefileCommon.mk

all:
	if [ ! -e "/usr/share/java/pnuts.jar" ]; then \
		echo "pnuts.jar not found"; \
		exit 1; \
	fi
	$(SUNJDK)/bin/javac -g -source 1.4 -classpath \
		../asejni.jar:/usr/share/java/pnuts.jar \
		asejni/*/*.java
	jar -cf ../libase-scr-Pnuts-pnuts.jar asejni/*/*.class

clean:
	rm -f asejni/*.class

