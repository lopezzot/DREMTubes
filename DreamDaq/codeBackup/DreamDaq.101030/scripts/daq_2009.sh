#!/bin/bash -f

hname=`hostname`
if [[ "$hname" != "pcdream.cern.ch" ]]; then
  echo "wrong host: \"$hname\" --> please run \"daq\" from pcdream"
  echo "usage: \"daq [clean|stop|vme|drs|osc|all] [# of evt.s]\""
  exit 1
fi

usage()
 {
  echo "usage: daq [clean|stop|vme|drs|osc|all] [# of evt.s]"
 }

if [ "$1" == "clean" ] ; then
  echo "ssh dreamtest@pcrome2 dreamProc.sh clean"
  ssh dreamtest@pcrome2 "dreamProc.sh clean" &
  echo "ssh dreamtest@pcpisadrc dreamProc.sh clean"
  ssh dreamtest@pcpisadrc "dreamProc.sh clean" &
  for job in `jobs -p` ; do
    echo $job
    wait $job
  done
  exit 0
fi

if [ "$1" == "stop" ] ; then
  echo "ssh dreamtest@pcrome2 dreamProc.sh stop"
  ssh dreamtest@pcrome2 "dreamProc.sh stop"
  sleep 3
  exit 0
fi

if [ "$1" != "" ] ; then
  opt=`tolower $1`
elif [ "$DREAMDAQ" != "" ] ; then
  opt=`tolower $DREAMDAQ`
else
  opt=vme
fi

if [ "$opt" != "vme" ] && [ "$opt" != "osc" ] && [ "$opt" != "drs" ] && [ "$opt" != "all" ]; then
  echo "wrong call: $*"
  usage
  exit 1
fi

if [ "$2" != "" ] ; then
  nevts="$2"
elif [ "$MAXEVT" != "" ] ; then
  nevts="$MAXEVT"
else
  nevts=20000
fi

waitforsummaries ()
 {
  while [ true ] ; do
    usleep 5000
    procs=`ps -aef | grep "ssh dreamtest@pc" | grep -v grep`
    if [[ "$procs" == "" ]] ; then
      break
    fi
  done
 }

printsummaries ()
 {
  echo "*** RUN ${runnr} closed"
  echo -n -e "\n\n"

  ntkn=`tail -6 /home/dreamtest/working/logfile.txt | grep Events | awk '{print $2}'`
  if [ "$ntkn" -lt "$nevts"  ] ; then
    echo -e "*** WARNING : run closed with $ntkn events (initial request was for $nevts) ***\n\n"
  fi

  lgfile="/home/dreamtest/working/logfile.txt"
  if [ -s "$lgfile" ] ; then
    string=$(tail -6 "$lgfile" | grep Events | awk '{print "Readout beam events: "$2}')
    echo $string
    string=$(tail -6 "$lgfile" | grep Pedestal | awk '{print "Readout pedestal events: "$3}')
    echo $string
  else
    echo -e "\n\n*** ERROR : missing or empty readout logfile $lgfile ***\n\n"
  fi

  lgfile="/home/dreamtest/working/dtwrlog.txt"
  if [ -s "$lgfile" ] ; then
    string=$(grep Events "$lgfile" | awk '{print "Written beam events: "$2}')
    echo $string
  else
    echo -e "\n\n*** ERROR : missing or empty data writer logfile $lgfile ***\n\n"
  fi
  lgfile="/home/dreamtest/working/pdwrlog.txt"
  if [ -s "$lgfile" ] ; then
    string=$(grep Events "$lgfile" | awk '{print "Written pedestal events: "$2}')
    echo $string
  else
    echo -e "\n\n*** ERROR : missing or empty ped writer logfile $lgfile ***\n\n"
  fi

  lgfile="/home/dreamtest/working/samplog.txt"
  if [ -s "$lgfile" ] ; then
    string=$(grep Sampled "$lgfile")
    if [ "$string" != "" ] ; then
      echo $string
    else
      echo -e "\n\n*** ERROR : Sampler didn't exit properly ***\n\n"
    fi
    string=$(grep mean "$lgfile" | tail -1 | awk '{print "cerenkov: " $5 " scintillation: " $10}')
    if [ "$string" != "" ] ; then
      echo $string
    else
      echo -e "\n\n*** ERROR : DAQ stopped in advance ***\n\n"
    fi
  else
    echo -e "\n\n*** ERROR : missing or empty sampler logfile $lgfile ***\n\n"
  fi

  echo -e "\n"

 }

f=0
exithandler ()
 {
  echo "CTRL+C detected"
  echo ssh dreamtest@pcrome2 \"dreamProc.sh stop\"
  ssh dreamtest@pcrome2 "dreamProc.sh stop"
  if [ "$f" -eq "0" ] ; then
    f=1
    echo "waiting all processes exited"
    waitforsummaries
    printsummaries
    exit 0
  fi
  for job in `jobs -p` ; do
    echo $job
    wait $job
  done
  exit 0
 }

cleanuphandler ()
 {
  echo "CTRL+4 or CTRL+\ detected"
  echo "ssh dreamtest@pcrome2 dreamProc.sh clean"
  ssh dreamtest@pcrome2 "dreamProc.sh clean" &
  echo "ssh dreamtest@pcpisadrc dreamProc.sh clean"
  ssh dreamtest@pcpisadrc "dreamProc.sh clean" &
  for job in `jobs -p` ; do
    echo $job
    wait $job
  done
  exit 0
 }

xoff=0
yoff=10
dreamproc()
 {
  ###echo ssh dreamtest@$1 \"dreamProc.sh $2 $3\"
  gnome-terminal --geometry=80x12+$xoff+$yoff --zoom=0.7 -e "ssh dreamtest@$1 \"dreamProc.sh $2 $3\"" &
  yoff=`expr $yoff + 160`
 }

while [ true ] ; do
  echo -n -e "\n\n"
  echo -n "*** Start DAQ for $nevts events with $opt readout [Y/n]: "
  read a
  if [[ $a == "N" || $a == "n" ]]; then
    echo "Exiting now ..."
    exit 0
  elif [[ $a == "Y" || $a == "y" || $a = "" ]]; then
    echo "Ok, starting now ..."
    break
  fi
done

echo "---> starting readout on pcrome2"
dreamproc pcrome2 $opt $nevts
sleep 2

if [ "$opt" = "drs" ] || [ "$opt" = "all" ] ; then
  echo "---> starting drs on pcpisadrc"
  dreamproc pcpisadrc drs
fi
sleep 2

echo "---> starting data writer on pcrome2"
dreamproc pcrome2 data
sleep 2

echo "---> starting pedestal writer on pcrome2"
dreamproc pcrome2 ped
sleep 2

echo "---> starting sampler on pcrome2"
dreamproc pcrome2 mon
sleep 2

trap exithandler SIGINT
trap cleanuphandler SIGQUIT

while read LINE; do runnr=$LINE ; done < /home/dreamtest/working/run.number

echo -n -e "\n\n"
echo "*** CTRL+C to exit"
echo "*** CTRL+4 or CTRL+\ to clean up"
echo "*** RUN NUMBER IS ${runnr} started on "$(date)

waitforsummaries

printsummaries

exit 0

