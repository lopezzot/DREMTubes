# DREMTubes
**Repository for Dual-Readout Calorimetry 2021 beam tests using the 2020 DR EM-sized Tubes prototype. It includes the Geant4 simulation, the DAQ code, the test-beam-data conversion tools and monitoring, and the physics analysis code.**

<!-- TABLE OF CONTENTS -->
<details open="open">
  <summary>Table of Contents</summary>
  <ol>
    <li><a href="#project-description">Project description</a></li>
    <li><a href="#authors-and-contacts">Authors and contacts</a></li>
     <li>
      <a href="#documentation-and-results">Documentation and results</a>
      <ul>
        <li><a href="#cern-twiki">CERN Twiki</a></li>
      </ul>
       <ul>
        <li><a href="#selected-presentations">Selected presentations</a></li>
      </ul>
    </li>
    <li><a href="#pictures">Pictures</a></li>
  </ol>                                           
</details>

<!--Project desription-->
## Project description
Code developed in the framework of the 2021 beam tests of the first tubes-based dual-readout calorimeter. To be used for both the Desy and the CERN-SPS related beam tests.\
Refer to README.md files in subfolders for instructions and further documentation:

- DREMTubes/ **A Geant4 simulation of the 2020 Dual-Readout em-sized tubes prototype beam tests.**
- DreamDaq/ **DAQ code used at the 2021 SPS Dual-Readout test beam.**
- TBDataPreparation/ **Code to convert raw test-beam-data to ROOT ntuples, and raw test-beam-data monitoring.**
- TBDataAnalysis/ **Code to analyze ROOT physics ntuples.**

Coding began on July 7, 2021.

<!--Authors and contacts-->
## Authors and contacts
- (CERN EP-SFT) Lorenzo Pezzotti (lorenzo.pezzotti@cern.ch), Alberto Ribon
- (University of Pavia and INFN Pavia) Jinky Agarwala, Gabriella Gaudio, Andrea Negri, Roberto Ferrari (roberto.ferrari@cern.ch), Giacomo Polesello
- (University of Insubria and INFN Milano) Romualdo Santoro (romualdo.santoro@uninsubria.it), Edoardo Proserpio, Simona Cometti, Agnese Giaz
- (University of Sussex) Iacopo Vivarelli (iacopo.vivarelli@cern.ch)

<!--Documentation and results-->
## Documentation and results

### CERN Twiki
- **CERN-SPS** August 2021 test beam Twiki [![Website shields.io](https://img.shields.io/website-up-down-green-red/http/shields.io.svg)](https://twiki.cern.ch/twiki/bin/view/DREAM/DreamTBAugust2021)
- **Desy** June 2021 test beam Twiki [![Website shields.io](https://img.shields.io/website-up-down-green-red/http/shields.io.svg)](https://twiki.cern.ch/twiki/bin/view/DREAM/DreamTBJune2021)

### Selected presentations
- Dual-Readout Calorimetry Meeting 19/11/2021, **SPS Testbeam Data Analysis & Online Beam Monitoring** [![Website shields.io](https://img.shields.io/website-up-down-green-red/http/shields.io.svg)](https://indico.cern.ch/event/1097245/contributions/4619481/attachments/2347737/4003780/DualROMeeting_20211117_Andreas.pdf)
- Dual-Readout Calorimetry Meeting 19/11/2021, **A first look to SPS TB data** [![Website shields.io](https://img.shields.io/website-up-down-green-red/http/shields.io.svg)](https://indico.cern.ch/event/1097245/contributions/4619162/attachments/2347702/4003707/gp_tbana_1.pdf)
- Dual-Readout Calorimetry Meeting 19/11/2021, **Results from the CERN TB Geant4 simulation** [![Website shields.io](https://img.shields.io/website-up-down-green-red/http/shields.io.svg)](https://indico.cern.ch/event/1097245/contributions/4619316/attachments/2347762/4003816/lopezzot_DRSW_17_11_2021.pdf)
- Dual-Readout Calorimetry Meeting 13/10/2021, **Status of 2021 Test Beam(s) SW** [![Website shields.io](https://img.shields.io/website-up-down-green-red/http/shields.io.svg)](https://indico.cern.ch/event/1086651/contributions/4569695/attachments/2327255/3964777/lopezzot_DR_SW_13_10_2021.pdf)
- Dual-Readout Calorimetry Meeting 21/7/2021, **DREMTubes: A Geant4 simulation of the DR tubes prototype 2021 beam tests** [![Website shields.io](https://img.shields.io/website-up-down-green-red/http/shields.io.svg)](https://indico.cern.ch/event/1061304/contributions/4460441/attachments/2285253/3883980/DR_lopezzot_21_7_2021.pdf)

<!--Pictures-->
## Pictures
<figure>
<figcaption align="center"><b>Fig. - Desy test beam setup.</b></figcaption>
<img src="https://github.com/lopezzot/DREMTubes/blob/dev/images/Desy_TB.jpeg" alt="Trulli" style="width:100%">
</figure>

<figure>
<figcaption align="center"><b>Fig. - Front view of the CERN SPS H8 beam line.</b></figcaption>
<img src="https://github.com/lopezzot/DREMTubes/blob/dev/images/CERN_TB.jpeg" alt="Trulli" style="width:100%">
</figure>
