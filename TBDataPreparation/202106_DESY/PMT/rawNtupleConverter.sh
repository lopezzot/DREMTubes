#!/bin/bash

if [ $# -ne 2 ]; then
  echo "Usage: $0 filename.bz2 TDC/noTDC"
  exit 7
fi

XPATH=/eos/user/i/ideadr/TB2021_Desy/rawData
IFILE=$1
OUTFILE=`basename $IFILE .bz2`
echo $OUTFILE
bunzip2 -k $XPATH/$1
ls -l $XPATH/$OUTFILE

echo $2
if [ "$2" = "TDC" ]; then
   echo "using ./rawToNtuple_TDC $OUTFILE"
   ./rawToNtuple_TDC $OUTFILE
else
   echo "using ./rawToNtuple $OUTFILE"
   ./rawToNtuple $OUTFILE
fi

rm $XPATH/$OUTFILE
