
all:
	g++ -shared cfunc.c -Wl,--output-def,cfunc.def -o cfunc.dll
	lib /machine:i386 /def:cfunc.def
	cl call.c cfunc.lib

clean:
	rm -f *.o *.obj *.lib *.dll *.exe *.exp

