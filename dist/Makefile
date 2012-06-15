
none:

all: aserpm
# all: pmcrpm aserpm

clean:
	rm -rf *.tar.gz *.rpm *.exe rpm

pmctar:
	cd ../pmcjs && $(MAKE) tar

asetar:
	cd ../ase && $(MAKE) tar

rpmdir:
	mkdir -p rpm/SPECS rpm/BUILD rpm/RPMS rpm/SRPMS

moverpm:
	-mv rpm/RPMS/*/*.rpm .
	-mv rpm/SRPMS/*.rpm .

pmcrpm: pmctar rpmdir
	rpmbuild --define "_topdir `pwd`/rpm" -ta pmcjs-*.tar.gz
	$(MAKE) moverpm

aserpm: asetar rpmdir
	rpmbuild --define "_topdir `pwd`/rpm" -ta ase-*.tar.gz
	$(MAKE) moverpm

