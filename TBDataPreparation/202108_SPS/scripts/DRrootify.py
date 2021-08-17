##**************************************************
## \file DRrootify.py
## \brief: converter from ASCII files to EventDR objects 
##         to ROOT files
## \author: Lorenzo Pezzotti (CERN EP-SFT-sim) @lopezzot
##          Andrea Negri (UniPV & INFN)
## \start date: 16 August 2021
##**************************************************

import DREvent
from ROOT import *
from array import array
import sys
import glob
import os

class DRrootify:
    '''Class to rootify raw ASCII files'''

    def __init__(self, fname):
        '''Class Constructor'''
        self.drfname = fname+".txt"
        self.drfile = TFile(fname+".root","RECREATE")
        self.tbtree = TTree("CERNSPS2021","CERNSPS2021")
        self.EventNumber = array('i',[0])
        self.NumOfPhysEv = array('i',[0])
        self.NumOfPedeEv = array('i',[0])
        self.NumOfSpilEv = array('i',[0])
        self.TriggerMask = array('i',[0])
        self.ADCs = array('i',[-1]*96)
        self.TDCsval = array('i',[-1]*48)
        self.TDCscheck = array('i',[-1]*48)

        self.tbtree.Branch("EventNumber",self.EventNumber,'EventNumber/I')
        self.tbtree.Branch("NumOfPhysEv",self.NumOfPhysEv,'NumOfPhysEv/I')
        self.tbtree.Branch("NumOfPedeEv",self.NumOfPedeEv,'NumOfPedeEv/I')
        self.tbtree.Branch("NumOfSpilEv",self.NumOfSpilEv,'NumOfSpilEv/I')
        self.tbtree.Branch("TriggerMask",self.TriggerMask,'TriggerMask/I')
        self.tbtree.Branch("ADCs",self.ADCs,'ADCs[96]/I')
        self.tbtree.Branch("TDCsval",self.TDCsval,'TDCsval[96]/I')
        self.tbtree.Branch("TDCscheck",self.TDCscheck,'TDCscheck[96]/I')

    def ReadandRoot(self):
        '''Read ASCII files line by line and rootify'''
        print "--->Start rootification of "+self.drfname
        for i, line in enumerate(open(self.drfname)):
            if i%5000 == 0 : print "->At line "+str(i)+" of "+str(self.drfname)
            evt = DREvent.DRdecode(line) 
            self.EventNumber[0] = evt.EventNumber
            self.NumOfPhysEv[0] = evt.NumOfPhysEv
            self.NumOfPedeEv[0] = evt.NumOfPedeEv
            self.NumOfSpilEv[0] = evt.NumOfSpilEv
            self.TriggerMask[0] = evt.TriggerMask
            for counter, l in enumerate(evt.ADCs.items()):
                self.ADCs[counter] = l[1]
            for counter, l in enumerate(evt.TDCs.items()):
                self.TDCsval[counter] = l[1][0]
                self.TDCscheck[counter] = l[1][1]
            self.tbtree.Fill()
        print "--->End rootification of "+self.drfname
    
    def Write(self):
        self.tbtree.Write()
        self.drfile.Close()

#Quick test me
#
#fname = "sps2021data.run493"
#dr = DRrootify(fname)
#dr.ReadandRoot()
#dr.Write()

#Get list of files on rawData and rawNtuple
#
#datafls = [x.split(".bz2")[0] for x in glob.glob("/eos/user/i/ideadr/TB2021_H8/rawData/")]
datafls = [x.split(".bz2")[0] for x in glob.glob("rawData/*.bz2")]
#ntuplfls = [x.split(".root")[0] for x in glob.glob("/eos/user/i/ideadr/TB2021_H8/rawNtuple/")]
ntuplfls = [x.split(".root")[0] for x in glob.glob("rawNtuple/*.root")]
newfls = list(set(datafls)-set(ntuplfls))

#Rootify those data
#
for fl in newfls:
    print "->Found new file to be rootified: "+str(fl)
    os.system("bzip2 -d -k "+str(fl)+".bz2")
    print "--->"+str(fl)+".bz2 decompressed"
    fname = fl[0:-4]
    dr = DRrootify(fname)
    dr.ReadandRoot()
    dr.Write()
    os.system("rm "+fl)
    os.system("mv "+str(fl[0:-4])+".root rawNtuple") 

##**************************************************
