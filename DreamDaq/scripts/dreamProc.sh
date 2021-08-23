#!/bin/bash

#to debug link stdout and stdin to a logfile
if [ $# -gt 1000 ]; then
  logfile=/home/dreamtest/working/$1_$2.log
  exec 3>&1 4>&2 1> >(tee >(cat - >${logfile}) >&3) 2> >(tee >(cat - >${logfile}) >&4)
fi

syncnfs()
 {
  while [ true ] ; do
    usleep 40000
    procr=`ps -aef | grep myReadOut | awk '{print $8}' | grep ^myReadOut | uniq`
    if [[ "$procr" == "" ]] ; then
      break
    else
      sync
    fi
  done
 }

############################## hostname ##############################

echo "Executing dreamProc: $*" 
echo "LD_LIBRARY_PATH is $LD_LIBRARY_PATH"
echo "PATH is $PATH"

USAGE="Usage: $0 <command> [args]"

if [ "$#" == "0" ]; then
	echo "$USAGE"
	sleep 2
	exit 1
fi

hname=`hostname -s`
if [ "$hname" != "pcdreamus" ]; then
	echo "wrong host $hname"
	sleep 2
	exit 2
fi

export STORAGEDIR=/home/dreamtest/storage
export WORKDIR=/home/dreamtest/working
export HISTODIR=/home/dreamtest/storage/hbook

if [ "$hname" == "pcdreamus" ]; then
  export THRESHOLD_SHORT=10
  export THRESHOLD_LONG=10
  cd ~/working
  dreamcommand=$1
  runnumber=$2
  if [ "$3" != "" ]; then
    export MAXEVT="$3"
  fi
  if [ "$4" != "" ]; then
    export PHYSPEDRATIO="$4"
  fi
  if [ "$dreamcommand" == "clean" ]; then
    echo "cleaning DAQ"
    cleanDAQ
  elif [ "$dreamcommand" == "stop" ]; then
    procr=`ps -aef | grep myReadOut | awk '{print $8}' | grep ^myReadOut | uniq`
    if [ "$procr" != "" ]; then
      echo "killing ${procr} with SIGINT"
      killall -s SIGINT ${procr}
    fi
  elif [ "$dreamcommand" == "vme" ]; then
    setV814
    which myReadOut
    mv logfile.txt logfile.sav
    myReadOut -runnr $runnumber 2>&1 | tee logfile.txt
  elif [ "$dreamcommand" == "drs" ] || [ "$dreamcommand" == "all" ]; then
    setV814
    which myReadOutDrs4
    mv logfile.txt logfile.sav
    myReadOutDrs4 -runnr $runnumber 2>&1 | tee logfile.txt
  elif [ "$dreamcommand" == "data" ]; then
    which myDataWriter
    mv dtwrlog.txt dtwrlog.sav
    myDataWriter data -runnr $runnumber 2>&1| tee dtwrlog.txt
  elif [ "$dreamcommand" == "ped" ]; then
    which myDataWriter
    mv pdwrlog.txt pdwrlog.sav
    myDataWriter pedestal -runnr $runnumber 2>&1| tee pdwrlog.txt
  elif [ "$dreamcommand" == "mon" ]; then
    which sampler
    mv samplog.txt samplog.sav
    sampler -runnr $runnumber 2>&1| tee samplog.txt
  elif [ "$dreamcommand" == "sync" ]; then
    syncnfs
  else
    echo "wrong request for dream daq on ${hname}"
    exit 3
  fi
  sleep 3
fi
exit 0
