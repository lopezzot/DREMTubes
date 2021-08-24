import glob
import os

mrgpath = "/eos/user/i/ideadr/TB2021_H8/mergedNtuple/"
mrgfls = [x[42+18:42+21] for x in glob.glob(mrgpath+"*.root")]
recpath = "/eos/user/i/ideadr/TB2021_H8/recoNtuple/"
recfls = [x[41+18:41+21] for x in glob.glob(recpath+"*.root")]
mrgfls = list(set(mrgfls) - set(recfls))
phspath = "/eos/user/i/ideadr/TB2021_H8/recoNtuple/"

if mrgfls:
	print str(len(mrgfls))+" new files found"


for fl in mrgfls:
	cmnd1 = "root -l -b -q .x 'PhysicsConverter.C(\""+fl+"\")'"
	os.system(cmnd1)
	cmnd2 = "mv physics_sps2021_run"+fl+".root "+phspath
	os.system(cmnd2)

if not mrgfls:
	print "No new files found."
