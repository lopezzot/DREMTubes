#!/bin/bash -f
echo "1118 0 "`doOfflineHisto 1118 data | grep -v EOF | awk '{print $5 " " $10}'`
echo "1119 5 "`doOfflineHisto 1119 data | grep -v EOF | awk '{print $5 " " $10}'`
echo "1122 10 "`doOfflineHisto 1122 data | grep -v EOF | awk '{print $5 " " $10}'`
echo "1124 15 "`doOfflineHisto 1124 data | grep -v EOF | awk '{print $5 " " $10}'`
echo "1125 20 "`doOfflineHisto 1125 data | grep -v EOF | awk '{print $5 " " $10}'`
echo "1127 25 "`doOfflineHisto 1127 data | grep -v EOF | awk '{print $5 " " $10}'`
echo "1128 30 "`doOfflineHisto 1128 data | grep -v EOF | awk '{print $5 " " $10}'`
