#!/bin/bash

FILES=`ls ./src/*.c`
BINDIR='bin/'

if [ ! -d "$BINDIR" ]; then
    mkdir $BINDIR && echo "Made directory $BINDIR"
fi

for file in $FILES
do
    base=$(basename $file .c)
    mpicc $file -o $BINDIR$base
done


