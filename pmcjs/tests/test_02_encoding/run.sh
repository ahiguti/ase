#!/bin/sh

TESTS="1 2 4";

source ../compat.sh

for i in $TESTS; do
  ../../jspl test$i.js > test$i.log 2>> stderr.log
done
for i in $TESTS; do
  if ! $DIFF -u test$i.log test$i.expected; then
    echo "test$i failed";
    exit 1
  fi
done
echo "OK."
exit 0
