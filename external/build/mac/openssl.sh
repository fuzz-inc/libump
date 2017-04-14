#!/bin/sh

fullpath() {
	ruby -e "print(File.expand_path('$1'))"
}

CURDIR=`dirname $0`
SRCDIR=`fullpath $CURDIR/../../openssl`
DSTDIR=`fullpath $CURDIR/../../mac/openssl`

rm -rf $DSTDIR
cd $SRCDIR
./Configure --prefix=$DSTDIR darwin64-x86_64-cc
make clean
make MACOSX_DEPLOYMENT_TARGET=10.9
make install_sw
