
JAVAHOME = /usr/java/default
JAVA = $(JAVAHOME)/bin/java
JAVAC = $(JAVAHOME)/bin/javac
JAVAOPT = -classpath "js.jar:."
RHTESTSRC = RhinoTest.java RhinoFoo.java RhinoBar.java RhinoBarWrap.java

test: RhinoTest.class
	$(JAVA) $(JAVAOPT) RhinoTest rhinotest.js

RhinoTest.class: $(RHTESTSRC)
	$(JAVAC) $(JAVAOPT) $(RHTESTSRC)

clean:
	rm *.class

