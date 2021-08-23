#!/bin/bash
for n in $(seq 649 657)
do
echo RUN  $n
myDaq2Ntu "$n" pedestal drs_matrix
myDaq2Ntu "$n" data  drs_matrix
done
for n in $(seq 649 657)
do
echo RUN  $n
merge_drs "$n" pedestal
merge_drs "$n" data
done

