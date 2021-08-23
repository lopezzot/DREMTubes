#!/bin/bash

cd /home/dreamtest/SPS.2021.08

Exec=myReadOutNoStop

daqPid=`ps -aef | awk '{print $2 " " $8}' | grep $Exec | awk '{print $1}'`
sleep 1

if [[ $daqPid != "" ]]; then
  echo "'$Exec' is running: PID is $daqPid"
  sleep 1
  echo "stopping process $daqPid"
  sleep 1
  kill -INT $daqPid
  echo "process $daqPid stopped"
  sleep 1

  DATE=`date +%Y.%m.%d`
  HOUR=`date +%H:%M`

  echo "'$Exec' stopped at " $DATE " " $HOUR
  echo "data file is" `ls sps2021data.* | tail -1`
  sleep 1

else
  DATE=`date +%Y.%m.%d`
  HOUR=`date +%H:%M`

  echo "'$Exec' is not running at " $DATE " " $HOUR
  echo "last data file is" `ls sps2021data.* | tail -1`
  sleep 1
fi

