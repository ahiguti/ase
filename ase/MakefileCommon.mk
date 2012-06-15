
ifndef CXX
CXX = g++
endif

ifdef ASE_BUILD_DEBUG
COPTIONS = -O0 -fno-default-inline -fstack-protector -Wall -ggdb
CXX += -DASE_DEBUG_CONSISTENCY -DASE_DEBUG_VARIANT_COUNT -DASE_DEBUG_MUTEX
else
COPTIONS = -O3 -Wall -g
CXX += -DNDEBUG
endif

ifdef ASE_USE_MUDFLAP
CXX += -fmudflapth
ASEDBGLIBS = -lmudflapth
endif

ifdef ASE_USE_EF
# NOTE: export EF_ALLOW_MALLOC_0=1
ASEDBGLIBS = -lefence
endif

ifdef ASE_USE_DEBUGMALLOC
CXX += -finstrument-functions
ASEDBGLIBS = debugmalloc.c
endif

ifdef ASE_USE_GCOV
CXX += -fprofile-arcs
endif

ifeq ($(shell uname -i),i386)
CXX += -march=i686 # requires __sync_fetch_and_add
endif

ifndef LUACPPFLAGS
LUACPPFLAGS = -I/usr/include/luacpp
endif

ifndef JSFLAGS
JSFLAGS = -I/usr/include/js -I/usr/include/nspr4 -DXP_UNIX -DJS_THREADSAFE
endif

ifndef SUNJDK
SUNJDK = $(shell \
	if [ -d /usr/java/default ]; \
	then echo /usr/java/default; \
	else echo /usr/lib/jvm/java; \
	fi)
endif

CXXFLAGS = $(COPTIONS) -DASE_CXX="$(CXX)"

ifdef ASE_DEFAULT_LIBEXEC
CXXFLAGS += -DASE_DEFAULT_LIBEXEC="$(ASE_DEFAULT_LIBEXEC)"
endif

ifdef ASE_JNI_JARDIR
CXXFLAGS += -DASE_JNI_JARDIR="\"$(ASE_JNI_JARDIR)\""
endif

SUNJDKMD = $(shell dirname `echo $(SUNJDK)/include/*/jni_md.h`)
SUNJDKLIBPATH = $(shell echo $(SUNJDK)/jre/lib/*/server)
SUNJDKFLAGS = -I$(SUNJDK)/include -I$(SUNJDKMD)

ASEREV = $(shell env LANG=C svn info | grep '^Revision:' | cut -d ' ' -f 2 )
ifeq ($(ASEREV),)
ASEREV = 1
endif
ASEVERSION = 1.0.$(ASEREV)

