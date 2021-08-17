#!/usr/bin/env python

import DREvent
import sys
import ROOT
import getopt
import glob
import os

PathToData='/home/dreamtest/SPS.2021.08/'

class DrMon:
  '''Data monitoring class for DualReadout 2021 Test Beam @H8  '''

  def __init__(self, fname, maxEvts, sample):
    '''Constructor '''
    self.fname    = fname
    self.maxEvts  = maxEvts
    self.sample   = sample
    self.hDict    = {}

  def bookAdcHistos(self, bins):
    '''Book ADC histograms '''
    for i in range(96):
      hname  = "adc-%02d" % i
      htitle = hname
      if i==32: htitle = htitle + " muon tracker"
      if i==64: htitle = htitle + " cherenkov 1"
      if i==65: htitle = htitle + " cherenkov 2"
      self.hDict[hname] = ROOT.TH1I(hname, htitle, bins, 0, 4095)      
    print "Booked ADCs histograms"

  def bookTdcHistos(self, bins):
    '''Book TDC histograms '''
    for i in range(16):
      hname = "tdc-%02d" % i
      htitle = hname
      if i==0 or i==4: htitle = htitle + " DWC" + str(i/4+1)+ " left"
      if i==1 or i==5: htitle = htitle + " DWC" + str(i/4+1)+ " right"
      if i==2 or i==6: htitle = htitle + " DWC" + str(i/4+1)+ " up"
      if i==3 or i==7: htitle = htitle + " DWC" + str(i/4+1)+ " down"
      self.hDict[hname] = ROOT.TH1I(hname, htitle, bins, 0, 4095)      
    self.hDict["tdc-sz"] = ROOT.TH1I("tdc-sz", "TDC Size", 17, -0.5, 16.5) 

    print "Booked TDCs histograms:"

  def bookDwcHistos(self, bins):
    '''Book DWC histograms '''
    for i in range(1, 3):
      dwc="dwc%d" % i
      hname = dwc + "l-r"
      self.hDict[hname] = ROOT.TH1I(hname, hname,  bins, -2048, 2048)
      
      hname = dwc + "u-d"
      self.hDict[hname] = ROOT.TH1I(hname, hname,  bins, -2048, 2048)
     
      bins=50
      hname = dwc + "l/r"
      self.hDict[hname] = ROOT.TH2I(hname, hname, bins, 0, 4095, bins, 0, 4095)

      hname = dwc + "u/d"
      self.hDict[hname] = ROOT.TH2I(hname, hname, bins, 0, 4095, bins, 0, 4095) 

      hname = dwc + "X/Y"
      self.hDict[hname] = ROOT.TH2I(hname, hname, bins, 0, 4095, bins, 0, 4095) 

    # Set fill color
    for h in self.hDict.values():
      h.SetFillColor(42)

    print "Booked DWCs histograms"


  def hFill(self, event):
    '''Fill the histogram '''
    # ADC
    for ch, val in event.ADCs.items():
      hname  = "adc-%02d" % ch
      self.hDict[hname].Fill(val)
    
    # TDC
    for ch, val in event.TDCs.items():
      hname  = "tdc-%02d" % ch
      self.hDict[hname].Fill(val[0])
 
    # DWC
    dwc1X = dwc1Y = -9999
    dwc2X = dwc2Y = -9999
    if 0 in event.TDCs and 1 in event.TDCs:       # DWC 1: left and right
      vA, vB = event.TDCs[0][0], event.TDCs[1][0]
      self.hDict['dwc1l-r'].Fill( vA - vB)
      self.hDict['dwc1l/r'].Fill( vA , vB)
      dwc1X = vA - vB
    
    if 2 in event.TDCs and 3 in event.TDCs:       # DWC 1: up and down
      vA, vB = event.TDCs[2][0], event.TDCs[3][0]
      self.hDict['dwc1u-d'].Fill( vA - vB)
      self.hDict['dwc1u/d'].Fill( vA , vB)
      dwc1Y = vA - vB
    
    if 4 in event.TDCs and 5 in event.TDCs:       # DWC 2: left and right
      vA, vB = event.TDCs[4][0], event.TDCs[5][0]
      self.hDict['dwc2l-r'].Fill( vA - vB)
      self.hDict['dwc2l/r'].Fill( vA , vB)
      dwc2X = vA - vB
    
    if 6 in event.TDCs and 7 in event.TDCs:       # DWC 1: up and down
      vA, vB = event.TDCs[6][0], event.TDCs[7][0]
      self.hDict['dwc2u-d'].Fill( vA - vB)
      self.hDict['dwc2u/d'].Fill( vA , vB)
      dwc2Y = vA - vB
  
 

  def readFile(self):
    '''Read raw ascii data from file, call the decoding function, fill the histograms'''
    print "Read and parse"
    step=1
    for i, line in enumerate(open(fname)):
      if i%self.sample==0:
        ev = DREvent.DRdecode(line) 
        if i==0:
          print ev.headLine()
        if i%step==0: print ev
        if i>step*10: step=step*10
        self.hFill(ev)
      if i >= self.maxEvts: break
    
  def message(self):
    msg = """
    Available histograms:
       * a 0-96:  ADC channels
           * 32: muon tracker
           * 64: cherenkov 1
           * 65: cherenkov 2
       * t 0-16:  TCD channels 
           * 0-3   DWC-1
           * 4-7   DWC-2
           * 8-15  PMT
           * 16    TDC entries
       * d 0-5:   DWC differences
           * 0: DWC1: up - down
           * 1: DWC1: left - right
           * 2: DWC2: up - down
           * 3: DWC2: left - right
           * 4: DWC1: up vs down
           * 5: DWC1: left vs right
           * 6: DWC2: up vs down
           * 7: DWC2: left vs right
           * 8: DWC1: X vs Y
           * 9: DWC2: X vs Y
    Available commands:
           * l 1/0: set/unset logy scale
    """
    print msg



  def commander(self):
    '''Interactive commander '''
    c=ROOT.TCanvas('c1','IDEA-DR',0,0,800,600)
    while True:
      print sorted(self.hDict.keys())
      line = raw_input('Show histogram (a/t/d chNum) --> ')
      pars = line.split()
      cmd = pars[0]
      opt = 0
      if len(pars) > 1:
        opt = cmd.split()[1]
      	try: opt=int(opt)
      	except ValueError: print 'Syntax error'; continue
      if cmd == "l":
        if opt == 0: c.SetLogy(0)
        else       : c.SetLogy(1)
                               #if num < 4: self.hListDwc[num].Draw()
                               #else      : self.hListDwc[num].Draw("box")
      else: 
        hname = cmd
        if hname in self.hDict:
          self.hDict[hname].Draw()
        else:
          print 'Unknown histogram'
      c.Update()


def Usage():
  print "Read raw data from text file and create monitor histograms"
  print "Usage: DrMon.py [options]"
  print "   -f fname    Data file to analize (def=the latest data file)"
  print "   -e maxEv    Maximum numver of events to be monitored (def=inf)"  
  print "   -s sample   Analyze only one event every 'sample'"
  sys.exit(2)

# Parse command line
fname  = ""
events = 999999999
sample = 1
try:
  opts, args = getopt.getopt(sys.argv[1:], "hf:e:s:")
except getopt.GetoptError, err:
  print str(err)
  Usage()
try:
  for o,a in opts:
    if    o == "-h": Usage()
    elif  o == "-f": fname    = a
    elif  o == "-e": events   = int(a)
    elif  o == "-s": sample   = int(a)
except(AttributeError): pass

if len(fname) < 1:
  list_of_files = glob.glob( PathToData + 'sps2021data*txt' ) 
  fname = max(list_of_files, key=os.path.getctime)

print 'Analizing', fname

drMon = DrMon(fname, events, sample)
drMon.bookAdcHistos(256)
drMon.bookTdcHistos(256)
drMon.bookDwcHistos(50)
drMon.readFile()
drMon.commander()

