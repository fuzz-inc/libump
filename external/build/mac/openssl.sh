#!/bin/sh

fullpath() {
	ruby -e "print(File.expand_path('$1'))"
}

CURDIR=`dirname $0`
SRCDIR=`fullpath $CURDIR/../../openssl`
DSTDIR=`fullpath $CURDIR/../../mac/openssl`

rm -rf $DSTDIR
cd $SRCDIR
./config --prefix=$DSTDIR
make clean
make MACOSX_DEPLOYMENT_TARGET=10.10
make install_sw
