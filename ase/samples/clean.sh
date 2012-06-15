#!/bin/sh

find .  -name "*.o" -o -name "*.a" -o -name "*.so" -o \
	-name "*.obj" -o -name "*.exe" -o -name "*.exe" -o \
	-name "*.dll" -o -name "*.lib" -o -name "*.ncb" -o \
	-name "*.idb" -o -name "*.pdb" -o -name "*.aps" -o \
	-name "*.ilk" -o -name "*.exp" -o -name "*.suo" -o \
	-name "*.dll.manifest" -o -name "*.exe.manifest" -o \
	-name "*.dll.manifest" -o -name "*.exe.manifest" -o \
	-name "*.intermediate.manifest" -o \
	-name "_*.*" -o -name "dlldata.c" -o \
	-name "*.vcproj.*" -o \
	-name "*.class" -o -name "*.jar" \
	| xargs rm -f
find . -name "Debug" -o -name "Release" -o -name "bin" -o -name "obj" \
	| xargs rm -rf
rm -f *.png
