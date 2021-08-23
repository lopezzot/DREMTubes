#!/bin/bash
if [ "$1" = "" ] ; then
  echo "Usage $0 runnumber"
  exit 1
fi
file=/home/dreamtest/working/data/datafile_run$1.dat
runnr=${file:41:4}
echo "$runnr "`doOfflineHisto $runnr data`
exit
