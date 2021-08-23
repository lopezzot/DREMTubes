#!/bin/bash -f

runnr=77

echo "Saving log files ..."
WD=`pwd`
WORKDIR=/home/dreamtest/working
LOGFILE="run${runnr}.log.tgz"
cd ${WORKDIR}
tar czf ${LOGFILE} dtwrlog.txt logfile.log logfile.txt pdwrlog.txt samplog.txt
cp ${LOGFILE} /home/dreamtest/working/log/
cd ${WD}
echo "Logs file saved in ${LOGFILE}"

