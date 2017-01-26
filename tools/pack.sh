#!/bin/bash

OUTDIR=${1-out/dispmanx}
DESTDIR=chromium-ozone-dispmanx
ARCHIVE=$DESTDIR.tar.gz

if ! test -d "$OUTDIR"; then
        echo "Not a directory: $OUTDIR"
        exit 1
fi

cd $OUTDIR
rm -rf $DESTDIR $ARCHIVE
mkdir -p $DESTDIR

tar     --exclude $DESTDIR \
        --exclude $ARCHIVE \
        --exclude gen \
        --exclude obj \
        --exclude pyproto \
        --exclude resources \
        --exclude Packages \
        --exclude 'newlib_*' \
        --exclude 'nacl_bootstrap*' \
        --exclude 'glibc_*' \
        --exclude 'clang_*' \
        --exclude 'irt_*' \
        --exclude '*.ninja*' \
        -cf - . | tar -C $DESTDIR -xf -

tar -czvf $ARCHIVE $DESTDIR
