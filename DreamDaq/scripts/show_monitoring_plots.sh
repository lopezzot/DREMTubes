#!/bin/bash
RUN=$1

if [ $# -ne 1 ]; then
  echo "Usage: $1 runnumber"
  exit
fi

echo "path: " `which doOfflineHisto`

NL=`ls -1 /home/dreamtest/working/hbook/*${RUN}* | wc -l`

if [ $NL == 0]; then
  echo "Executing doOfflineHisto"
  doOfflineHisto $RUN data
else
  echo "Root file already present:"
  ls -l /home/dreamtest/working/hbook/*${RUN}*
  echo "Run 'doOfflineHisto' manually if you want to regenerate the root file"
fi


root -l /home/dreamtest/bin/show_monitoring_plots.cxx"($RUN, \"$*\")"


#doOfflineHisto $RUN data && root -l /home/dreamtest/bin/show_monitoring_plots.cxx"($RUN, \"$*\")"
