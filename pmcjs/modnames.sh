#!/bin/sh
find . -name "*.js" | xargs grep 'Perl.Use' | \
  sed -s 's/.*Perl.Use(.\(.*\).);/\1/' | sort | uniq
