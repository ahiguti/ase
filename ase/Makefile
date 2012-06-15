
include MakefileCommon.mk

TARGETS_MAIN = libase.so aserun
TARGETS_REMOTE = asermtserver asermtserver \
	libase-lib-Remote.so libase-lib-Remote.so
TARGETS_DUMMY = libase-scr-Dummy.so
TARGETS_CPP = libase-scr-CPP.so
TARGETS_LUA = libase-scr-Lua.so
TARGETS_JS = libase-scr-JS.so
TARGETS_JAVA = libase-scr-Java.so
TARGETS_RHINO = $(TARGETS_JAVA) libase-scr-Rhino-js.jar
TARGETS_PNUTS = $(TARGETS_JAVA) libase-scr-Pnuts-pnuts.jar

TARGETS_MINI = \
	$(TARGETS_MAIN) \
	$(TARGETS_REMOTE) \
	$(TARGETS_DUMMY) \
	$(TARGETS_CPP)

TARGETS_ALL = \
	$(TARGETS_MINI) \
	$(TARGETS_LUA) \
	$(TARGETS_JAVA) \
	$(TARGETS_JS) \
	$(TARGETS_RHINO)

%.o: %.cpp
	$(CXX) -c -I. -fPIC $(CXXFLAGS) -o $@ $<

mini: $(TARGETS_MINI)

all: $(TARGETS_ALL) test

debug:
	+env ASE_BUILD_DEBUG=1 $(MAKE) all

dm:
	+env ASE_BUILD_DEBUG=1 ASE_USE_DEBUGMALLOC=1 $(MAKE) all

ef:
	+env ASE_BUILD_DEBUG=1 ASE_USE_EF=1 $(MAKE) all

mudflap:
	+env ASE_BUILD_DEBUG=1 ASE_USE_MUDFLAP=1 $(MAKE) all

gcov:
	+env ASE_USE_GCOV=1 $(MAKE) all

#######################################################################

ASE_OBJS = \
	ase.o aseutfconv.o aseexcept.o asetyped.o asebindmethod.o \
	asestr.o asestrutil.o aseutf16.o asedllhandle.o aseio.o \
	asedelayvar.o asedelayscr.o aseruntime.o \
	asestrmap.o asearray.o asefuncobj.o aseenum.o asecclass.o \
	aseflags.o aseintermed.o asemalloc.o aseasn1.o asermt_protocol.o \
	asermt_concurrent.o asermt_client.o aseio_thread.o aseio_listener.o \
	aseio_bufstream.o aseio_getaddrinfo.o asestrbuf.o aseserialize.o

