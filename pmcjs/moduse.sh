#!/bin/sh
echo *.js | xargs grep 'Perl.Use' | \
  sed -s 's/\([^\:]*\):.*Perl.Use(.\(.*\).);/\1    \2/'

