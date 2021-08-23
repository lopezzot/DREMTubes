#!/bin/bash -f

############################## hostname ##############################

echo "Executing dreamProc: $*" 

hname=`hostname`
if [ "$hname" != "pcpisadrc.cern.ch" ] && [ "$hname" != "pcdreamdaq" ]; then
  echo "wrong host $hname"
  sleep 3
  exit 1
fi

if [ "$hname" = "pcdreamdaq" ]; then
  cd ~/working
  if [ "$2" != "" ]; then
    export MAXEVT="$2"
  fi
  if [ "$1" == "clean" ]; then
    echo "cleaning DAQ"
    cleanDAQ
  elif [ "$1" == "stop" ]; then
    procr=`ps -aef | grep myReadOut | awk '{print $8}' | grep ^myReadOut | uniq`
    if [ "$procr" != "" ]; then
      echo "killing ${procr} with SIGINT"
      killall -s SIGINT ${procr}
    fi
  elif [ "$1" == "vme" ]; then
    which myReadOut
    mv logfile.txt logfile.sav
    myReadOut 2>&1 | tee logfile.txt
  elif [ "$1" == "osc" ]; then
    which myReadOutOsc
    mv logfile.txt logfile.sav
    myReadOutOsc 2>&1 | tee logfile.txt
  elif [ "$1" == "drs" ]; then
    which myReadOutDrs4
    mv logfile.txt logfile.sav
    myReadOutDrs4 2>&1 | tee logfile.txt
  elif [ "$1" == "all" ]; then
    which myReadOutAll
    mv logfile.txt logfile.sav
    myReadOutAll 2>&1 | tee logfile.txt
  elif [ "$1" == "data" ]; then
    which myDataWriter
    mv dtwrlog.txt dtwrlog.sav
    myDataWriter data 2>&1| tee dtwrlog.txt
  elif [ "$1" == "ped" ]; then
    which myDataWriter
    mv pdwrlog.txt pdwrlog.sav
    myDataWriter pedestal 2>&1| tee pdwrlog.txt
  elif [ "$1" == "mon" ]; then
    which sampler
    mv samplog.txt samplog.sav
    sampler 2>&1| tee samplog.txt
  else
    echo "wrong request for dream daq on ${hname}"
    exit 4
  fi
  sleep 3
fi
exit 0
