#! /usr/bin/env python

import ROOT
import os
from array import array
import numpy as np
import glob,time

####### Hard coded information - change as you want
SiPMFileDir="/afs/cern.ch/user/i/ideadr/scratch/TB2021_H8/rawNtupleSiPM"
DaqFileDir="/afs/cern.ch/user/i/ideadr/scratch/TB2021_H8/rawNtuple"
MergedFileDir="/afs/cern.ch/user/i/ideadr/scratch/TB2021_H8/mergedNtuple"
SiPMTreeName = "SiPMData"
SiPMMetaDataTreeName = "EventInfo"
DaqTreeName = "CERNSPS2021"
SiPMNewTreeName = "SiPMSPS2021"
EvtOffset = -1000
doNotMerge = False



####### main function to merge SiPM and PMT root files with names specified as arguments
    
def CreateBlendedFile(SiPMFileName,DaqFileName,outputfilename):
    SiPMinfile = None
    Daqinfile = None

    if not CheckFileNames(SiPMFileName,DaqFileName):
        print 'Problems, exiting......'
        return -1

    #open input.......

    SiPMinfile = ROOT.TFile.Open(SiPMFileName)
    Daqinfile = ROOT.TFile.Open(DaqFileName)

    #.... and output files
    
    OutputFile = ROOT.TFile.Open(outputfilename,"recreate")

    if not (SiPMMetaDataTreeName in SiPMinfile.GetListOfKeys()):
        print "Cannot find tree with name " + SiPMMetaDataTreeName + " in file " + SiPMinfile.GetName()
        return -1
    if not (DaqTreeName in Daqinfile.GetListOfKeys()):
        print "Cannot find tree with name " + DaqTreeName + " in file " + DaqInfile.GetName()
        return -1
    
    DaqInputTree = Daqinfile.Get(DaqTreeName)
    SiPMInputTree = SiPMinfile.Get(SiPMTreeName)

    ###### Do something to understand the offset

    global EvtOffset
    EvtOffset = DetermineOffset(SiPMInputTree,DaqInputTree)

    if doNotMerge:
        return 0 

    ###### Now really start to merge stuff
    
    newDaqInputTree = DaqInputTree.CloneTree()
    OutputFile.cd()
    newDaqInputTree.Write()
        
    EventInfoTree = SiPMinfile.Get(SiPMMetaDataTreeName)
    newEventInfoTree = EventInfoTree.CloneTree()
    OutputFile.cd()
    newEventInfoTree.Write()

    OutputFile.cd()
    newSiPMTree = ROOT.TTree(SiPMNewTreeName,"SiPM info")
        
    CloneSiPMTree(DaqInputTree,SiPMInputTree,OutputFile)
                  
    newSiPMTree.Write()
    OutputFile.Close()    
    return 0

# main function to reorder and merge the SiPM file

