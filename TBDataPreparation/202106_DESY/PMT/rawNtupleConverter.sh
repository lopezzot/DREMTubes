#!/bin/bash

if [ $# -ne 1 ]; then
  echo "Usage: $0 filename.bz2"
  exit 7
fi

XPATH=/eos/user/i/ideadr/TB2021_Desy/rawData
IFILE=$1
OUTFILE=`basename $IFILE .bz2`
echo $OUTFILE
bunzip2 -k $XPATH/$1
ls -l $XPATH/$OUTFILE

echo "CHIAMA QUI IL TUO PROGRAMMA DANDOGLI \$OUTFILE"
./rawToNtuple $OUTFILE
#rawToNtuple_TDC $OUTFILE

rm $XPATH/$OUTFILE
