#!/bin/bash

run=`grep RunNumber /home/dreamtest/working/.dreamdaqrc | awk '{print $2}'`
run=`echo $run`


filelist=`ls -altr /home/dreamtest/working/data/datafile_run${run}_*.dat | awk '{ print $9}'`
word_count=`echo $filelist | wc -w`
((word_count -= 2))
i=0
for file in $filelist
do
	if  (( i < word_count ))
	then
	  echo rm -f $file
	  rm -f $file
	else
	  echo $file "left for daq processes"
	fi
	((i += 1))
done
filelist=`ls -altr /home/dreamtest/working/pedestal/pedestal_run${run}_*.dat | awk '{ print $9}'`
word_count=`echo $filelist | wc -w`
((word_count -= 2))
i=0
for file in $filelist
do
	if  (( i < word_count ))
	then
	  echo rm -f $file
	  rm -f $file
	else
	  echo $file "left for daq processes"
	fi
	((i += 1))
done
