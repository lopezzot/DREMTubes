#!/bin/bash

#adapted from http://mainstreetanswers.org/wvc54gc.php

TARGETDIR=/home/dreamtest/working/tablePositionScreenShots/
TMPDIR=/tmp/tablePos

RUNNBR=`cat /home/dreamtest/working/run.number | awk '{print $1}'`
DATE=`date +"%s"`
TAIL="run${RUNNBR}.${DATE}.jpg"
DELAY=0
if [ "$#" -eq "1" ];
  then DELAY=$1
fi

sleep $DELAY 

mplayer http://192.168.1.3/img/video.asf -cache 32 -nosound -frames 1 -vo jpeg:outdir=${TMPDIR} 1>/dev/null 2>/dev/null
FILE=`ls -1rt ${TMPDIR} | tail -1`
NAME="Hor.${TAIL}"
cp ${TMPDIR}/${FILE} ${TARGETDIR}/${NAME}
echo "Camera image saved in: ${TARGETDIR}/${NAME}"

mplayer http://192.168.1.2/img/video.asf -cache 32 -nosound -frames 1 -vo jpeg:outdir=${TMPDIR} 1>/dev/null 2>/dev/null
FILE=`ls -1rt ${TMPDIR} | tail -1`
NAME="Rot.${TAIL}"
cp ${TMPDIR}/${FILE} ${TARGETDIR}/${NAME}
echo "Camera image saved in: ${TARGETDIR}/${NAME}"

