#!/bin/bash -e

# pbc lexer and parser appears before configure script 
[ x`which flex` == x ] && echo 'need flex' && exit 1
[ x`which bison` == x ] && echo 'need bison' && exit 1
if [ x`which libtoolize` != x ]; then
    LT=libtoolize
elif [ x`which glibtoolize` != x ]; then
    LT=glibtoolize
else
    exit 1
fi
$LT --force --copy
aclocal 
autoheader
automake --add-missing --copy --gnu
autoconf 

if test -f config.status; then
    sh config.status
fi
