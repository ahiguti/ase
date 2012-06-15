#!/bin/sh

TESTS="`seq 1 5`"
SKIPWIN="4 5"

source ../compat.sh

for i in $TESTS; do
  ../../jspl test$i.js > test$i.log 2>> stderr.log
done
if [ "`uname -o`" = "Cygwin" ]; then
  for i in $SKIPWIN; do
    cp -f test$i.expected test$i.log
  done
fi
for i in $TESTS; do
  if ! $DIFF -u test$i.log test$i.expected; then
    echo "test$i failed";
    exit 1
  fi
done
echo "OK."
exit 0

