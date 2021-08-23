#!/bin/bash

# ---------------------------------------- startdaq ----------------------------------------
if [ "$LD_LIBRARY_PATH" == "" ]; then
  export LD_LIBRARY_PATH=/home/dreamtest/lib
else
  export LD_LIBRARY_PATH=/home/dreamtest/lib:$LD_LIBRARY_PATH
fi

dreamdaqhost=pcdreamus
## dreamguihost=pcdreamdaq2
## to run in Pavia with cosmics - Roberto 12.05.2021
dreamguihost=pcdreamus
##dreameorsound=sounds/Reveille_on_bugle.ogg
xxxdreameorsound=sounds/Retreat.ogg
dreameorsound=/usr/lib64/libreoffice/share/gallery/sounds/romans.wav

xoff=0
yoff=10
dreamproc()
 {
  gnome-terminal --geometry=100x18+$xoff+$yoff --zoom=0.8 -e "sshDreamProc.sh $1 $2 $3 $4 $5; echo \"Exiting ....\" ; sleep 3" &
  yoff=`expr $yoff + 290`
 }

startdreamdaq()
 {
  run_number=$1
  daq_mode=`tolower $2`
  beamevtmx=$3
  phys2ped=$4

  mv /home/dreamtest/working/runguilog.txt /home/dreamtest/working/runguilog.sav

  echo "++++++++++++++++++++++ RUN ${run_number} ++++++++++++++++++++++"
  echo -e "\n\n`date` starting run ${run_number}"
  echo ${run_number} > /home/dreamtest/working/run.number

  echo "---> starting readout on $dreamdaqhost"
  echo "dreamproc $dreamdaqhost $daq_mode $run_number $beamevtmx $phys2ped"
  dreamproc $dreamdaqhost $daq_mode $run_number $beamevtmx $phys2ped
  sleep 2

  echo "---> starting data writer on $dreamdaqhost"
  echo "dreamproc $dreamdaqhost data $run_number"
  dreamproc $dreamdaqhost data $run_number
  sleep 2

  echo "---> starting pedestal writer on $dreamdaqhost"
  echo "dreamproc $dreamdaqhost ped $run_number"
  dreamproc $dreamdaqhost ped $run_number
  sleep 2

  echo "---> starting sampler on $dreamdaqhost"
  echo "dreamproc $dreamdaqhost mon $run_number"
  dreamproc $dreamdaqhost mon $run_number
 }

# ---------------------------------------- stopdaq ----------------------------------------

stopdreamdaq()
 {
  runnr=`grep RunNumber /home/dreamtest/working/.dreamdaqrc | awk '{print $2}'`
  echo "`date` stopping run ${runnr}"
  echo "sshDreamProc.sh $dreamdaqhost stop"
  sshDreamProc.sh $dreamdaqhost stop &
  echo ssh $dreamguihost " paplay $dreameorsound " &
  ssh $dreamguihost " paplay $dreameorsound " &
 }

# ---------------------------------------- cleandaq ----------------------------------------

cleandreamdaq()
 {
  echo "sshDreamProc.sh $dreamdaqhost clean"
  sshDreamProc.sh $dreamdaqhost clean &
 }

# ---------------------------------------- infodaq ----------------------------------------

waitforsummaries ()
 {
  while [ true ] ; do
    usleep 1000
    procs=`ps -aef | awk '{print $8}' | grep -e myDataWriter -e myReadOut -e sampler | grep -v grep`
    if [[ "$procs" == "" ]] ; then
      break
    fi
  done
 }

doofflinepedhisto ()
 {
  runnr=`grep RunNumber /home/dreamtest/working/.dreamdaqrc | awk '{print $2}'`
  ssh $dreamguihost "doOfflineHisto ${runnr} pedestal 2>&1 >/home/dreamtest/working/doPedHisto.txt &" &
  echo "Program 'doOfflineHisto ${runnr} pedestal' launched on $dreamguihost in backgroud"
 }

doofflinedatahisto ()
 {
  runnr=`grep RunNumber /home/dreamtest/working/.dreamdaqrc | awk '{print $2}'`
  doOfflineHisto ${runnr} data 2>&1 >/home/dreamtest/working/doDataHisto.txt &
  echo "Program 'doOfflineHisto ${runnr} data' launched in backgroud"
 }

