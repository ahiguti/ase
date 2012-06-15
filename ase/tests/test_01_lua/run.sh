#!/bin/sh

#TESTS="`seq -w 1 2`";
TESTS="01 03 05";

source ../compat.sh

for i in $TESTS; do
  args=""
  if [ -f "test$i.args" ]; then args="`cat test$i.args`"; fi
  if [ -f "test$i.expect2" ]; then
    ../../aserun-local test$i.lua $args > test$i.log 2> test$i.log2
  else
    ../../aserun-local test$i.lua $args > test$i.log 2>> stderr.log
  fi
done
for i in $TESTS; do
  if ! $DIFF -u test$i.log test$i.expected; then
    echo "test$i failed";
    exit 1
  fi
  if [ -f "test$i.expect2" ]; then
    lines="`wc -l < test$i.expect2`"
    head -$lines test$i.log2 > test$i.log2h
    if ! $DIFF -u test$i.log2h test$i.expect2 && \
       ! $DIFF -u test$i.log2h test$i.expect2ef; then
      echo "test$i failed";
      exit 1
    fi
  fi
done
echo "OK."
exit 0

