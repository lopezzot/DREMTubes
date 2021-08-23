#!/bin/bash

cd /home/dreamtest/SPS.2021.08

Exec=myReadOutSPS2021

daqPid=`ps -aef | awk '{print $2 " " $8}' | grep $Exec | awk '{print $1}'`
sleep 1

if [[ $daqPid != "" ]]; then
  echo "'$Exec' is running: PID is $daqPid"
  sleep 1
  echo "killing process $daqPid"
  sleep 1
  kill -9 $daqPid
  echo "process $daqPid killed"
  sleep 1

  DATE=`date +%Y.%m.%d`
  HOUR=`date +%H:%M`

  echo "'$Exec' killed at " $DATE " " $HOUR
  echo "data file is" `ls sps2021data.* | tail -1`
  sleep 1

else
  DATE=`date +%Y.%m.%d`
  HOUR=`date +%H:%M`

  echo "'$Exec' is not running at " $DATE " " $HOUR
  echo "last data file is" `ls sps2021data.* | tail -1`
  sleep 1
fi

