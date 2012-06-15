#!/bin/sh

pushd deps/gmp
if [ ! -f gmp.h ]; then
  ./configure
  cp gmp-mparam.h t.h
  mv t.h gmp-mparam.h
fi
popd