libase.a: $(ASE_OBJS)
	cd aseasn1 && $(MAKE)
	rm -f libase.a
	$(AR) rc libase.a $(ASE_OBJS) \
		aseasn1/aseremote_impl.o aseasn1/generated/*.o
	$(AR) s libase.a

libase.so: $(ASE_OBJS)
	cd aseasn1 && $(MAKE)
	$(CXX) $(CXXFLAGS) $(ASE_OBJS) \
		aseasn1/aseremote_impl.o aseasn1/generated/*.o \
		-shared -fPIC -o libase.so

aserun: aserun.cpp libase.so
	$(CXX) -I. -fPIC $(CXXFLAGS) aserun.cpp \
		-rdynamic -ldl -lpthread $(ASEDBGLIBS) -L. -lase \
		-o aserun

libase-scr-CPP.so: aseccloader.cpp
	$(CXX) $(CXXFLAGS) -shared -fPIC -I. aseccloader.cpp \
		-L. -lase -o libase-scr-CPP.so

asermtserver: asermtserver.cpp
	$(CXX) $(CXXFLAGS) -I. $^ -o $@ $(ASEDBGLIBS) \
		-L. -lase -lpthread -ldl

libase-lib-Remote.so: asermt_client_main.cpp
	$(CXX) $(CXXFLAGS) -I. -fPIC -shared $^ -o $@ \
	-L. -lase -lpthread

ASEDUMMY_OBJS = \
	asedummy.o asedummy_sctx.o asedummy_aseimpl.o asedummy_util.o \
	asedummy_asepr.o

libase-scr-Dummy.so: $(ASEDUMMY_OBJS)
	$(CXX) $(CXXFLAGS) -shared -fPIC -I. \
		$(ASEDUMMY_OBJS) -L. -lase -o libase-scr-Dummy.so

ASELUA_OBJS = \
	aselua.o aselua_sctx.o aselua_aseimpl.o aselua_util.o aselua_asepr.o
ASELUA_SRCS = $(ASELUA_OBJS:.o=.cpp)

libase-scr-Lua.so: $(ASELUA_SRCS)
	$(CXX) $(CXXFLAGS) $(LUACPPFLAGS) -shared -fPIC -I. \
		$(ASELUA_SRCS) -lluacpp -L. -lase -o libase-scr-Lua.so

ASEJS_OBJS = asejs.o asejs_sctx.o asejs_asepr.o asejs_aseimpl.o asejs_util.o
ASEJS_SRCS = $(ASEJS_OBJS:.o=.cpp)

libase-scr-JS.so: $(ASEJS_SRCS)
	$(CXX) $(CXXFLAGS) $(JSFLAGS) -shared -fPIC -I. \
		$(ASEJS_SRCS) -ljs -L. -lase -o libase-scr-JS.so

ASECOM_OBJS = \
	asecom.o asecom_sctx.o asecom_aseimpl.o asecom_util.o asecom_asepr.o

libase-scr-COM.so: $(ASECOM_OBJS)
	$(CXX) $(CXXFLAGS) $(COMFLAGS) -shared -fPIC -I. \
		$(ASECOM_OBJS) -L. -lase -o libase-scr-COM.so

ASEJNI_OBJS = \
	asejni.o asejni_sctx.o asejni_util.o asejni_priv.o \
	asejni_classinfo.o asejni_jobjpr.o asejni_jcl.o asejni_nprx.o \
	asejni_runtime.o
ASEJNI_SRCS = $(ASEJNI_OBJS:.o=.cpp)

libase-scr-Java.so: $(ASEJNI_SRCS)
	$(SUNJDK)/bin/javac -Xlint:deprecation -g asejni/*.java
	jar -cf asejni.jar asejni/*.class
	rm -f asejni/*.class
	$(CXX) $(SUNJDKFLAGS) $(CXXFLAGS) -fno-strict-aliasing -fPIC \
		-I. -c $(ASEJNI_SRCS)
	$(CXX) $(SUNJDKFLAGS) $(CXXFLAGS) -shared -fPIC -I. \
		$(ASEJNI_OBJS) $(ASEDBGLIBS) \
		-L. -lase \
		-o libase-scr-Java.so

libase-scr-Rhino-js.jar:
	cd rhino && $(MAKE); \

libase-scr-Pnuts-pnuts.jar:
	cd pnuts && $(MAKE); \

#######################################################################

prepare:
	find ../depslns/lua/ -name "*.dll" -exec cp -f {} . \;
	find ../deps/nsprpub -name "libnspr4.dll" -exec cp -f {} . \;
	find ../depslns/js -name "*.dll" -exec cp -f {} . \;
	find ../pmcjs/libpmcjs.dll -exec cp -f {} . \;
	javac -g asejni/*.java
	jar -cf asejni.jar asejni/*.class

buildsetup:
	cd setup; $(MAKE) buildsetup

clean:
	rm -f *.o *.a *.class TestClass.h $(TARGETS_ALL) asermtserver
	find .  -name "*.so" -o -name "*.o" -o -name "*.class" -o \
		-name "*.obj" -o -name "*.exe" -o -name "*.exe" -o \
		-name "*.a" -o \
		-name "*.dll" -o -name "*.lib" -o -name "*.ncb" -o \
		-name "*.idb" -o -name "*.pdb" -o -name "*.aps" -o \
		-name "*.ilk" -o -name "*.exp" -o -name "*.suo" -o \
		-name "*.dll.manifest" -o -name "*.exe.manifest" -o \
		-name "*.dll.manifest" -o -name "*.exe.manifest" -o \
		-name "*.intermediate.manifest" \
		| xargs rm -f
	rm -f asejava/*.class
	rm -f asejava/*.h
	rm -f asejni/*.class
	rm -f asejni/*.h
	rm -f libase-*/asejni/*.class
	rm -f *.gcda *.gcno *.gcov
	rm -f asejni.jar
	rm -f setup/*.exe setup/*.iss
	cd misc && $(MAKE) clean
	cd rhino && $(MAKE) clean
	cd pnuts && $(MAKE) clean
	cd tests && $(MAKE) clean
	cd samples && sh clean.sh
	cd svrsamples && sh clean.sh
	# rm -f ase.spec

tar:
	sed -e "s/___VERSION___/$(ASEVERSION)/g" asespec.in > ase.spec && \
	wd=`pwd` && dnm=`basename $$wd` && \
	pushd .. && rm -rf  "$$dnm.tmp" && mkdir "$$dnm.tmp" && \
	tar c --exclude ".svn" "$$dnm" | tar x -C "$$dnm.tmp/" && \
	cd "$$dnm.tmp" && \
	mv "$$dnm" "ase-$(ASEVERSION)" && \
	cd "ase-$(ASEVERSION)" && \
	$(MAKE) clean && cd .. && \
	tar cfz "ase-$(ASEVERSION).tar.gz" "ase-$(ASEVERSION)" && \
	mv "ase-$(ASEVERSION).tar.gz" ../dist && \
	cd .. && rm -rf "$$dnm.tmp"

test:
	cd tests; $(MAKE)

retest:
	cd tests; $(MAKE) clean; $(MAKE)

