#!/bin/bash

for f in $(ls sps2021data.run58[789].txt sps2021data.run59*.txt sps2021data.run60*.txt sps2021data.run61*.txt sps2021data.run62*.txt sps2021data.run630.txt)
 do
  ls $f
  grep ffff0005 $f > $f.bug
 done
