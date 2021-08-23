#!/bin/bash

cd /home/dreamtest/SPS.2021.08

DATE=`date +%Y.%m.%d`
HOUR=`date +%H:%M`

echo "started at " $DATE " " $HOUR
echo "use 'kill -9' in order to kill the script"

while true; do 
	myReadOutSPS2021 &>> logfile.all.txt

	DATE=`date +%Y.%m.%d`
	HOUR=`date +%H:%M`

	echo "dead at " $DATE " " $HOUR
	echo "data file is " `ls sps2021data.* | tail -1`
	sleep 1
done

