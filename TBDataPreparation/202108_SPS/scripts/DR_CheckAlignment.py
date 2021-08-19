#! /usr/bin/env python

import ROOT
from DR_metadata import DR_metadata

filename = "merged_run631.root"
metadata = DR_metadata()
DaqTreeName = metadata.DaqTreeName
SiPMTreeName = metadata.SiPMNewTreeName

ifile = ROOT.TFile(filename)
daq_tree = ifile.Get(DaqTreeName)
SiPM_tree = ifile.Get(SiPMTreeName)

daq_tree.AddFriend(SiPM_tree)

outfile = ROOT.TFile("output_631_offset0.root","recreate")

h_adc = ROOT.TH1F("h_adc","",1024,0,1024)
h_adc_totenele10 = ROOT.TH1F("h_adc_totenele10","",1024,0,1024)
h_totene_SiPM = ROOT.TH1F("h_totene_SiPM","",1000,0.,100000.)
h_totene_PMT = ROOT.TH1F("h_totene_PMT","",1000,0.,10000.) 
h_alignment = ROOT.TH2F("h_alignment","",1000,0,100000,1024,0,1024)
h_evtnumVsh_adc = ROOT.TH2F("h_evtnumVsh_adc","",10000,0,10000,1024,0,1024)
h_enePMTVseneSiPM = ROOT.TH2F("h_enePMTVseneSiPM","",100,0,10000,100,0,100000)


for ev in daq_tree:

    # compute total SiPM energy
    tot_ene_SiPM = 0.
    tot_ene_PMT = 0.
    
    for i in range(0,64):
        tot_ene_SiPM = tot_ene_SiPM + ev.HG_Board0[i]
        tot_ene_SiPM = tot_ene_SiPM + ev.HG_Board1[i]
        tot_ene_SiPM = tot_ene_SiPM + ev.HG_Board2[i]
        tot_ene_SiPM = tot_ene_SiPM + ev.HG_Board3[i]
        tot_ene_SiPM = tot_ene_SiPM + ev.HG_Board4[i]
    # plot energy in the SiPM against adc_muon

    
    
    # compute total PMT energy (scin + cher)

    for i in range(0,16):
        tot_ene_PMT = tot_ene_PMT + ev.ADCs[i]


    h_alignment.Fill(tot_ene_SiPM,ev.ADCs[32])
    h_adc.Fill(ev.ADCs[32])
    
    h_totene_SiPM.Fill(tot_ene_SiPM)
    h_totene_PMT.Fill(tot_ene_PMT)

    h_enePMTVseneSiPM.Fill(tot_ene_PMT, tot_ene_SiPM)    
    
    if (tot_ene_SiPM) < 10:
        h_adc_totenele10.Fill(ev.ADCs[32])


outfile.Write()
outfile.Close()





