
include /usr/libexec/ase/MakefileCommon.mk

run: libase-lib-ASETestNative.so
	time aserun testint.lua
	time aserun testintfast.lua
	time aserun teststr.lua
	time aserun teststrfast.lua
	time aserun testcbint.lua
	time aserun testcbstr.lua

libase-lib-ASETestNative.so: asetestnative.cpp
	$(CXX) $(CXXFLAGS) -fPIC -shared asetestnative.cpp \
		-o libase-lib-ASETestNative.so -lase

clean:
	rm -f *.so
	rm -f *.dll *.lib *.exp *.ilk *.pdb
	cd vc8; rm -rf *.ncb *.suo *.vcproj.* Debug Release

