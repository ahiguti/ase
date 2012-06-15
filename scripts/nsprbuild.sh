#!/bin/sh

if ( pwd | fgrep -q ' ' ); then
  echo "PWD='`pwd`', which contains a space character."
  echo "You can not build nsprpub."
  exit 1
fi

cd deps/nsprpub
if [ ! -f Makefile ]; then
  ./configure --disable-optimize --enable-debug
fi
make

