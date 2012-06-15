#!/bin/sh

TESTS="`seq 1 40`";

source ../compat.sh

../../jspl test0.js foo bar baz > test0.log 2>> stderr.log

for i in $TESTS; do
  ../../jspl test$i.js > test$i.log 2>> stderr.log
done
for i in 0 $TESTS; do
  if ! $DIFF -u test$i.log test$i.expected; then
    echo "test$i failed";
    exit 1
  fi
done
echo "OK."
exit 0

