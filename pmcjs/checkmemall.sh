#!/bin/sh

if [ "$1" == "" ]; then
  JSFILES=`find . -name "*.js" | grep -v "loop" | grep -v "timing"`;
else
  echo "examine incl 'loop' and 'timing' scripts";
  JSFILES=`find . -name "*.js"`;
fi
JSPL=`pwd`/jspl
CHKM=`pwd`/checkmem.pl
for i in $JSFILES; do
  if grep -q 'SKIP_CHECKMEM' "$i"; then
    echo "skip $i";
    continue;
  fi;
  echo "running $i";
  d=`dirname "$i"`;
  b=`basename "$i"`;
  pushd "$d" > /dev/null;
  "$JSPL" "$b" 2> stderr.log > stdout.log
  if ! perl "$CHKM" < stderr.log; then
    echo "$i FAILED";
    exit 1;
  fi;
  popd > /dev/null;
done
echo "ok";

#### Note: it's ok that tests/test_05_builtin/test5.js fails.
#### it failes because of fork().

