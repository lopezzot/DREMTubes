#!/usr/bin/env python

import glob
import os

mrgpath = "/eos/user/i/ideadr/TB2021_H8/mergedNtuple/"
mrgfls = [x[len(x)-8:len(x)-5] for x in glob.glob(mrgpath+"*.root")]
recpath = "/afs/cern.ch/user/j/jagarwal/workspace/public/SPS2021/physicsNtuple/"
recfls = [x[len(x)-8:len(x)-5] for x in glob.glob(recpath+"*.root")]
mrgfls = list(set(mrgfls) - set(recfls))
phspath = "/afs/cern.ch/user/j/jagarwal/workspace/public/SPS2021/physicsNtuple/"

if mrgfls:
	print str(len(mrgfls))+" new files found"


for fl in mrgfls:
	cmnd1 = "root -l -b -q .x 'PhysicsConverter.C(\""+fl+"\")'"
	os.system(cmnd1)
	cmnd2 = "mv physics_sps2021_run"+fl+".root "+phspath
	os.system(cmnd2)

if not mrgfls:
	print "No new files found."
