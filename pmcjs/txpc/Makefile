

ifndef MOZINC
MOZINC = $(shell perl ./mozconfig.pl inc)
MOZLIB = $(shell perl ./mozconfig.pl libs)
MOZIDL = $(shell perl ./mozconfig.pl idl)
JS_INCLUDES = -I$(MOZINC) -I$(MOZINC)/js -I$(MOZINC)/xpcom \
  -I$(MOZINC)/xpconnect -I/usr/include/nspr4 -DUSE_XPCOM=$(MOZLIB)
JS_LIBPATH = -L$(MOZLIB) -lmozjs -lxpcom -Wl,--rpath=$(MOZLIB)
endif

CFLAGS += $(JS_INCLUDES) $(DEBUGMALLOC_CFLAGS)
LDFLAGS += $(JS_LIBPATH) -lnspr4

CFLAGS += -g -O -DDEBUG -fPIC #-Werror
CFLAGS += -DXP_UNIX -DJS_THREADSAFE
CC = gcc
CXXFLAGS = $(CFLAGS)
XPIDL = $(MOZLIB)/run-mozilla.sh $(MOZLIB)/xpidl -I$(MOZIDL)

TXPC_OBJS = \
	txpc.o

all:
	$(XPIDL) -m header itxpc.idl
	$(XPIDL) -m typelib itxpc.idl
	$(MAKE) libtxpc.so

install:
	install -m 755 libtxpc.so $(MOZLIB)/components/
	install -m 644 itxpc.xpt $(MOZLIB)/components/
	$(MOZLIB)/run-mozilla.sh $(MOZLIB)/regxpcom

uninstall:
	rm -f $(MOZLIB)/components/libtxpc.so
	rm -f $(MOZLIB)/components/itxpc.xpt
	$(MOZLIB)/run-mozilla.sh $(MOZLIB)/regxpcom

libtxpc.so: $(TXPC_OBJS)
	$(CC) -shared $(CFLAGS) $(TXPC_OBJS) $(LDFLAGS) -o libtxpc.so

clean:
	rm -rf *.o *.exe jspl pmctest *.so *.dll *.exp *.lib *.obj *.pdb *.rsp
	rm -rf *.a *.so *.pm
	rm -rf itxpc.h itxpc.xpt

