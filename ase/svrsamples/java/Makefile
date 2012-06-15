
include /usr/libexec/ase/MakefileCommon.mk

run: ASETestJava.jar
	time aserun testint.lua
	time aserun teststr.lua
	time aserun testcbint.lua
	time aserun testcbstr.lua

ASETestJava.jar: ASETestJava/TestObj.java ASETestJava/CallbackInterface.java
	$(SUNJDK)/bin/javac -g ASETestJava/TestObj.java
	$(SUNJDK)/bin/jar -cf ASETestJava.jar ASETestJava/*.class

clean:
	rm -f *.jar */*.class

