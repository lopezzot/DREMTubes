#! /usr/bin/env python

import ROOT
from DR_metadata import DR_metadata

filename = "stocazzo_581_offset0.root"
metadata = DR_metadata()
DaqTreeName = metadata.DaqTreeName
SiPMTreeName = metadata.SiPMNewTreeName

ifile = ROOT.TFile(filename)
daq_tree = ifile.Get(DaqTreeName)
SiPM_tree = ifile.Get(SiPMTreeName)

daq_tree.AddFriend(SiPM_tree)

outfile = ROOT.TFile("output_581_offset0_firstHalf.root","recreate")

h_adc = ROOT.TH1F("h_adc","",1024,0,1024)
h_adc_totenele10 = ROOT.TH1F("h_adc_totenele10","",1024,0,1024)
h_totene = ROOT.TH1F("h_totene","",1000,0,5000) 
h_alignment = ROOT.TH2F("h_alignment","",1000,0,5000,1024,0,1024)
h_evtnumVsh_adc = ROOT.TH2F("h_evtnumVsh_adc","",10000,0,10000,1024,0,1024)

counter = 0
for ev in daq_tree:
    counter = counter + 1

    # compute total SiPM energy
    tot_ene = 0.
    for i in range(0,64):
        tot_ene = tot_ene + ev.HG_Board0[i]
        tot_ene = tot_ene + ev.HG_Board1[i]
        tot_ene = tot_ene + ev.HG_Board2[i]
        tot_ene = tot_ene + ev.HG_Board3[i]
        tot_ene = tot_ene + ev.HG_Board4[i]
    # plot energy in the SiPM against adc_muon


    h_alignment.Fill(tot_ene,ev.ADCs[32])
    h_adc.Fill(ev.ADCs[32])
    
    h_totene.Fill(tot_ene)

    
    
    if (tot_ene) < 10:
        h_adc_totenele10.Fill(ev.ADCs[32])
        h_evtnumVsh_adc.Fill(counter,ev.ADCs[32])
    else:
        h_evtnumVsh_adc.Fill(counter,1)


outfile.Write()
outfile.Close()





