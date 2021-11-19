# TBDataPreparation/202106_DESY 

This directory contains software to handle data from DESY testbeam 2021. 
* Parse txt file from VME DAQ and create raw ntuple
* Parse dat file from SIPM DAQ and create raw ntuple
* merge the two ntuple
* convert to physiscs ntuple

## script dir

### DREvent.py

definition of DREvent class, method to decode the VME daq files (header + patload containing ADC and TDC) 
TDC information are only included from run XXX on

### DRrootity.py

get the list of file to be rootified, call the  DREvent decode method, and write out the .root file

