import os, sys, subprocess, uproot
from glob import glob
import multiprocessing as mp
from tqdm import tqdm
import numpy as np
from scipy.io import savemat


ntuplePath = "/afs/cern.ch/user/i/ideadr/cernbox/TB2021_H8/rawNtupleSiPM"


def getFiles():
    files = glob(ntuplePath + "/*list.root")
    files = list(map(os.path.abspath, files))
    return files


def getAligned():
    files = glob(ntuplePath + "/*.npz")
    files = list(map(os.path.abspath, files))
    return files


def getFilesToAlign():
    allFiles = getFiles()
    aligned = getAligned()
    allFiles = set([f.rsplit(".", 1)[0] for f in allFiles])
    aligned = set([f.rsplit(".", 1)[0] for f in aligned])
    toAlign = allFiles - aligned
    toAlign = [f + ".root" for f in toAlign]
    return list(toAlign)


def runAlignement(fname):
    # Load data
    with uproot.open(fname) as f:
        tid = np.array(f["SiPMData"]["TriggerId"], dtype=np.uint64)
        if tid.max() == 0:
            tqdm.write(f"Error in file {fname}. Skipping")
            return None
        hg = np.array(f["SiPMData"]["HighGainADC"], dtype=np.uint16)
        lg = np.array(f["SiPMData"]["LowGainADC"], dtype=np.uint16)
        bid = np.array(f["SiPMData"]["BoardId"], dtype=np.uint8)

    # Sort with respect to tid
    sortIdx = np.argsort(tid)
    hg = hg[sortIdx]
    lg = lg[sortIdx]
    bid = bid[sortIdx]
    tid = tid[sortIdx]
    tiduniq = np.unique(tid)

    nEvents = tiduniq.size

    hgMatrix = np.zeros((20, 16, nEvents), dtype=np.uint16)
    lgMatrix = np.zeros((20, 16, nEvents), dtype=np.uint16)

    # Align data
    for i, t in enumerate(
        tqdm(tiduniq, leave=False, dynamic_ncols=True, position=1, colour="RED", desc=fname.rsplit("/", 1)[-1])
    ):
        firstidx = np.searchsorted(tid, t)
        nBoards = np.count_nonzero(tid[firstidx : firstidx + 5] == t)
        boards = bid[firstidx : firstidx + nBoards]
        for j in range(nBoards):
            b = boards[j]
            hgMatrix[b * 4 : b * 4 + 4, :, i] = hg[firstidx + j].reshape(4, 16)
            lgMatrix[b * 4 : b * 4 + 4, :, i] = lg[firstidx + j].reshape(4, 16)
    np.savez_compressed(fname[:-5], hg=hgMatrix, lg=lgMatrix, tid=tiduniq)
    savemat(
        fname[:-5] + ".mat",
        {"matrixHighGainSiPM": hgMatrix, "matrixLowGainSiPM": lgMatrix, "triggerId": tiduniq},
        do_compression=True,
        oned_as="row",
    )


def alignAll(fnames):
    for f in tqdm(fnames, dynamic_ncols=True, position=0, colour="GREEN"):
        runAlignement(f)


if __name__ == "__main__":
    toAlign = getFilesToAlign()
    for file in toAlign:
        tqdm.write(file)

    alignAll(toAlign)
