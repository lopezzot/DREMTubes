#!/bin/bash

hname=$1
myname=`hostname -s`
shift
if [[ "$hname" != "$myname" ]]; then
  echo "remotely running -> ssh $hname dreamProc.sh $*"
  ssh $hname "dreamProc.sh $*"
else
  echo "locally running -> dreamProc.sh $*"
  dreamProc.sh $*
fi

exit 0
