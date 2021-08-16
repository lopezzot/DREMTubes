# DREMTubes - TBDataPreparation/202106\_DESY/PMT

This code can be used with data taken in Desy TB in June 2021, which are currently saved here:
/eos/user/i/ideadr/TB2021\_Desy/rawData

## Raw (.txt file ) data to raw Ntuple code

 * rawToNtuple.cxx
  * to be used for the first part of data taking, when TDC was non included in the DAQ
 * rawToNtuple\_TDC.cxx 
  * to be used for the first part of data taking    

### Usage
 * compile the rawToNtuple.cxx (rawToNtuple\_TDC.cxx) with _g++ -o rawToNtuple rawToNtuple.cxx `root-config --cflags --libs`_
 * run the script rawNtupleConverter.sh with _./rawNtupleConverter.sh filename.txt.bz2 TDC/noTDC_ for selecting the correct executable
