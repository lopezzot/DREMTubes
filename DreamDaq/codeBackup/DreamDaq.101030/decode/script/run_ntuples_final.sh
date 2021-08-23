#!/bin/bash

myDaq2Ntu 278 pedestal nodrs_noscope
myDaq2Ntu 278 data nodrs_noscope

myDaq2Ntu 281 pedestal nodrs_noscope
myDaq2Ntu 281 data nodrs_noscope

myDaq2Ntu 284 pedestal nodrs_noscope
myDaq2Ntu 284 data nodrs_noscope

myDaq2Ntu 290 pedestal nodrs_noscope
myDaq2Ntu 290 data nodrs_noscope

myDaq2Ntu 292 pedestal nodrs_noscope
myDaq2Ntu 292 data nodrs_noscope

myDaq2Ntu 294 pedestal nodrs_noscope
myDaq2Ntu 294 data nodrs_noscope

myDaq2Ntu 295 pedestal nodrs_noscope
myDaq2Ntu 295 data nodrs_noscope

myDaq2Ntu 343 pedestal nodrs_noscope
myDaq2Ntu 343 data nodrs_noscope

myDaq2Ntu 346 pedestal nodrs_noscope
myDaq2Ntu 346 data nodrs_noscope

myDaq2Ntu 350 pedestal nodrs_noscope
myDaq2Ntu 350 data nodrs_noscope

myDaq2Ntu 354 pedestal nodrs_noscope
myDaq2Ntu 354 data nodrs_noscope

myDaq2Ntu 355 pedestal nodrs_noscope
myDaq2Ntu 355 data nodrs_noscope

myDaq2Ntu 360 pedestal nodrs_noscope
myDaq2Ntu 360 data nodrs_noscope

myDaq2Ntu 364 pedestal nodrs_noscope
myDaq2Ntu 364 data nodrs_noscope

myDaq2Ntu 369 pedestal nodrs_noscope
myDaq2Ntu 369 data nodrs_noscope

myDaq2Ntu 381 pedestal nodrs_noscope
myDaq2Ntu 381 data nodrs_noscope


for l in $(seq 297 331)
do
echo RUN  $l
myDaq2Ntu "$l" pedestal nodrs_noscope
myDaq2Ntu "$l" data nodrs_noscope
done


for k in $(seq 332 343)
do
echo RUN  $k
myDaq2Ntu "$k" pedestal nodrs_noscope
myDaq2Ntu "$k" data nodrs_noscope
done


for j in $(seq 956 984)
do
echo RUN  $j
myDaq2Ntu "$j" pedestal nodrs
myDaq2Ntu "$j" data nodrs
done
