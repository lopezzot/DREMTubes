#!/bin/bash
list=`ls /home/dreamtest/working/data/ | grep "^datafile_run11[0-9][0-9]" | grep ".dat$"`
listing2=$(ls /home/dreamtest/working/data/datafile_run11??.dat)
listing3=$(ls /home/dreamtest/working/data/datafile_run11??.dat)
for i in $listing3 ; do
  runnr=${i:41:4}
  echo "$runnr "`doOfflineHisto $runnr data`
done
exit
