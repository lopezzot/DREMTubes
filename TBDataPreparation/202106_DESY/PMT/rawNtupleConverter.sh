#!/bin/bash

if [ $# -ne 2 ]; then
  echo "Usage: $0 filename.bz2 TDC/noTDC"
  exit 7
fi

WORKDIR=/tmp 
XPATH=/eos/user/i/ideadr/TB2021_Desy/rawData
IFILE=$1
OUTFILE=`basename $IFILE .bz2`
echo $OUTFILE
bunzip2 -c -k $XPATH/$1 > $WORKDIR/$OUTFILE
ls -l $WORKDIR/$OUTFILE

echo $2
if [ "$2" = "TDC" ]; then
   echo "using ./rawToNtuple_TDC $WORKDIR/$OUTFILE"
   ./rawToNtuple_TDC $WORKDIR/$OUTFILE
else
   echo "using ./rawToNtuple $WORKDIR/$OUTFILE"
   ./rawToNtuple $WORKDIR/$OUTFILE
fi

rm $WORKDIR/$OUTFILE
