#!/usr/bin/env python
import DREvent
import sys
import ROOT
import getopt
import glob
import os
import re
import commands

PathToData='/home/dreamtest/SPS.2021.08/'
BLUBOLD='\033[94m\033[1m'
BOLD   ='\033[1m'
BLU    ='\033[94m'
RED    ='\033[31m'
NOCOLOR='\033[0m'

# CONFIGURATION
NumAdcChannels = 96
NumTdcChannels = 48
ns_TdcCounts   = 0.139063
ns_mm          = 5.333333  
mm_ns          = 1./ns_mm

################################################################
# SIGNAL HANDLER ###############################################
################################################################
from signal import signal, SIGINT
stop=False
def handler(signal_rcv, frame):
  global stop
  stop=True


#################################################################
### DrMon CLASS #################################################
#################################################################
class DrMon:
  '''Data monitoring class for DualReadout 2021 Test Beam @H8  '''

  ##### DrMon method #######
  def __init__(self, fname, maxEvts, sample, trigCut=0):
    '''Constructor '''
    self.fname      = fname    # File name
    self.maxEvts    = maxEvts  # Max number of events to process
    self.sample     = sample   # Sampling fraction
    self.trigCut    = trigCut  # Trigger cut  
    self.hDict      = {}       # Dictionary of histograms
    self.histoMap   = {}       # Mapping dictionary
    self.lastEv     = None     # Last DREvent object
    self.canvas     = None     # ROOT canvas
    self.canNum     = 0        # Number of pads in canvas
    self.numOfLines = 0        # Number of lines in the file
    self.lastLine   = 0        # Last line read
    self.runNum = "0"
    tmp=re.findall('\d+',fname)
    if len(tmp) != 0:
      self.runNum = tmp[-1]
    self.cmdShCuts  = {        # Mapping between command shortCuts and commands
       "pmtMapC"    : self.DrawPmtAdcMapC, 
       "pmtMapS"    : self.DrawPmtAdcMapS,  
       "fers"       : self.DrawFers,
       "beam"       : self.DrawDwcBeamProfile, 
       "beam_mm"    : self.DrawDwcBeamProfile_mm,
       "dwcTDCs"    : self.DrawDwcTDCs,
       "caloPos"    : self.DrawCaloCenterOfGravity,
       "caloTot"    : self.DrawTotalEnergyInCalo,
       "MuonOverPre": self.DrawMuonOverPre,
    }
    self.cmdShCutsV = list(self.cmdShCuts)
    self.NumOfLinesOfThisFile()
    self.histoMapping()

  ##### DrMon method #######
  def histoMapping(self):	
    '''Create the histo mapping dictionary '''
    # TDC
    for i in range(1, 9):
      self.histoMap[ "tdc-C%d" % i ] = "tdc-%02d" % (i + 31)   # ch 32-29
    for i in range(1, 9):
      self.histoMap[ "tdc-S%d" % i ] = "tdc-%02d" % (i + 39)   # ch 40-47

    # ADC
    for i in range(1, 9):
      self.histoMap[ "adc-C%d" % i ] = "adc-%02d" % (i - 1)
    for i in range(1, 9):
      self.histoMap[ "adc-S%d" % i ] = "adc-%02d" % (i + 7)

    # FERS
    for i in range(0, 5):
      self.histoMap[ "fers-%d" % i ] = "tdc-%02d" % (i + 8)

    self.histoMap[ "MuonTrk" ] = "adc-32"
    self.histoMap[ "Cheren1" ] = "adc-64"
    self.histoMap[ "Cheren2" ] = "adc-65"
    self.histoMap[ "PreShow" ] = "adc-16"
    
  ##### DrMon method #######
  def NumOfLinesOfThisFile(self):
    '''Calculate the number of lines '''
    cmd = 'wc -l ' + fname
    out = commands.getstatusoutput(cmd)[1]
    self.numOfLines = int(out.split()[0])
    
  ##### DrMon method #######
  def book1D(self, hname, bins, mi, ma, axTitle=""):
    '''Utility to book histograms 1D '''
    h = ROOT.TH1I(hname, hname, bins, mi, ma) 
    h.GetXaxis().SetTitle(axTitle)
    self.hDict[hname] = h
    return h
  
  ##### DrMon method #######
  def book2D(self, hname, bins, mi, ma, axTitle="", ayTitle=""):
    '''Utility to book histograms 2D '''
    h = ROOT.TH2I(hname, hname, bins, mi, ma, bins, mi, ma) 
    self.hDict[hname] = h
    h.GetXaxis().SetTitle(axTitle)
    h.GetYaxis().SetTitle(ayTitle)
    return h

  ##### DrMon method #######
  def bookAdcHistos(self, bins):
    '''Book ADC histograms '''
    for i in range(NumAdcChannels):
      hname  = "adc-%02d" % i
      self.book1D( "adc-%02d" % i, bins, 0, 4096, 'adcCounts')

    # Histograms to monitor CALO position
    for kind in ("C", "S"):
      for ax in ("X", "Y"):
        hname = "calo%s_%s" % (kind, ax)
        htitl = "Calo %s center of gravity using %s fibers" % (ax, kind) 
        self.book1D( hname, 160, -80, 80, 'adcCounts')
    
    # Total energy
    self.book1D( "PmtTotC",  bins, 0, 4096, 'adcCounts')
    self.book1D( "PmtTotS",  bins, 0, 4096, 'adcCounts')
    self.book2D( "PmtTotSC", bins, 0, 4096, 'S [adcCounts]', 'C [adcCounts]')
    
    self.book2D( "HitMap_S", 3, -1.5, 1.5, 'tower', 'tower')
    self.book2D( "HitMap_C", 3, -1.5, 1.5, 'tower', 'tower')
    
    print "Booked ADCs histograms"

  ##### DrMon method #######
  def bookTdcHistos(self, bins):
    '''Book TDC histograms '''
    for i in range(NumTdcChannels):
      hname = "tdc-%02d" % i
      htitle = hname
      if i==0 or i==4: htitle = htitle + " DWC" + str(i/4+1)+ " left"
      if i==1 or i==5: htitle = htitle + " DWC" + str(i/4+1)+ " right"
      if i==2 or i==6: htitle = htitle + " DWC" + str(i/4+1)+ " up"
      if i==3 or i==7: htitle = htitle + " DWC" + str(i/4+1)+ " down"
      self.hDict[hname] = ROOT.TH1I(hname, htitle, bins, 0, 4096)      
    self.book1D( "tdc_sz", NumTdcChannels, -0.5, NumTdcChannels+0.5, 'NumOfTdcCh per event')

    print "Booked TDCs histograms:"
  
  ##### DrMon method #######
  def bookDwcHistos(self, bins):
    '''Book DWC histograms '''
    for i in range(1, 3):
      dwc   = "dw%d" % i
      lim   = 2048
      self.book1D(dwc + "l-r", bins, -lim, lim, "tdcCounts")
      self.book1D(dwc + "u-d", bins, -lim, lim, "tdcCounts")

      lim  = 4096
      self.book2D(dwc + "l/r",   bins,    0, lim, "tdcCounts", "tdcCounts")
      self.book2D(dwc + "u/d",   bins,    0, lim, "tdcCounts", "tdcCounts")
      self.book2D(dwc + "XY",    bins, -lim, lim, "tdcCounts", "tdcCounts")
      
      lim  = 64
      self.book2D(dwc + "XY_mm", bins, -lim, lim, 'mm',        'mm')
    
    lim  = 4096
    self.book2D("dwx1/x2", bins, -lim, lim, "tdcCounts", "tdcCounts")
    self.book2D("dwy1/y2", bins, -lim, lim, "tdcCounts", "tdcCounts")

    lim  =  48
    self.book1D( "dwx1-x2", bins, -lim, lim, 'mm')
    self.book1D( "dwy1-y2", bins, -lim, lim, 'mm')
    
    print "Booked DWCs histograms"

  ##### DrMon method #######
  def bookOthers(self):
    '''Book others histograms '''
    self.book1D( "trMask", 8, -0.5, 7.5)
    self.book2D( "pre/muon", 4096, 0, 4096, "preshower [adcCounts]",  "muonTraker [adcCounts]")
    self.book2D( "chere1/2", 4096, 0, 4096, "Cherenkov1 [adcCounts]", "Cherenkov2 [adcCounts]")
     

  ##### DrMon method #######
  def SetFillColor(self, col):
    '''Set fill color'''
    for h in self.hDict.values():
      h.SetFillColor(col)


  ##### DrMon method #######
  def centerOfGravity(self, weight, neg, pos):
    '''Calculate center of gravity of 2 couples of numbers 
       the 1st set positioned at -weight and the 2nd at +weight'''
    norm = sum(neg) + sum(pos)
    if norm == 0: 
      return None
    numerator = -weight*sum(neg) + weight*sum(pos)
    return numerator/norm


  ##### DrMon method #######
  def hFillPmtHitMapS(self, event):
    adc = event.ADCs
    h = self.hDict['HitMap_S']
    if adc[15] > 198: h.Fill(-1, 1)
    if adc[14] > 190: h.Fill( 0, 1)
    if adc[13] > 174: h.Fill( 1, 1)
    if adc[12] > 219: h.Fill(-1, 0)
    if adc[11] > 214: h.Fill( 1, 0)
    if adc[10] > 236: h.Fill(-1,-1)
    if adc[ 9] > 177: h.Fill( 0,-1)
    if adc[ 8] > 191: h.Fill( 1,-1)

  ##### DrMon method #######
  def hFillPmtHitMapC(self, event):
    adc = event.ADCs
    h = self.hDict['HitMap_C']
    if adc[ 7] > 165: h.Fill(-1, 1)
    if adc[ 6] > 218: h.Fill( 0, 1)
    if adc[ 5] > 174: h.Fill( 1, 1)
    if adc[ 4] > 191: h.Fill(-1, 0)
    if adc[ 3] > 229: h.Fill( 1, 0)
    if adc[ 2] > 218: h.Fill(-1,-1)
    if adc[ 1] > 232: h.Fill( 0,-1)
    if adc[ 0] > 232: h.Fill( 1,-1)

  ##### DrMon method #######
  def hFill(self, event):
    '''Fill the histogram '''

    if self.trigCut:
      if event.TriggerMask != self.trigCut:
        return
    
    # Others
    self.hDict["trMask"].Fill(event.TriggerMask)
    self.hDict["pre/muon"].Fill( event.ADCs[16],  event.ADCs[32])
    self.hDict["chere1/2"].Fill( event.ADCs[64],  event.ADCs[65])

    # ADC
    for ch, val in event.ADCs.items():
      hname  = "adc-%02d" % ch
      self.hDict[hname].Fill(val)
    
    self.hFillPmtHitMapS(event)
    self.hFillPmtHitMapC(event)
  
    # Center of gravity
    adc=event.ADCs 
    if event.TriggerMask & 0x1:
      # Calorimeter position Scintillator fibers
      x = self.centerOfGravity(35, ( adc[10], adc[12], adc[15] ), ( adc[ 8], adc[11], adc[13] ) )
      y = self.centerOfGravity(35, ( adc[10], adc[ 9], adc[ 8] ), ( adc[15], adc[14], adc[13] ) )
      if x != None and y != None: 
        self.hDict["caloS_X"].Fill(x)
        self.hDict["caloS_Y"].Fill(y)

      # Calorimeter position Cherenkov fibers
      x = self.centerOfGravity(35, ( adc[2], adc[4], adc[7] ), ( adc[0], adc[3], adc[5] ) )
      y = self.centerOfGravity(35, ( adc[2], adc[1], adc[0] ), ( adc[7], adc[6], adc[5] ) )
      if x != None and y != None: 
        self.hDict["caloC_X"].Fill(x)
        self.hDict["caloC_Y"].Fill(y)

    # Total
    sumC, sumS = 0, 0
    for i in range(8):
      sumC += adc[i]
      sumS += adc[i+8]
    self.hDict["PmtTotS"].Fill(sumS)
    self.hDict["PmtTotC"].Fill(sumC)
    self.hDict["PmtTotSC"].Fill(sumS, sumC)
    
    # TDC
    for ch, val in event.TDCs.items():
      hname  = "tdc-%02d" % ch
      self.hDict[hname].Fill(val[0])
    self.hDict["tdc_sz"].Fill(len(event.TDCs))
 
    # DWC: 
    dwcvect=[]
    for i in range(0,7,2):    
      if i in event.TDCs and i+1 in event.TDCs:       # DWC 1: left and right
        vA, vB = event.TDCs[i][0], event.TDCs[i+1][0]
        a , b = "l", "r"
        if i%4 == 0: 
          a , b = "u", "d"
        self.hDict[ 'dw%d%s-%s' %  (i/4 +1, a, b ) ].Fill( vA - vB)
        self.hDict[ 'dw%d%s/%s' %  (i/4 +1, a, b ) ].Fill( vA , vB)
        dwcvect.append(vA-vB)
    if len(dwcvect) == 4:
      k = mm_ns * ns_TdcCounts
      x1, y1 = dwcvect[0], dwcvect[1]
      x2, y2 = dwcvect[2], dwcvect[3] 
      self.hDict['dw1XY'].Fill(x1, y1)
      self.hDict['dw2XY'].Fill(x2, y2)
      self.hDict['dw1XY_mm'].Fill(x1*k, y1*k)
      self.hDict['dw2XY_mm'].Fill(x2*k, y2*k)
      self.hDict['dwx1/x2' ].Fill(x1, x2)
      self.hDict['dwy1/y2' ].Fill(y1, y2)
      self.hDict['dwx1-x2' ].Fill(x1*k - x2*k)
      self.hDict['dwy1-y2' ].Fill(y1*k - y2*k)
     

  ##### DrMon method #######
  def readFile(self, offset=0):
    '''Read raw ascii data from file, call the decoding function, fill the histograms'''
    print "Read and parse. Type CTRL+C to interrupt"
     
    global stop   
    stop = False
    step=1
    for i, line in enumerate(open(fname)):
      if stop: break
      if i < offset: continue
      self.lastLine = i
      if i%self.sample==0:
        ev = DREvent.DRdecode(line) 
        if i==0:
          print ev.headLine()
        if i%step==0: print ev
        if i>step*10: step=step*10
        self.hFill(ev)
        self.lastEv = ev
      if i >= self.maxEvts: break

  ##### DrMon method #######
  def dumpHelp(self):
    '''Write the list of available histograms'''
    print BLU,; print "Available histograms:", NOCOLOR
    l = sorted(self.hDict.keys()) 
    lTdc =  [x for x in l if x.startswith("tdc-")]
    lAdc =  [x for x in l if x.startswith("adc-")]
    lOth =  [x for x in l if not x.startswith("adc-") and not x.startswith("tdc-") ]
    print "  %-8s   %-8s   %-8s " % ( lTdc[0], '--->', lTdc[-1] )
    print "  %-8s   %-8s   %-8s " % ( lAdc[0], '--->', lAdc[-1] )
    for i, h in enumerate( lOth ):
      print "  %-8s" % h,
      if (i+1)%8==0: print ""

    print BLU; print "Available histograms, aliases:", NOCOLOR
    for i, h in enumerate( sorted( self.histoMap.keys() ) ):
      print "  %-8s" % h,
      if (i+1)%8==0: print ""

    print BLU; print "Special canvases (calo maps, fers, etc ...)", NOCOLOR
    for i, h in enumerate( self.cmdShCutsV ):
      print "%2d) %-12s" % (i,h),
      if (i+1)%5==0: print ""

    print BLU;  
    print "Other commands:", NOCOLOR
    print "   l 0/1     SetLogY"
    print "   z 0/1     SetLogZ"
    print "   s         Dump file statistics"
    print "   c color   Change the color of all histos" 
    print "   r nEvts   Read other nEvts events"
    print "   q         Quit"


  ##### DrMon method #######
  def createCanvas(self, dim):
    '''Create a canvas'''
    s="Run" + self.runNum + ": "
    if   dim == 9: 
      self.canvas = ROOT.TCanvas('c9', s+'PMT TOWERS',0 , 0, 800, 800)
      self.canvas.Divide(3,3)
    elif dim == 8: 
      self.canvas = ROOT.TCanvas('c2', s+'IDEA-DR8', 0, 0, 800, 1000)
      self.canvas.Divide(2,4)
    elif dim == 4: 
      self.canvas = ROOT.TCanvas('c4', s+'IDEA-DR4', 0, 0, 800, 800)
      self.canvas.Divide(2,2)
    elif dim == 6: 
      self.canvas = ROOT.TCanvas('c6', s+'IDEA-DR6', 0, 0, 1000, 600)
      self.canvas.Divide(3,2)
    elif dim == 2: 
      self.canvas = ROOT.TCanvas('c2', s+'IDEA-DR2', 0, 0, 500, 1000)
      self.canvas.Divide(1,2)
    else: 
      self.canvas = ROOT.TCanvas('c1', s+'IDEA-DR', 0, 0, 800, 600)
    self.canNum = dim
  
  ##### DrMon method #######
  def setLogY(self, val):
    '''Set/unset logY scale'''
    val = self.ToNumber(val)
    if val == None: 
      return
    if self.canNum == 1:
      self.canvas.SetLogy(val)
    else:
      for i in range(1,self.canNum+1) :
        self.canvas.cd(i).SetLogy(val)
    print "SetLogy =", val

  ##### DrMon method #######
  def setLogZ(self, val):
    '''Set/unset logZ scale'''
    val = self.ToNumber(val)
    if not val: 
      return
    if self.canNum == 1:
      self.canvas.SetLogz(val)
    print "SetLogz =", val

  ##### DrMon method #######
  def DrawDwcTDCs(self):
    '''Draw the DWC TDCs'''
    if self.canNum != 8:
      self.createCanvas(8)
    for i in range (8):
      self.canvas.cd(i+1);
      self.hDict[ "tdc-%02d" % i ].Draw()
    self.canvas.Update()

  ##### DrMon method #######
  def DrawMuonOverPre(self):
    '''Draw the muonTraker histo over preShower one'''
    if self.canNum != 1:
      self.createCanvas(1)
    hp = self.hDict[ self.histoMap["PreShow"] ]
    hm = self.hDict[ self.histoMap["MuonTrk"] ]
    hp.SetFillColor(42); hp.Draw()
    hm.SetFillColor(45); hm.Draw("same")
    self.canvas.Update()

  ##### DrMon method #######
  def DrawDwcBeamProfile(self):
    '''Draw the beam profile histograms'''
    if self.canNum != 2:
      self.createCanvas(2)
    self.canvas.cd(1);  self.hDict[ "dw1XY" ].Draw('col')
    self.canvas.cd(2);  self.hDict[ "dw2XY" ].Draw('col')
    self.canvas.Update()
  
  ##### DrMon method #######
  def DrawDwcBeamProfile_mm(self):
    '''Draw the beam profile histograms in mm'''
    if self.canNum != 2:
      self.createCanvas(2)
    self.canvas.cd(1);  self.hDict[ "dw1XY_mm" ].Draw('col')
    self.canvas.cd(2);  self.hDict[ "dw2XY_mm" ].Draw('col')
    self.canvas.Update()

  ##### DrMon method #######
  def DrawFers(self):
    '''Draw the Fers channels'''
    if self.canNum != 6:
      self.createCanvas(6)
    for i in range(5):
      self.canvas.cd(i+1)
      s = "fers-%d" % (i)
      print s
      self.hDict[ self.histoMap[s] ].Draw()
    self.canvas.Update()

  ##### DrMon method #######
  def DrawPmtAdcMapC(self):
    '''Draw the PMT map for Cherenkov channels'''
    # -- Channel---    -- ADC Ch ---   -- PadsNum --
    # | 8 | 7 | 6 |    | 7 | 6 | 5 |   | 1 | 2 | 3 |
    # | 5 |   | 4 |    | 4 |   | 3 |   | 4 | 5 | 6 |
    # | 3 | 2 | 1 |    | 2 | 1 | 0 |   | 7 | 8 | 9 |
    # -------------    -------------   -------------
    if self.canNum != 9:
      self.createCanvas(9)
    ch=7
    for i in range(1,10):
      if i == 5: continue
      self.canvas.cd(i)
      self.hDict[ "adc-%02d" % ch ].Draw()
      ch -= 1
    self.canvas.Update()
  
  ##### DrMon method #######
  def DrawPmtAdcMapS(self):
    '''Draw the PMT map for Scintillator channels'''
    # -- Channel---    -- ADC Ch ---      -- PadsNum --
    # | 8 | 7 | 6 |    | 15 | 14 | 13 |   | 1 | 2 | 3 |
    # | 5 |   | 4 |    | 12 |    | 11 |   | 4 |   | 6 |
    # | 3 | 2 | 1 |    | 10 |  9 |  8 |   | 7 | 8 | 9 |
    # -------------    -------------      -------------
    if self.canNum != 9:
      self.createCanvas(9)
    ch=15
    for i in range(1,10):
      if i == 5: continue
      self.canvas.cd(i)
      self.hDict[ "adc-%02d" % ch ].Draw()
      ch -= 1
    self.canvas.Update()
  
  ##### DrMon method #######
  def DrawCaloCenterOfGravity(self):
    '''Draw histograms about calorimeter center of gravity'''
    if self.canNum != 4:
      self.createCanvas(4)
    self.canvas.cd(1); self.hDictDrawAndFit( "caloS_X" )
    self.canvas.cd(2); self.hDictDrawAndFit( "caloS_Y" ) 
    self.canvas.cd(3); self.hDictDrawAndFit( "caloC_X" ) 
    self.canvas.cd(4); self.hDictDrawAndFit( "caloC_Y" ) 
    self.canvas.Update()
  
  ##### DrMon method #######
  def DrawTotalEnergyInCalo(self):
    '''Draw the total energy in calo PMT'''
    if self.canNum != 2:
      self.createCanvas(2)
    self.canvas.cd(1); self.hDict['PmtTotS'].Draw()
    self.canvas.cd(2); self.hDict['PmtTotC'].Draw()
    self.canvas.Update()


  ##### DrMon method #######
  def DrawSingleHisto(self, cmd, opt):
    '''Draw single histogram'''

    if self.canNum != 1:
      self.createCanvas(1)
        
    h = None
    hname = cmd
    if hname in self.histoMap:
      h = self.hDict[ self.histoMap[hname] ]
    elif hname in self.hDict:
      h = self.hDict[hname]
    else:
      print RED, BOLD, 'Unknown histogram', NOCOLOR
      return
    if opt == "same":
      h.SetFillColor( h.GetFillColor() + 3 )
    h.Draw(opt)
    nBins=h.GetNbinsX()
    uFlow = h.GetBinContent(0)
    oFlow = h.GetBinContent(nBins+1)
    if uFlow*oFlow > 0:
      print BOLD
      print "Underflow:", h.GetBinContent(0)
      print "Overflow :", h.GetBinContent(nBins+1)
      print NOCOLOR
    self.canvas.Update()

  ##### DrMon method #######
  def hDictDrawAndFit(self, hname):
    '''Utility method'''
    h = self.hDict[ hname ]
    h.Draw()
    h.Fit("gaus")

  ##### DrMon method #######
  def DumpStats(self):
    print BLU, BOLD
    print 'File name:', fname
    print 'Events   :', self.lastEv.EventNumber
    print 'PhysEv   :', self.lastEv.NumOfPhysEv
    print 'PedeEv   :', self.lastEv.NumOfPedeEv
    print 'SpilEv   :', self.lastEv.NumOfSpilEv
    self.NumOfLinesOfThisFile()
    print '#OfLines :', self.numOfLines 
    print NOCOLOR

  ##### DrMon method #######
  def readMore(self, val):
    '''Interactive commander '''
    optInt=-1
    try: optInt=int(val)
    except ValueError: print 'Invalid parameter'; return
    if optInt>=0:
      self.maxEvts = self.lastLine + optInt         
      self.readFile(self.lastLine)

  
  ##### DrMon method #######
  def ToNumber(self, val):
    '''Interactive commander '''
    if not isinstance(val, (int, long)):
      try: return int(val)
      except ValueError: 
        print 'Not an integer'
        return None
    return None

  ##### DrMon method #######
  def commander(self):
    '''Interactive commander '''
    
    self.createCanvas(1)
    while True:
      # Command
      self.dumpHelp()
      print BLUBOLD + "\n__________________________________________________"
      line = raw_input( 'DrMon prompt --> ' + NOCOLOR)
      pars = line.split()
      if len(pars) == 0: continue
      cmd = pars[0]
      opt = ""
      if len(pars) > 1:
        opt = pars[1]

      if   cmd == "q": print "Bye"; sys.exit(0)
      elif cmd == "l": self.setLogY(opt)
      elif cmd == "z": self.setLogZ(opt)
      elif cmd == "s": self.DumpStats()
      elif cmd == "c": self.SetFillColor(opt) 
      elif cmd == "r": self.readMore(opt)      
      elif cmd in self.cmdShCuts:  # SHORTCUTS
        self.cmdShCuts[cmd]()
      elif cmd.isdigit():          # SHORTCUTS WITH DIGITS
        hIdx = int(cmd)
        if hIdx < len(self.cmdShCuts): 
          self.cmdShCuts[ self.cmdShCutsV[hIdx] ]()
      else: 
        self.DrawSingleHisto(cmd, opt)


