#!/bin/bash
for l in $(seq 332 479)
do
echo RUN  $l
merge_drs "$l" pedestal 
merge_drs "$l" data 
done
for n in $(seq 658 1006)
do
echo RUN  $n
merge_drs "$n" pedestal 
merge_drs "$n" data  
done