printsummaries ()
 {
  nevts=`grep NrOfPhysicsEvents /home/dreamtest/working/.dreamdaqrc | awk '{print $2}'`
  runnr=`grep RunNumber /home/dreamtest/working/.dreamdaqrc | awk '{print $2}'`

  echo "*** RUN ${runnr} closed"
  echo -n -e "\n\n"

  lgfile="/home/dreamtest/working/logfile.txt"
  ntkn=`tail -3 "$lgfile" | grep "Total Events" | awk '{print $9}'`
  if [[ "$ntkn" != "" ]]; then
    if [[ "$ntkn" -lt "$nevts" ]] ; then
      echo -e "*** WARNING : run closed with $ntkn beam events (initial request was for $nevts) ***\n\n"
    fi
  fi

  if [ -s "$lgfile" ] ; then
    string=$(tail -3 "$lgfile" | grep "Total Events" | awk '{print "Total events: "$5}')
    echo $string
    string=$(tail -3 "$lgfile" | grep "Total Events" | awk '{print "Readout beam events: "$9}')
    echo $string
    string=$(tail -3 "$lgfile" | grep "Total Events" | awk '{print "Readout pedestal events: "$14}')
    echo $string
  else
    echo -e "\n\n*** ERROR : missing or empty readout logfile $lgfile ***\n\n"
  fi

  lgfile="/home/dreamtest/working/dtwrlog.txt"
  if [ -s "$lgfile" ] ; then
    a=$(grep Events "$lgfile" | awk '{print $5}')
    total=0
    for i in $a
    do
      total=$((total+i))
    done
    echo "Written beam events: $total"
  else
    echo -e "\n\n*** ERROR : missing or empty data writer logfile $lgfile ***\n\n"
  fi
  lgfile="/home/dreamtest/working/pdwrlog.txt"
  if [ -s "$lgfile" ] ; then
    a=$(grep Events "$lgfile" | awk '{print $5}')
    total=0
    for i in $a
    do
      total=$((total+i))
    done
    echo "Written pedestal events: $total"
  else
    echo -e "\n\n*** ERROR : missing or empty ped writer logfile $lgfile ***\n\n"
  fi

  lgfile="/home/dreamtest/working/samplog.txt"
  if [ -s "$lgfile" ] ; then
    string=$(tail -3 "$lgfile" | grep "sampled" | awk '{print "Sampled " $5 " events over " $8 " " $9}')
    if [ "$string" != "" ] ; then
      echo $string
    else
      echo -e "\n\n*** ERROR : Sampler didn't exit properly ***\n\n"
    fi
  else
    echo -e "\n\n*** ERROR : missing or empty sampler logfile $lgfile ***\n\n"
  fi

  echo -e "\n"

  # Saving log files
  WORKDIR=/home/dreamtest/working
  echo "Saving log files ..."
  echo "*************** run ${runnr} logging time: `date` ***************" >> ${WORKDIR}/ReadOutLogFile.log
  cat /home/dreamtest/working/logfile.txt >> ${WORKDIR}/ReadOutLogFile.log
  echo "logfile.txt appended to ${WORKDIR}/ReadOutLogFile.log"
  WD=`pwd`
  LOGFILE="run${runnr}.log.tgz"
  cd ${WORKDIR}
  echo "All logs being saved in ${LOGFILE}"
  echo "---------------------- RUN ${runnr} ----------------------"
  getinfo ${runnr} <daqguilog.txt >runguilog.txt
  tar czf ${LOGFILE} dtwrlog.txt logfile.txt pdwrlog.txt samplog.txt runguilog.txt
  mv ${LOGFILE} /home/dreamtest/working/log/
  cd ${WD}

 }

backupdata ()
 {
  BACKUPSCRIPT=/home/dreamtest/bin/backupDataFile
  if [ -a $BACKUPSCRIPT ]; then
    $BACKUPSCRIPT ${runnr} &
    echo "Program '$BACKUPSCRIPT' launched in backgroud"
  else
    echo -e "\033[1;31m[WARNING]\033[0m Backup script '$BACKUPSCRIPT' is missing "
  fi
}
  
makentuples ()
 {
  NTUPLEPRODUCTION=/home/dreamtest/bin/do_make_ntuples.sh
  if [ -a $NTUPLEPRODUCTION ]; then
    $NTUPLEPRODUCTION ${runnr} ${runnr} &
    echo "Program '$NTUPLEPRODUCTION' launched in backgroud"
  else
    echo -e "\033[1;31m[WARNING]\033[0m ntuple production script '$NTUPLEPRODUCTION' is missing "
  fi
 }

infodreamdaq()
 {
  waitforsummaries
  printsummaries
  # backupdata
  # doofflinepedhisto
  # makentuples
  # doofflinedatahisto
  # aplay -D default:CARD=TMX320VC5509 /home/dreamtest/working/sounds/endOfRun.wav /home/dreamtest/working/sounds/imperial.wav 2> /dev/null &  

 }

# ---------------------------------------- file sync ----------------------------------------

logsync()
 {
  echo "sshDreamProc.sh $dreamdaqhost sync"
  sshDreamProc.sh $dreamdaqhost sync &
 }

# ---------------------------------------- usage ----------------------------------------

usage()
 {
  echo "Usage: "
  echo "       $0 start <run_nr> <mode> <max_evt> <ped_ratio>"
  echo "       $0 stop"
  echo "       $0 clean"
  echo "       $0 log"
  echo "       $0 sync"
 }

# ---------------------------------------- main ----------------------------------------

if [ "$1" == "start" ]; then
  if [ "$5" != "" ]; then
    startdreamdaq $2 $3 $4 $5
  else
    usage
  fi
elif [ "$1" == "stop" ]; then
  stopdreamdaq
elif [ "$1" == "clean" ]; then
  cleandreamdaq
elif [ "$1" == "log" ]; then
  infodreamdaq
elif [ "$1" == "sync" ]; then
  logsync
else
  usage
fi

exit 0
