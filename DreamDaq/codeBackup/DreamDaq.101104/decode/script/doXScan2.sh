#!/bin/bash -f
echo "1104 63 "`doOfflineHisto 1104 data | grep -v EOF | awk '{print $5 " " $10}'`
echo "1105 65 "`doOfflineHisto 1105 data | grep -v EOF | awk '{print $5 " " $10}'`
echo "1107 67 "`doOfflineHisto 1107 data | grep -v EOF | awk '{print $5 " " $10}'`
echo "1108 69 "`doOfflineHisto 1108 data | grep -v EOF | awk '{print $5 " " $10}'`
echo "1109 71 "`doOfflineHisto 1109 data | grep -v EOF | awk '{print $5 " " $10}'`
echo "1110 73 "`doOfflineHisto 1110 data | grep -v EOF | awk '{print $5 " " $10}'`
echo "1113 75 "`doOfflineHisto 1113 data | grep -v EOF | awk '{print $5 " " $10}'`
echo "1115 77 "`doOfflineHisto 1115 data | grep -v EOF | awk '{print $5 " " $10}'`
