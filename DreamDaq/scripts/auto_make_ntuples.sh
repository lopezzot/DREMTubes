#!/bin/bash

export LD_LIBRARY_PATH=/home/dreamtest/lib
. ~/.bashrc

if [ -f ~/working/auto_make_ntuples.pid ]; then
    pid=`cat ~/working/auto_make_ntuples.pid`
    if kill -0 $pid; then
	echo "another auto_make_ntuples is alive, exiting..."
	exit
    fi
fi
echo $$ > ~/working/auto_make_ntuples.pid

export DATADIR=$HOME/storage
export NTUPLEDIR=$DATADIR/ntuple
export LOGDIR=$NTUPLEDIR/log

last_run=`grep RunNumber /home/dreamtest/working/.dreamdaqrc | awk '{print $2}'`

#last_reconstructed_run=`ls -rt /mnt/dreamix2/ntuple/|tail -1| egrep -o [0-9]+`
last_reconstructed_run=`cat ~/working/last_reconstructed_run.number`

run_stop=$((last_run-1))
run_start=$((last_reconstructed_run+1))

if [ "$run_stop" -gt "$run_start" ]; then
    date 
    echo "reconstructing run from $run_start to $run_stop"
    
    for ((run=$run_start; run<=$run_stop; run=$run+1)); do  
	echo "reconstructing ntuple $run"
	~/bin/myDaq2Ntu $run pedestal |& tee $LOGDIR/ped_$run.log &&
	~/bin/myDaq2Ntu $run data |& tee $LOGDIR/data_$run.log &&
	echo $run_stop > ~/working/last_reconstructed_run.number
    done
else
    date
    echo "no new run to reconstruct"
fi

rm ~/working/auto_make_ntuples.pid

