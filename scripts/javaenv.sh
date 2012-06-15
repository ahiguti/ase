#!/bin/sh
#export PATH="/cygdrive/c/mingw/bin/:$PATH"
jdk=`echo /cygdrive/c/Program\ Files/Java/jdk* | head -1`
export PATH="$jdk/bin/:$PATH"
exec $*

