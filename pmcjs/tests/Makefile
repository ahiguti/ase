
SHELL=/bin/bash

all: test

force:
	for i in test_*; do \
	  pushd $$i > /dev/null; \
	  echo -n "$$i ... "; \
	  if [ -f ./DONE ]; then \
	    echo "skip"; \
	  elif ! ./run.sh > test.log; then \
	    echo -n "FORCE ... "; \
	    $(MAKE) accept; \
	    if ! ./run.sh > test.log; then \
	      echo "FAILED"; \
	      exit 1; \
	    fi; \
	  else \
	    echo "success"; \
	  fi; \
	  touch ./DONE; \
	  popd > /dev/null; \
	done; \
	exit 0;

test:
	for i in test_*; do \
	  pushd $$i > /dev/null; \
	  echo -n "$$i ... "; \
	  if [ -f ./DONE ]; then \
	    echo "skip"; \
	  elif ! ./run.sh > test.log; then \
	    echo "FAILED"; \
	    exit 1; \
	  else \
	    echo "success"; \
	  fi; \
	  touch ./DONE; \
	  popd > /dev/null; \
	done; \
	exit 0;

retest: clean test

xmlformat:
	for i in test_*/*.xml; do \
	  xmllint -format "$$i" > xmllint.tmp.log && \
	  mv xmllint.tmp.log "$$i"; \
	done;

clean:
	for i in test_*; do \
	  pushd $$i > /dev/null; \
	  if [ -f Makefile ]; then \
	    $(MAKE) clean; \
	  fi; \
	  rm -rf ./GWS_db; \
	  rm -f ./DONE; \
	  popd > /dev/null; \
	done; \
	exit 0;
	rm -f test_*/*.log
	rm -f test_*/*.out
	rm -rf test_*/*_db

cleanprepare:
	rm -f ../run/*.exe
	rm -f ../run/poi.jar
	rm -f ../run/*.dll

