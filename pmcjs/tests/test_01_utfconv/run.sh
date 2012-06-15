#!/bin/sh

if [ "`uname -o`" = "Cygwin" ]; then
  echo "Skip."
  exit 0
fi

if [ ! -f ../../pmctest ]; then
  echo executable not found;
  exit 1
fi
../../pmctest gen32 > u32.txt 2>> stderr.log
iconv -f UTF32 -t UTF16 < u32.txt | tail -c +3 > u16.txt
iconv -f UTF32 -t UTF8 < u32.txt > u8.txt
../../pmctest 8to16 u8.txt > pmc16.txt 2>> stderr.log
../../pmctest 16to8 u16.txt > pmc8.txt 2>> stderr.log
../../pmctest 8to16err > u8e.txt 2>> stderr.log
../../pmctest 16to8err > u16e.txt 2>> stderr.log
if ! cmp u16.txt pmc16.txt; then
  echo "pmc16 failed";
  exit 1
fi
if ! cmp u8.txt pmc8.txt; then
  echo "pmc8 failed";
  exit 1
fi
if ! cmp u8e.txt u8e.expected; then
  echo "u8e failed";
  exit 1
fi
if ! cmp u16e.txt u16e.expected; then
  echo "u16e failed";
  exit 1
fi
echo "OK."
exit 0
