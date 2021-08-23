#!/bin/bash

WORKDIR=$1
SECONDS=$2
TMP=/tmp/oscclean.tmp

CURRTIME=$[`date +%s`]
echo $CURRTIME

THRTIME=$[$[CURRTIME]-$[SECONDS]]

FILES=`ls -lrt --time-style=+%s "$WORKDIR"`

FILES=`echo "$FILES" |grep -v total`

echo "$FILES" > $TMP

while read i
  do 
  TIME=`echo "$i"|awk '{print $6}'`
  if [ $TIME -lt $THRTIME ]
      then
      FILE=`echo "$i"|awk '{print $7}'`
      rm $WORKDIR/$FILE
  fi
done < $TMP
