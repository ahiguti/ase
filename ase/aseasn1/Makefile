
aseremote_impl.o: aseremote_impl.cpp
	cd generated && $(MAKE)
	$(CXX) -fPIC -c -Wall -g -O -I.. -I./generated $^

clean:
	cd generated && $(MAKE) clean
	rm -f *.o

