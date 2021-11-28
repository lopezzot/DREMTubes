import glob
import os

mrgpath = "/eos/user/i/ideadr/TB2021_Desy/mergedNtuple/"
mrgfls = [x[45+18:45+21] for x in glob.glob(mrgpath+"*.root")]
recpath = "/eos/user/i/ideadr/TB2021_Desy/recoNtuple/"
recfls = [x[44+18:44+21] for x in glob.glob(recpath+"*.root")]
mrgfls = list(set(mrgfls) - set(recfls))
phspath = "/eos/user/i/ideadr/TB2021_Desy/recoNtuple/"

if mrgfls:
	print str(len(mrgfls))+" new files found"


for fl in mrgfls:
	cmnd1 = "root -l -b -q .x 'PhysicsConverter.C(\""+fl+"\")'"
	os.system(cmnd1)
	cmnd2 = "mv physics_desy2021_run"+fl+".root "+phspath
	os.system(cmnd2)

if not mrgfls:
	print "No new files found."
