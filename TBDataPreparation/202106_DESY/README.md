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


# Mapping for VME Acquisition (ADCs and TDCs)

**Channel** | **ADC module** | **ADC ch** | **index in Ntuple**
------------ | ------------- | ------------- | -------------
C1 | ADC-0 | ch0 | 0
C2 | ADC-0 | ch2 | 1
C3 | ADC-0 | ch4 | 2
C4 | ADC-0 | ch6 | 3
C5 | ADC-0 | ch8 | 4
C6 | ADC-0 | ch10 | 5
C7 | ADC-0 | ch12 | 6
C8 | ADC-0 | ch14 | 7
S1 | ADC-0 | ch16 | 8
S2 | ADC-0 | ch18 | 9
S3 | ADC-0 | ch21 | 10
S4 | ADC-0 | ch22 | 11
S5 | ADC-0 | ch24 | 12
S6 | ADC-0 | ch27 | 13
S7 | ADC-0 | ch28 | 14
S8 | ADC-0 | ch31 | 15