def CloneSiPMTree(DaqInputTree,SiPMInput,OutputFile):
    newTree = OutputFile.Get(SiPMNewTreeName)
    TriggerTimeStampUs = array('d',[0])
    EventNumber = array('i',[0])
    HG_Board = []
    LG_Board = []
    for i in range(0,5):
        HG_Board.append(np.array(64*[0],dtype=np.uint16))
        LG_Board.append(np.array(64*[0],dtype=np.uint16))
    HGinput = np.array(64*[0],dtype=np.uint16)
    LGinput = np.array(64*[0],dtype=np.uint16)

    newTree.Branch("TriggerTimeStampUs",TriggerTimeStampUs,'TriggerTimeStampUs/D')
    newTree.Branch("HG_Board0",HG_Board[0],"HG_Board0[64]/s")
    newTree.Branch("HG_Board1",HG_Board[1],"HG_Board1[64]/s")
    newTree.Branch("HG_Board2",HG_Board[2],"HG_Board2[64]/s")
    newTree.Branch("HG_Board3",HG_Board[3],"HG_Board3[64]/s")
    newTree.Branch("HG_Board4",HG_Board[4],"HG_Board4[64]/s")
    newTree.Branch("LG_Board0",LG_Board[0],"LG_Board0[64]/s")
    newTree.Branch("LG_Board1",LG_Board[1],"LG_Board1[64]/s")
    newTree.Branch("LG_Board2",LG_Board[2],"LG_Board2[64]/s")
    newTree.Branch("LG_Board3",LG_Board[3],"LG_Board3[64]/s")
    newTree.Branch("LG_Board4",LG_Board[4],"LG_Board4[64]/s")
    newTree.Branch("EventNumber",EventNumber,"EventNumber/s")
    SiPMInput.SetBranchAddress("HighGainADC",HGinput)
    SiPMInput.SetBranchAddress("LowGainADC",LGinput)

    '''The logic is the following: start with a loop on the Daq tree. For each event find out which entries of the SiPMInput need to be looked at (those with corresponding TriggerId). Once this information is available, copy the information of the boards and save the tree''' 

    entryDict = {}

    for ievt,evt in enumerate(SiPMInput):
        if evt.TriggerId in entryDict:
            entryDict[evt.TriggerId].append(ievt)
        else:
            entryDict[evt.TriggerId] = [ievt]
            
    totalNumberOfEvents = DaqInputTree.GetEntries()

    print "Total Number of Events from DAQ " + str(totalNumberOfEvents)
    print "Merging with an offset of " + str(EvtOffset)


    
    for daq_ev in range(0,totalNumberOfEvents):
        if (daq_ev%10000 == 0):
            print str(daq_ev) + " events processed"

        for iboard in range(0,5):
            HG_Board[iboard].fill(0)
            LG_Board[iboard].fill(0)

        evtToBeStored = []
        try:
            evtToBeStored = entryDict[daq_ev + EvtOffset]
        except:
            evtToBeStored = []

        for entryToBeStored in evtToBeStored:
            SiPMInput.GetEntry(entryToBeStored)
            #### Dirty trick to read an unsigned char from the ntuple
            myboard = map(ord,SiPMInput.BoardId)[0]
            np.copyto(HG_Board[myboard],HGinput)
            np.copyto(LG_Board[myboard],LGinput)
            TriggerTimeStampUs[0] = SiPMInput.TriggerTimeStampUs
        EventNumber[0] = daq_ev
        
        newTree.Fill()


def DetermineOffset(SiPMTree,DAQTree):
    x = array('i')
    y = array('i')
    xsipm = array('i')
    ysipm = array('i')
    ##### build a list of entries of pedestal events in the DAQ Tree
    DAQTree.SetBranchStatus("*",0)
    DAQTree.SetBranchStatus("TriggerMask",1)
    pedList = set()
    evList = set()
    for iev,ev in enumerate(DAQTree):
        if ev.TriggerMask == 6:
            pedList.add(iev)
        evList.add(iev)
    DAQTree.SetBranchStatus("*",1)
    ##### Now build a list of missing TriggerId in the SiPM tree
    SiPMTree.SetBranchStatus("*",0)
    SiPMTree.SetBranchStatus("TriggerId",1)
    TriggerIdList = set()
    for ev in SiPMTree:
        TriggerIdList.add(ev.TriggerId)
    SiPMTree.SetBranchStatus("*",1)
    ### Find the missing TriggerId
    TrigIdComplement = evList - TriggerIdList
    print "from PMT file: events "+str(len(evList))+" pedestals: "+str(len(pedList))
    print "from SiPM file: events with no trigger "+str(len(TrigIdComplement))
    #### do some diagnostic plot
    for p in pedList:
	    x.append(p)
	    y.append(2)
    for p2 in TrigIdComplement:
	    xsipm.append(p2)
	    ysipm.append(1)
    hist = ROOT.TH1I("histo","histo",100, 0, 100)
    for i in np.diff(sorted(list(pedList))):
	    hist.Fill(i)
    hist.Write()
    hist2 = ROOT.TH1I("histo2","histo2",100,0,100)
    for i in np.diff(sorted(list(TrigIdComplement))):
	    hist2.Fill(i)
    hist2.Write()
    graph = ROOT.TGraph(len(x),x,y)
    graph2 = ROOT.TGraph(len(xsipm),xsipm,ysipm)
    graph2.SetMarkerStyle(6)
    graph2.SetMarkerColor(ROOT.kRed)
    graph.SetMarkerStyle(6)
    graph.Write()
    graph2.Write()

    ### Scan possible offsets to find out for which one we get the best match between the pedList and the missing TriggerId

    minOffset = -1000
    minLen = 10000000
    
    diffLen = {}

    for offset in range(-4,5):
        offset_set = {x+offset for x in pedList}
        diffSet =  offset_set - TrigIdComplement
        diffLen[offset] = len(diffSet)
        if len(diffSet) < minLen:
            minLen = len(diffSet)
            minOffset = offset
        print "Offset " + str(offset) + ": " + str(diffLen[offset]) + " ped triggers where SiPM fired"
    print "Minimum value " + str(minLen) + " occurring for " + str(minOffset) + " offset"

    return minOffset

