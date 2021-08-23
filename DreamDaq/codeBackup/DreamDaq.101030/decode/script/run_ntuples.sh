#!/bin/bash
for i in $(seq 156 179)   
do
echo RUN  $i
myDaq2Ntu "$i" pedestal nodrs_noscope
myDaq2Ntu "$i" data nodrs_noscope
done
for j in $(seq 185 197)
do
echo RUN  $j
myDaq2Ntu "$j" pedestal nodrs
myDaq2Ntu "$j" data nodrs
done
for k in $(seq 198 331)
do
echo RUN  $k
myDaq2Ntu "$k" pedestal nodrs_noscope
myDaq2Ntu "$k" data nodrs_noscope
done
for l in $(seq 332 479)
do
echo RUN  $l
myDaq2Ntu "$l" pedestal drs_fb_lk
myDaq2Ntu "$l" data drs_fb_lk
done
for m in $(seq 547 567)
do
echo RUN  $m
myDaq2Ntu "$m" pedestal nodrs
myDaq2Ntu "$m" data nodrs
done
for n in $(seq 658 1006)
do
echo RUN  $n
myDaq2Ntu "$n" pedestal drs_matrix 
myDaq2Ntu "$n" data  drs_matrix
done
for s in $(seq 1007 1543)
do
echo RUN  $s
myDaq2Ntu "$s" pedestal nodrs
myDaq2Ntu "$s" data nodrs
done
