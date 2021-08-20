#source /cvmfs/atlas.cern.ch/repo/sw/tdaq/tools/cmake_tdaq/bin/cm_setup.sh tdaq-09-03-00
date >> /afs/cern.ch/user/i/ideadr/cernbox/TB2021_H8/sipmscript/acronjob.log

source /cvmfs/sw.hsf.org/key4hep/setup.sh
python /afs/cern.ch/user/i/ideadr/cernbox/TB2021_H8/sipmscript/convert.py
python /afs/cern.ch/user/i/ideadr/cernbox/TB2021_H8/sipmscript/align.py
