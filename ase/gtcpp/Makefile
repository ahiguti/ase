
CXXLIBS = -lpthread -lresolv -lz
CXXFLAGS = -Wall -W -Wformat=2 -g -O3 -DNDEBUG

ifdef GTCPP_DEBUG
#CXXFLAGS = -I.. -Wall -W -Wformat=2 -g -O0
CXXFLAGS = -Wall -Wformat=2 -g -O0
endif

ifdef GTCPP_RELEASE
CXXFLAGS = -Wall -W -Wformat=2 -g -O3 -DNDEBUG
endif

ifdef GTCPP_PROFILE
CXXFLAGS = -Wall -W -Wformat=2 -p -g -O3 -DNDEBUG
endif

ifdef GTCPP_EFENCE
CXXLIBS += -lefence
endif

ifeq ($(shell uname -i),i386)
CXXFLAGS += -march=i686 # requires __sync_fetch_and_add
endif

CXXFLAGS += -I..
CXXDEFS = -DSVN_REVISION="$(shell svnversion -n .)"

TARGETS = btestmain ptestmain karp_rabin kr_varlen krgrep \
	mail2plain mailfmt \
	wordcount findresemble ngramsearch algotestmt smtputil \
	libgtbase.so libgtposix.so

GTBASE_OBJS = string_buffer.o string_util.o rfc822.o base64.o \
	aseutfconv.o html.o ascii.o ms932.o quotedpr.o escape_hex.o \
	aho_corasick.o horspool.o
GTPOSIX_OBJS = error.o posix_error.o auto_thread.o auto_threadattr.o \
	auto_file.o socket.o tproxy.o unistd.o \
	auto_addrinfo.o resolver.o auto_iconv.o file_util.o debugcount.o \
	scoped_sigaction.o scoped_sigmask.o time.o \
	auto_dir.o auto_glob.o auto_regex.o buffered_file.o \
	rfc822_util.o piggyback_tr.o zlib_util.o \
	smtp_common.o smtp_client.o smtp_server.o tcp_server.o smtp_proxy.o
BTESTMAIN_OBJS = btestmain.o
PTESTMAIN_OBJS = ptestmain.o test_io.o test_socket.o test_smtp.o test_dir.o \
	test_thrtiming.o test_resolver.o test_glob.o test_regex.o \
	test_smtpd.o test_rfc822.o test_html.o test_zlib.o
SMTPUTIL_OBJS = smtputil.o smtputil_smtpc.o smtputil_smtpd.o \
	smtputil_proxy.o smtputil_misc.o

release:
	GTCPP_RELEASE=1 $(MAKE) all

debug:
	GTCPP_DEBUG=1 $(MAKE) all

profile:
	GTCPP_PROFILE=1 $(MAKE) all

efence:
	GTCPP_DEBUG=1 GTCPP_EFENCE=1 $(MAKE) all

all: $(TARGETS)

%.o: %.cpp
	$(CXX) -c -fPIC $(CXXFLAGS) $(CXXDEFS) -o $@ $+

libgtbase.so: $(GTBASE_OBJS)
	rm -f $@
	$(CXX) $(CXXFLAGS) $(CXXDEFS) $+ -shared -fPIC -o $@

libgtposix.so: $(GTPOSIX_OBJS)
	rm -f $@
	$(CXX) $(CXXFLAGS) $(CXXDEFS) $+ -shared -fPIC -lpthread -o $@

libgtbase.a: $(GTBASE_OBJS)
	rm -f $@
	$(AR) cr $@ $(GTBASE_OBJS)

libgtposix.a: $(GTPOSIX_OBJS)
	rm -f $@
	$(AR) cr $@ $(GTPOSIX_OBJS)

karp_rabin: karp_rabin.cpp
	$(CXX) $(CXXFLAGS) $(CXXDEFS) -o $@ $+ $(CXXLIBS)

kr_varlen: kr_varlen.cpp
	$(CXX) $(CXXFLAGS) $(CXXDEFS) -o $@ $+ $(CXXLIBS)

krgrep: krgrep.cpp libgtposix.a libgtbase.a
	$(CXX) $(CXXFLAGS) $(CXXDEFS) -o $@ $+ $(CXXLIBS)

btestmain: $(BTESTMAIN_OBJS) libgtbase.a
	$(CXX) $(CXXFLAGS) $(CXXDEFS) -o $@ $+ $(CXXLIBS)

ptestmain: $(PTESTMAIN_OBJS) libgtposix.a libgtbase.a
	$(CXX) $(CXXFLAGS) $(CXXDEFS) -o $@ $+ $(CXXLIBS)

mail2plain: mail2plain.cpp libgtposix.a libgtbase.a
	$(CXX) $(CXXFLAGS) $(CXXDEFS) -o $@ $+ $(CXXLIBS)

mailfmt: mailfmt.cpp libgtposix.a libgtbase.a
	$(CXX) $(CXXFLAGS) $(CXXDEFS) -o $@ $+ $(CXXLIBS)

wordcount: wordcount.cpp libgtposix.a libgtbase.a
	$(CXX) $(CXXFLAGS) $(CXXDEFS) -o $@ $+ $(CXXLIBS)

findresemble: findresemble.cpp libgtposix.a libgtbase.a
	$(CXX) $(CXXFLAGS) $(CXXDEFS) -o $@ $+ $(CXXLIBS)

ngramsearch: ngramsearch.cpp libgtposix.a libgtbase.a
	$(CXX) $(CXXFLAGS) $(CXXDEFS) -o $@ $+ $(CXXLIBS)

algotestmt: algotestmt.cpp libgtposix.a libgtbase.a
	$(CXX) $(CXXFLAGS) $(CXXDEFS) -o $@ $+ $(CXXLIBS)

smtputil: $(SMTPUTIL_OBJS) libgtposix.a libgtbase.a
	$(CXX) $(CXXFLAGS) $(CXXDEFS) $+  -o $@ $(CXXLIBS)

clean:
	rm -f *.o *.a $(TARGETS)

