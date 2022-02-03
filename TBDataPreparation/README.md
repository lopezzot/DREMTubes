# TBDataPreparation
**Repository for the code to convert raw test-beam data to ROOT ntuples.**

## Availale test beam data sets
For every relevant software version we release the corresponding test beam data set. The following is the list of available data sets so far.

### Testbeam at CERN SPS H8 line

| SW version    | Path          |  Comments     |
| ------------- | ------------- | ------------- |
| v1.3.1        | /eos/user/i/ideadr/TB2021_H8/CERNDATA/v1.3.1/ | Data produced with v1.3.1 i.e. with new calibration constants. Only PhysicsConverter was updated, so data production starts from mergedNtuples/ as copied from v1.3. Only reconNtuple/ files are stored.  |
| v1.3          | /eos/user/i/ideadr/TB2021_H8/CERNDATA/v1.3/ | Data produced starting from .bz2 files from PMT DAQ and from .root files from SiPM DAQ as available at the end of the CERN test beam. Produced by Lorenzo on 26/11/2021. |

### Testbeam at DESY
To be written.

## Data production

The following are instructions to handle raw test-beam data taken at the CERN SPS and DESY 2021 beam tests.
- An overview presentation on raw data handling:\
  Dual-Readout Calorimetry Meeting 13/10/2021, **Status of 2021 Test Beam(s) SW** [![Website shields.io](https://img.shields.io/website-up-down-green-red/http/shields.io.svg)](https://indico.cern.ch/event/1086651/contributions/4569695/attachments/2327255/3964777/lopezzot_DR_SW_13_10_2021.pdf)

### 202108_SPS: Testbeam at CERN SPS H8 line. 
The following are instructions to handle raw test-beam data taken at the CERN SPS 2021 test beam. The entire production takes about 4 hours.
1. Git clone the test-beam sw on lxplus
   ```sh
   git@github.com:lopezzot/DREMTubes.git
   ```
2. Copy the entire directory with raw data for auxiliary detectors (preshower, muon counter, cherenkov counters, ...) and PMT-read-out towers. Create the rawNtuple folder for data rootification
   ```sh
   cp -r /eos/user/i/ideadr/TB2021_H8/rawData/ .
   mkdir rawNtuple/
   ```
   One file is saved per each run and named sps2021.data.runNo.txt.bz2, with run numbers from 400 to 743. Therefore the entire data collection goes from ```sps2021.data.run400.txt.bz2``` to ```sps2021.data.run743.txt.bz2```.
3. Transform ASCII files to root files. Edit ```DREMTubes/TBDataPreparation/202108_SPS/scripts/DRrootify.py``` so that ```datapath``` and ```ntuplepath``` variables point to the ```rawData/``` and the ```rawNtuples/``` folders respectively. Then rootify the data
   ```sh
   python DREMTubes/TBDataPreparation/202108_SPS/scripts/DRrootify.py
   ```
   Hint: if something goes wrong just re-run the command, it will check for already produced .root files and skip them.
4. Copy the raw data from SiPM DAQ, and create directory for merged files
   ```sh
   cp -r /eos/user/i/ideadr/TB2021_H8/rawNtupleSiPM/ .
   mkdir mergedNtupe
   ```
4. Merge the information from the two DAQs. Edit the ```DREMTubes/TBDataPreparation/202108_SPS/scripts/DR_BlendedDaq2Root.py``` and make sure that the ```SiPMFileDir```, ```DaqFileDir``` and ```MergedFileDir``` point to the correct directories. Then
   ```sh
   python DREMTubes/TBDataPreparation/202108_SPS/scripts/DR_BlendedDaq2Root.py --newFiles
   ```
5. Create folder for reconstrcuted files. Edit ```DREMTubes/TBDataPreparation/202108_SPS/scripts/DoPhysicsConverter.py``` so that ```mrgpath```, ```recpath``` and ```phspath``` point to the correct repositories. Also edit ```DREMTubes/TBDataPreparation/202108_SPS/scripts/PhysicsConverter.C``` and modify the ```infile``` variable so that it points to the merged files directory. Then, apply calibration constants and high-level objects abstaction
   ```sh
   cd DREMTubes/TBDataPreparation/202108_SPS/scripts/
   python DoPhysicsConverter.py
   ```
   
### 202106_DESY: Testbeam at DESY. 
The following are instructions to handle raw test-beam data taken at the DESY 2021 test beam.
To be written.
