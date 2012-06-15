#!/bin/sh
if [ ! -f deps/moztools/bin/nsinstall.exe ]; then
  pushd deps
  rm -rf moztools
  unzip moztools-static.zip
  popd
fi
env PATH="`pwd`/deps/moztools/bin/:$PATH" $*

