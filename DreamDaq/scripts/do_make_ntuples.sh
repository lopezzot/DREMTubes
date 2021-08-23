#!/bin/bash

. ~/.bashrc

export DATADIR=/mnt/dreamix2
export NTUPLEDIR=$DATADIR/ntuple
export LOGDIR=$NTUPLEDIR/log

if [ $# -ne 2 ]; then
  echo "Usage $0 <first run> <last run>"
  exit 1
fi
run_start=$1
run_stop=$2

date >> $LOGDIR/log
echo "reconstructing run from $run_start to $run_stop" >> $LOGDIR/log

for ((run=$run_start; run<=$run_stop; run=$run+1)); do  ~/bin/myDaq2Ntu $run pedestal &> $LOGDIR/ped_$run.log; ~/bin/myDaq2Ntu $run data &> $LOGDIR/data_$run.log; done

