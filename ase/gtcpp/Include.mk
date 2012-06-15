
CXXLIBS += -lpthread
GTCPP_DIR = $(TOP_DIR)/asebuilddir/ase/gtcpp
LIBGTBASE = $(GTCPP_DIR)/libgtbase.a
LIBGTPOSIX = $(GTCPP_DIR)/libgtposix.a

ifdef GTCPP_DEBUG
CXXFLAGS += -I$(GTCPP_DIR)/.. -Wall -Wformat=2 -g -O0
endif

ifdef GTCPP_RELEASE
CXXFLAGS += -I$(GTCPP_DIR)/.. -Wall -W -Wformat=2 -g -O3 -DNDEBUG
endif

ifdef GTCPP_PROFILE
CXXFLAGS += -Wall -W -Wformat=2 -p -g -O3 -DNDEBUG
endif

ifdef GTCPP_EFENCE
CXXLIBS += -lefence
endif

CXXDEFS += -DSVN_REVISION="$(shell svnversion -n .)"

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
	$(CXX) -c -I.. -fPIC $(CXXFLAGS) $(CXXDEFS) -o $@ $< 

$(LIBGTBASE):
	pushd $(GTCPP_DIR) && $(MAKE) all && popd

$(LIBGTPOSIX):
	pushd $(GTCPP_DIR) && $(MAKE) all && popd

clean:
	pushd $(GTCPP_DIR) && $(MAKE) clean && popd
	rm -f *.o $(TARGETS) $(CLEANTARGETS)

cleanall: clean

