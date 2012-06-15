
SYSNAME = $(shell uname -o)

##########################################################################

all:

release:
	if [ "$(SYSNAME)" = "Cygwin" ]; then \
	  $(MAKE) aserelease; \
	else \
	  $(MAKE) aseunix; \
	fi

clean: cleanobj
	-cd deps/nsprpub && $(MAKE) clean
	-cd pmcjs && $(MAKE) clean
	-cd ase && $(MAKE) clean
	-if [ "$(SYSNAME)" = "Cygwin" ]; then \
	  for i in $(VSSOLS); do \
	    ./scripts/devenvwrap90.bat $$i /clean Release; done; \
	  for i in $(VSSOLS); do \
	    ./scripts/devenvwrap90.bat $$i /clean Debug; done; \
	 fi
	-cd dist && $(MAKE) clean

cleanobj:
	find .  -name "*.so" -o -name "*.o" -o -name "*.class" -o \
		-name "*.obj" -o -name "*.exe" -o -name "*.exe" -o \
		-name "*.dll" -o -name "*.lib" -o -name "*.ncb" -o \
		-name "*.idb" -o -name "*.pdb" -o -name "*.aps" -o \
		-name "*.ilk" -o -name "*.exp" -o -name "*.suo" -o \
		-name "*.vcproj.*.*.*" -o \
		-name "*.dll.manifest" -o -name "*.exe.manifest" -o \
		-name "*.dll.manifest" -o -name "*.exe.manifest" -o \
		-name "*.intermediate.manifest" \
		| xargs rm -f
	find . -name "Release" -a -type d | xargs rm -rf
	find . -name "Debug" -a -type d | xargs rm -rf

tar:
	cd ase && $(MAKE) tar
#	cd pmcjs && $(MAKE) tar

##########################################################################

aseunix:
	cd dist && $(MAKE) aserpm
#	cd dist && $(MAKE) pmcrpm aserpm

preparedeps:
	cd deps && $(MAKE) preparedeps

pmcrelease: preparedeps
	./scripts/vsenv90.bat ./scripts/moztools.sh ./scripts/nsprbuild.sh
	./scripts/devenvwrap90.bat depslns/js/js.sln /build Release
	cd pmcjs && ../scripts/gmakewrap71.bat -f Makefile.win32

pmcdebug: preparedeps
	./scripts/vsenv90.bat ./scripts/moztools.sh ./scripts/nsprbuild.sh
	./scripts/devenvwrap90.bat depslns/js/js.sln /build Debug
	cd pmcjs && ../scripts/gmakewrap71.bat -f Makefile.win32

aserelease: pmcrelease
	./scripts/devenvwrap90.bat depslns/lua/lua.sln /build Release
	./scripts/devenvwrap90.bat ase/vc9/ase/ase.sln /build Release
	cd ase && ../scripts/javaenv.sh $(MAKE) prepare buildsetup

asedebug: pmcdebug
	./scripts/devenvwrap90.bat depslns/lua/lua.sln /build Debug
	./scripts/devenvwrap90.bat ase/vc9/ase/ase.sln /build Debug
	cd ase && ../scripts/javaenv.sh $(MAKE) prepare

osslbuild:
	./scripts/opensslbuild.sh

