
PMCREV = $(shell env LANG=C svn info | grep '^Revision:' | cut -d ' ' -f 2 )
PMCVERSION = 1.0.$(PMCREV)

ifndef JS_INCLUDES
JS_INCLUDES = -I/usr/include/js -I/usr/include/nspr4
JS_LIBPATH = -ljs
endif

ifdef USE_XPCOM
ifndef MOZINC
MOZINC = $(shell perl ./mozconfig.pl inc)
MOZLIB = $(shell perl ./mozconfig.pl libs)
JS_INCLUDES = -I$(MOZINC) -I$(MOZINC)/js -I$(MOZINC)/xpcom -I$(MOZINC)/caps \
  -I$(MOZINC)/xpconnect -I/usr/include/nspr4 -DUSE_XPCOM=$(MOZLIB)
JS_LIBPATH = -L$(MOZLIB) -lmozjs -lxpcom -Wl,--rpath=$(MOZLIB)
endif
endif

ifdef USE_DEBUGMALLOC
DEBUGMALLOC_OBJS = debugmalloc.o
DEBUGMALLOC_CFLAGS = -finstrument-functions
endif

COPTFLAGS = -O3 -DNDEBUG -Wuninitialized
ifdef PMC_DEBUG
COPTFLAGS = -O0 -DDEBUG
endif

ifdef USE_MUDFLAP
CFLAGS += -fmudflapth
LDFLAGS += -lmudflapth
endif

ifdef USE_EF
LDFLAGS += -lefence
endif

ifdef USE_GCOV
CFLAGS += -fprofile-arcs
endif


CFLAGS += $(JS_INCLUDES) $(DEBUGMALLOC_CFLAGS)
LDFLAGS += $(JS_LIBPATH)

CFLAGS += -Wall -Wformat=2

CFLAGS += $(COPTFLAGS) -g -fPIC #-Werror
CFLAGS += -DXP_UNIX -DJS_THREADSAFE # -DMOZILLA_INTERNAL_API
CFLAGS += $(shell perl -MExtUtils::Embed -e ccopts)
LDFLAGS += -lnspr4 $(shell perl -MExtUtils::Embed -e ldopts)
CC = gcc
CXX = g++
CXXFLAGS = $(CFLAGS) -fno-rtti -Wno-non-virtual-dtor

JSPL_OBJS = jspl.o
XPCLOAD_OBJS = xpcload.o

PMCJS_OBJS = \
	pmcjs.o poutil.o pmobject.o poprivate.o pmconv.o pjsobject.o \
	pjsglobal.o pjsobjiter.o pmutf.o utfconv.o utfutil.o polist.o \
	pmcexception.o pcondemand.o $(DEBUGMALLOC_OBJS)

TARGETS = libpmcjs.a libpmcjs.so jspl pmctest

ifdef USE_XPCOM
TARGETS += xpcload
endif

all: $(TARGETS) test

debug:
	+env PMC_DEBUG=1 $(MAKE)

ef:
	+env USE_EF=1 PMC_DEBUG=1 $(MAKE)

mudflap:
	+env USE_MUDFLAP=1 PMC_DEBUG=1 $(MAKE)

gcov:
	+env USE_GCOV=1 $(MAKE)

xp:
	+env USE_XPCOM=1 $(MAKE) 


notest: $(TARGETS)

libpmcjs.a: $(PMCJS_OBJS) JavaScript/JavaScript.o
	ar rc libpmcjs.a $(PMCJS_OBJS) JavaScript/JavaScript.o

JavaScript/JavaScript.o: JavaScript/JavaScript.xs
	cd JavaScript; perl -MDevel::PPPort -e 'Devel::PPPort::WriteFile()'
	cd JavaScript; perl Makefile.PL; $(MAKE)

libpmcjs.so: $(PMCJS_OBJS) JavaScript/JavaScript.o
	$(CC) -shared $(CFLAGS) $(PMCJS_OBJS) JavaScript/JavaScript.o \
		$(LDFLAGS) -o libpmcjs.so

jspl: $(PMCJS_OBJS) $(JSPL_OBJS) JavaScript/JavaScript.o
	$(CXX) $(CXXFLAGS) $(PMCJS_OBJS) JavaScript/JavaScript.o \
		$(JSPL_OBJS) -o jspl $(LDFLAGS)

xpcload: $(XPCLOAD_OBJS)
	$(CXX) $(CXXFLAGS) $(XPCLOAD_OBJS) -o xpcload $(LDFLAGS)

pmctest: $(PMCJS_OBJS) pmctest.o
	$(CC) $(CFLAGS) $(PMCJS_OBJS) pmctest.o JavaScript/JavaScript.o \
		-o pmctest $(LDFLAGS)

debugmalloc.o: debugmalloc.c
	$(CC) -c $(CFLAGS) -fno-instrument-functions debugmalloc.c

clean:
	rm -rf *.o *.exe jspl pmctest *.so *.dll *.exp *.lib *.obj *.pdb *.rsp
	rm -rf xpcload
	rm -rf *.a *.so *.pm *.log *.out
	rm -f *.gcda *.gcno *.gcov
	find . -name "*.log" -exec rm {} \;
	cd JavaScript; $(MAKE) clean; rm -f Makefile.old rm -f ppport.h
	cd tests; $(MAKE) clean
	cd scripts; $(MAKE) clean
	cd samples; $(MAKE) clean
	cd txpc; $(MAKE) clean
	cd txpc2; $(MAKE) clean

tar:
	sed -e "s/___VERSION___/$(PMCVERSION)/g" \
	pmcjsspec.in > pmcjs.spec && \
	wd=`pwd` && dnm=`basename $$wd` && \
	pushd .. && rm -rf  "$$dnm.tmp" && mkdir "$$dnm.tmp" && \
	tar c --exclude ".svn" "$$dnm" | tar x -C "$$dnm.tmp/" && \
	cd "$$dnm.tmp" && \
	mv "$$dnm" "pmcjs-$(PMCVERSION)" && \
	cd "pmcjs-$(PMCVERSION)" && \
	$(MAKE) clean && cd .. && \
	tar cfz "pmcjs-$(PMCVERSION).tar.gz" "pmcjs-$(PMCVERSION)" && \
	mv "pmcjs-$(PMCVERSION).tar.gz" ../dist/ && \
	cd .. && rm -rf "$$dnm.tmp"

test: jspl pmctest
	cd tests; $(MAKE)

