# DREMTubes - TBDataPreparation
**Repository for the code to convert raw test-beam data to ROOT ntuples.**

## Availale test beam data sets

### Testbeam at CERN SPS H8 line

For every relevant software version we release the corresponding test beam data set. The following is the list of available data sets so far.

| SW version    | Path          |  Comments     |
| ------------- | ------------- | ------------- |



## Data production

The following are instructions to handle raw test-beam data taken at the CERN SPS and DESY 2021 beam tests.\
An overview presentation: Dual-Readout Calorimetry Meeting 13/10/2021, **Status of 2021 Test Beam(s) SW** [![Website shields.io](https://img.shields.io/website-up-down-green-red/http/shields.io.svg)](https://indico.cern.ch/event/1086651/contributions/4569695/attachments/2327255/3964777/lopezzot_DR_SW_13_10_2021.pdf)

### 202108_SPS: Testbeam at CERN SPS H8 line. 
The following are instructions to handle raw test-beam data taken at the CERN SPS 2021 test beam.
1. Git clone the test-beam sw on lxplus:
   ```sh
   git@github.com:lopezzot/DREMTubes.git
   ```
2. copy the entire directory with raw data for auxiliary detectors (preshower, muon counter, cherenkov counters, ...) and PMT-read-out towers. Create the rawNtuple folder for data rootification.
   ```sh
   cp -r /eos/user/i/ideadr/TB2021_H8/rawData/ .
   mkdir rawNtuple/
   ```
   One file is saved per each run and named sps2021.data.runNo.txt.bz2, with run numbers from 400 to 743. Therefore the entire data collection goes from ```sps2021.data.run400.txt.bz2``` to ```sps2021.data.run743.txt.bz2```.
3. Transform ASCII files ro root files. Edit ```DREMTubes/TBDataPreparation/202108_SPS/scripts/DRrootify.py``` so that ```datapath``` and ```ntuplepath``` variables point to the ```rawData/``` and the ```rawNtuples/``` folders respectively. Then rootify the data
   ```sh
   python DREMTubes/TBDataPreparation/202108_SPS/scripts/DRrootify.py
   ```
   Hint: if something goes wrong just re-run the command, it will check for already produced .root files and skip them.
   







### 202106_DESY: Testbeam at DESY. 
To be written after the first data production.