def CheckFileNames(SiPMFileName,DaqFileName):
    retval = True
    if not os.path.isfile(SiPMFileName):
        print 'Error! File ' + SiPMFileName + ' is not there'  
        retval = False
    if not os.path.isfile(DaqFileName):
        print 'Error! File ' + DaqFileName + ' is not there'  
        retval = False
    return retval 



def doRun(runnumber,outfilename):
    inputDaqFileName = DaqFileDir + "/sps2021data.run" + str(runnumber) + ".root"
    inputSiPMFileName = SiPMFileDir + "/Run" + str(runnumber) + "_list.root"
    return CreateBlendedFile(inputSiPMFileName,inputDaqFileName,outfilename)

def GetNewRuns():
    retval = []
    sim_list = glob.glob(SiPMFileDir + '/*')
    daq_list = glob.glob(DaqFileDir + '/*')
    merged_list = glob.glob(MergedFileDir + '/*')

    sim_run_list = []

    for filename in sim_list:
        sim_run_list.append(os.path.basename(filename).split('_')[0].lstrip('Run'))

    daq_run_list = [] 

    for filename in daq_list:
        daq_run_list.append(os.path.basename(filename).split('.')[1].lstrip('run'))

    already_merged = set()

    for filename in merged_list:
        already_merged.add(os.path.basename(filename).split('_')[2].split('.')[0].lstrip('run') )

    cand_tomerge = set()

    for runnum in sim_run_list:
        if (int(runnum) >= 644):
            if runnum in daq_run_list:
                cand_tomerge.add(runnum)
    tobemerged = cand_tomerge - already_merged
    return sorted(tobemerged)

    return retval 

###############################################################
        
def main():
    import argparse                                                                      
    parser = argparse.ArgumentParser(description='This script runs the merging of the "SiPM" and the "Daq" daq events. The option --newFiles shoudl be used only in TB mode, has the priority on anything else, and tries to guess which new files are there and merge them. \n Otherwise, the user needs to provide either --inputSiPM and --inputDaq, or --runNumber. --runNumber has priority if both sets of options are provided. --runNumber will assume the lxplus default test beam file location.')
    parser.add_argument('--inputSiPM', dest='inputSiPM',default='0',help='Input SiPM file')
    parser.add_argument('--inputDaq', dest='inputDaq',default='0',help='Input Daq file')
    parser.add_argument('--output', dest='outputFileName',default='SiPM_PMT_output.root',help='Output file name')
    parser.add_argument('--no_merge', dest='no_merge',action='store_true',help='Do not do the merging step')           
    parser.add_argument('--runNumber',dest='runNumber',default='0', help='Specify run number. The output file name will be merged_sps2021_run[runNumber].root ')
    parser.add_argument('--newFiles',dest='newFiles',action='store_true', default=False, help='Looks for new runs in ' + SiPMFileDir + ' and ' + DaqFileDir + ', and merges them. To be used ONLY from the ideadr account on lxplus')

    
    par  = parser.parse_args()
    global doNotMerge
    doNotMerge = par.no_merge

    if par.newFiles:
        ##### build runnumber list
        rn_list = GetNewRuns()
        for runNumber in rn_list:
            outfilename = MergedFileDir + '/merged_sps2021_run' + str(runNumber) + '.root'
            print '\n\nGoing to merge run ' + runNumber + ' and the output file will be ' + outfilename + '\n\n'  
            allgood = doRun(runNumber, outfilename)
        return 

    allGood = 0

    if par.runNumber != '0':
        print 'Looking for run number ' + par.runNumber
        outfilename = 'merged_sps2021_run' + str(par.runNumber) + '.root'
        allGood = doRun(par.runNumber,outfilename)
    else: 
        if par.inputSiPM != '0' and par.inputDaq != '0':
            print 'Running on files ' + par.inputSiPM + ' and ' +  par.inputDaq
            start = time.time()
            allGood = CreateBlendedFile(par.inputSiPM,par.inputDaq,par.outputFileName)
            end = time.time()
            print 'Execution time ' + str(end-start)
        else:
            print 'You need to provide either --inputSiPM and --inputDaq, or --runNumber. Exiting graciously.....'
            parser.print_help()
            return 

    if allGood != 0:
        print 'Something went wrong. Please double check your options. If you are absolutely sure that the script should have worked, contact iacopo.vivarelli AT cern.ch'

############################################################################################# 

if __name__ == "__main__":    
    main()
