#!/bin/bash -f

echo Starting ntuple production for run $1
echo
 myDaq2Ntu $1 pedestal && myDaq2Ntu $1 data  