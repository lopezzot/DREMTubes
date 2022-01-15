import os, sys, subprocess
from glob import glob
import multiprocessing as mp
#from tqdm import tqdm


#rawdataPath = "/afs/cern.ch/user/i/ideadr/cernbox/TB2021_H8/rawData"
#rawntuplePath = "/afs/cern.ch/user/i/ideadr/cernbox/TB2021_H8/rawNtupleSiPM"

rawdataPath = "/eos/user/i/ideadr/TB2021_Desy/rawDataSiPM"
rawntuplePath = "/eos/user/i/ideadr/TB2021_Desy/rawNtupleSiPM"


def getFiles():
    files = glob(rawdataPath + "/*.dat")
    files = list(map(os.path.abspath, files))
    files = [f for f in files]
    #files = [f for f in files if os.path.getsize(f)/1024/1024 > 3]
    #print (files)
    return files


def moveConverted(fnames):
    toMove = glob(rawdataPath + "/Run*.root")
    for f in toMove:
        newfname = f.replace("rawDataSiPM","rawNtupleSiPM")
        os.rename(f,newfname)
    for f in fnames:
        p = subprocess.Popen(["bzip2", "-z", f])
        p.wait()


def runConversion(fname):
    print(fname)
    p = subprocess.Popen(["./../SIPM/converter/dataconverter", fname])
    p.wait()


def convertAll(fnames):
    # Run conversion .dat -> .root
    with mp.Pool(mp.cpu_count(), maxtasksperchild=4) as pool:
        list(
            #tqdm(
                pool.imap_unordered(runConversion, fnames),
                #total=len(fnames),
                #unit="file",
                #dynamic_ncols=True,
                #position=0,
                #colour="GREEN",
            #)
        )


if __name__ == "__main__":
    toConvert = getFiles()

    convertAll(toConvert)
    moveConverted(toConvert)
