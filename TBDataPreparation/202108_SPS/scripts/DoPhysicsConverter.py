import glob
import os,subprocess
import multiprocessing

mrgpath = "/eos/user/i/ideadr/TB2021_H8/mergedNtuple/"
mrgfls = [x[42+18:42+21] for x in glob.glob(mrgpath+"*.root")]
recpath = "/eos/user/i/ideadr/TB2021_H8/recoNtuple/"
recfls = [x[41+18:41+21] for x in glob.glob(recpath+"*.root")]
mrgfls = list(set(mrgfls) - set(recfls))
phspath = "/eos/user/i/ideadr/TB2021_H8/recoNtuple/"

def doConversion(fname):
    p1 = subprocess.Popen(["root","-l","-b","-q",".x","'PhysicsConverter.C("+fname+")'"])
    p1.wait()
    p2 = subprocess.Popen(["mv","physics_sps2021_run"+fname+".root",phspath])
    p2.wait()

if mrgfls:
    print(str(len(mrgfls))+" new files found")
    with multiprocessing.Pool(4) as pool:
        pool.map_async(doConversion,mrgfls)

if not mrgfls:
    print("No new files found.")
