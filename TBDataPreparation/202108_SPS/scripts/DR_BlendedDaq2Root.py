#! /usr/bin/env python

import ROOT
import os
from array import array

####### Hard coded information - change as you want
SiPMTreeName = "SiPMData"
SiPMMetaDataTreeName = "EventInfo"
DaqTreeName = "CERNSPS2021"
SiPMNewTreeName = "SiPMSPS2021"
verbose = False
EvtOffset = -1000
doNotMerge = False

def main():
    import argparse                                                                      
    parser = argparse.ArgumentParser(description='This script runs the merging of the "SiPM" and the "PMT" daq events')
    parser.add_argument('--inputSiPM', dest='inputSiPM',required=True,help='Input SiPM file')
    parser.add_argument('--inputPMT', dest='inputPMT',required=True,help='Input PMT file')
    parser.add_argument('--output', dest='outputFileName',default='SiPM_PMT_output.root',help='Output file name')
    parser.add_argument('--no_merge', dest='no_merge',action='store_true',help='Do not do the merging step')           
    parser.add_argument('--verbose',dest='verbose',action='store_true',help='Increase verbosity')
    par  = parser.parse_args()
    global verbose
    verbose = par.verbose
    global doNotMerge
    doNotMerge = par.no_merge
    CreateBlendedFile(par.inputSiPM,par.inputPMT,par.outputFileName)


####### main function to merge SiPM and PMT root files with names specified as arguments
    
def CreateBlendedFile(SiPMFileName,DaqFileName,outputfilename):
    SiPMinfile = None
    Daqinfile = None
    try:
        SiPMinfile = ROOT.TFile.Open(SiPMFileName)
        SiPMinfile.IsOpen()
    except:
        print "File " + SiPMFileName + " not found. Exiting....."
        return -1

    try:
        Daqinfile = ROOT.TFile.Open(DaqFileName)
        Daqinfile.IsOpen()
    except:
        print "File " + DaqFileName + " not found. Exiting....."
        return -1


    OutputFile = ROOT.TFile.Open(outputfilename,"recreate")

    ##### Start by copying everything which is not controversial

    if not (SiPMMetaDataTreeName in SiPMinfile.GetListOfKeys()):
        print "Cannot find tree with name " + SiPMMetaDataTreeName + " in file " + SiPMinfile.GetName()

    ### Check the alignment



    if not (DaqTreeName in Daqinfile.GetListOfKeys()):
        print "Cannot find tree with name " + DaqTreeName + " in file " + DaqInfile.GetName()
        return -1
    
    DaqInputTree = Daqinfile.Get(DaqTreeName)
    SiPMInputTree = SiPMinfile.Get(SiPMTreeName)

    ###### Do something to understand the offset

    global EvtOffset
    EvtOffset = DetermineOffset(SiPMInputTree,DaqInputTree)

    if doNotMerge:
        return 
    
    newDaqInputTree = DaqInputTree.CloneTree()
    OutputFile.cd()
    newDaqInputTree.Write()
        
    EventInfoTree = SiPMinfile.Get(SiPMMetaDataTreeName)
    newEventInfoTree = EventInfoTree.CloneTree()
    OutputFile.cd()
    newEventInfoTree.Write()

    ##### This does not work at the moment, we'll then understand why - not the highest priority
    
#    SiPMinfile.cd("Histograms")
#    oldDir = ROOT.gDirectory
#    print oldDir
#    oldDir.ReadAll()
#    OutputFile.cd()
#    OutputFile.mkdir("Histograms")
#    OutputFile.cd("Histograms")
#    oldDir.GetList().Write()
        
    OutputFile.cd()
    newSiPMTree = ROOT.TTree(SiPMNewTreeName,"SiPM info")
        
    CloneSiPMTree(DaqInputTree,SiPMInputTree,OutputFile)
                  
    newSiPMTree.Write()
    OutputFile.Close()    


# main function to reorder and merge the SiPM file

def CloneSiPMTree(DaqInputTree,SiPMInput,OutputFile):
    newTree = OutputFile.Get(SiPMNewTreeName)
    TriggerTimeStampUs = array('d',[0])
    EventNumber = array('i',[0])
    HG_Board = []
    LG_Board = []
    for i in range(0,5):
        HG_Board.append(array('i',[-1]*64))
        LG_Board.append(array('i',[-1]*64))
    newTree.Branch("TriggerTimeStampUs",TriggerTimeStampUs,'TriggerTimeStampUs/D')
    newTree.Branch("HG_Board0",HG_Board[0],"HG_Board0[64]/I")
    newTree.Branch("HG_Board1",HG_Board[1],"HG_Board1[64]/I")
    newTree.Branch("HG_Board2",HG_Board[2],"HG_Board2[64]/I")
    newTree.Branch("HG_Board3",HG_Board[3],"HG_Board3[64]/I")
    newTree.Branch("HG_Board4",HG_Board[4],"HG_Board4[64]/I")
    newTree.Branch("LG_Board0",LG_Board[0],"LG_Board0[64]/I")
    newTree.Branch("LG_Board1",LG_Board[1],"LG_Board1[64]/I")
    newTree.Branch("LG_Board2",LG_Board[2],"LG_Board2[64]/I")
    newTree.Branch("LG_Board3",LG_Board[3],"LG_Board3[64]/I")
    newTree.Branch("LG_Board4",LG_Board[4],"LG_Board4[64]/I")
    newTree.Branch("EventNumber",EventNumber,"EventNumber/I")

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
    print "Is the output Verbose? " + str(verbose)

    for daq_ev in range(0,totalNumberOfEvents):
        if verbose:
            if (daq_ev%1000 == 0):
                print str(daq_ev) + " events processed"
        TriggerTimeStampUs 
        for iboard in range(0,5):
            for ch in range(0,64):
                HG_Board[iboard][ch] = 0
                LG_Board[iboard][ch] = 0
        if (daq_ev + EvtOffset) in entryDict:
            for entryToBeStored in entryDict[daq_ev + EvtOffset]:
                SiPMInput.GetEntry(entryToBeStored)
                #### Dirty trick to read an unsigned char from the ntuple
                myboard = map(ord,SiPMInput.BoardId)[0]
                for ch in range(0,64):
                    HG_Board[myboard][ch] = SiPMInput.HighGainADC[ch]
                    LG_Board[myboard][ch] = SiPMInput.LowGainADC[ch]
            TriggerTimeStampUs = SiPMInput.TriggerTimeStampUs
        EventNumber = daq_ev
        
        newTree.Fill()


def DetermineOffset(SiPMTree,DAQTree):
    ##### build a list of entries of pedestal events in the DAQ Tree
    DAQTree.SetBranchStatus("*",0)
    DAQTree.SetBranchStatus("TriggerMask",1)
    pedList = set()
    evList = set()
    for iev,ev in enumerate(DAQTree):
        if ev.TriggerMask == 6:
            pedList.add(iev)
        evList.add(iev)
#    print pedList
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



        


        
    


if __name__ == "__main__":    
    main()
