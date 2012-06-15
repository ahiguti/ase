#!/bin/sh

if [ -f /usr/bin/diff.exe ]; then
  export DIFF='diff --ignore-space --strip-trailing-cr'
elif [ "`uname`" = "Darwin" ]; then
  export DIFF='diff'
else
  export DIFF='diff --ignore-space --strip-trailing-cr'
fi

