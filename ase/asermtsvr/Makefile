
GTCPP_DIR = ../gtcpp
TARGETS = asermtserver asermtserver-dl libase-lib-Remote.so

include ../MakefileCommon.mk

all: $(TARGETS)

asermtserver: asermtserver.cpp
	$(CXX) $(CXXFLAGS) -I.. $^ -o $@ \
		-rdynamic \
		-Wl,-whole-archive \
		$(GTCPP_DIR)/libgtposix.a $(GTCPP_DIR)/libgtbase.a \
		../libase.a \
		-Wl,-no-whole-archive -ldl -lpthread -lresolv
	cp -a asermtserver ..

asermtserver-dl: asermtserver.cpp
	$(CXX) $(CXXFLAGS) -I.. $^ -o $@ -L$(GTCPP_DIR) \
		-lgtposix -lgtbase -L.. -lase -lpthread -ldl -lresolv
	cp -a asermtserver-dl ..

libase-lib-Remote.so: asermtclient.cpp
	$(CXX) $(CXXFLAGS) -I.. -I../../.. -fPIC -shared $^ -o $@
	cp -a libase-lib-Remote.so ..

clean:
	rm -f $(TARGETS)

