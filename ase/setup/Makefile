
include ../MakefileCommon.mk

COMPILECMD = "C:/Program Files/Inno Setup 5/ISCC.exe"
ASENAMEVER = Abstract Script Engines $(ASEVERSION)

all:

buildsetup:
	cat aseiss.in | \
	  sed -e "s/^AppVerName=.*/AppVerName=$(ASENAMEVER)/g" > ase.iss
	$(COMPILECMD) /O../.. /Fdist/asesetup-$(ASEVERSION) ase.iss
	rm -f ase.iss