def Usage():
  print "Read raw data from text file and create monitor histograms"
  print "Usage: DrMon.py [options]"
  print "   -f fname    Data file to analize (def=the latest data file)"
  print "   -e maxEv    Maximum numver of events to be monitored (def=inf)"  
  print "   -s sample   Analyze only one event every 'sample'"
  print "   -r runNbr   Analyze run number runNbr"
  print "   -t trigCut  Trigger cut [5=phys, 6=pede]"
  sys.exit(2)


################################################################
# MAIN #########################################################
################################################################
# Parse command line
fname  = ""
events = 999999999
sample = 1
run    = 0
trigCut= 0
try:
  opts, args = getopt.getopt(sys.argv[1:], "hf:e:s:r:t:")
except getopt.GetoptError, err:
  print str(err)
  Usage()
try:
  for o,a in opts:
    if    o == "-h": Usage()
    elif  o == "-f": fname    = a
    elif  o == "-e": events   = int(a)
    elif  o == "-s": sample   = int(a)
    elif  o == "-r": run      = a
    elif  o == "-t": trigCut  = int(a)
except(AttributeError): pass

if run > 0:
  fname = "sps2021data.run%s.txt" % run
  fname = PathToData + fname
elif len(fname) < 1:
  list_of_files = glob.glob( PathToData + 'sps2021data*txt' ) 
  fname = max(list_of_files, key=os.path.getctime)

if not os.path.isfile(fname):
  print RED, "[ERROR] File", fname, "not found", NOCOLOR
  sys.exit(404)

# Install signal handler to interrupt file reading
signal(SIGINT, handler)

print 'Analyzing', fname
drMon = DrMon(fname, events, sample, trigCut)
drMon.bookAdcHistos(512)
drMon.bookTdcHistos(512)
drMon.bookDwcHistos(512)
drMon.bookOthers()
drMon.SetFillColor(42)
drMon.readFile()
drMon.commander()

