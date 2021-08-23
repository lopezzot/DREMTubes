#!/bin/bash -f
echo "1050 63 "`doOfflineHisto 1050 data | grep -v EOF | awk '{print $5 " " $10}'`
echo "1052 67 "`doOfflineHisto 1052 data | grep -v EOF | awk '{print $5 " " $10}'`
echo "1053 65 "`doOfflineHisto 1053 data | grep -v EOF | awk '{print $5 " " $10}'`
echo "1054 69 "`doOfflineHisto 1054 data | grep -v EOF | awk '{print $5 " " $10}'`
echo "1056 73 "`doOfflineHisto 1056 data | grep -v EOF | awk '{print $5 " " $10}'`
echo "1058 71 "`doOfflineHisto 1058 data | grep -v EOF | awk '{print $5 " " $10}'`
echo "1059 73 "`doOfflineHisto 1059 data | grep -v EOF | awk '{print $5 " " $10}'`
echo "1060 75 "`doOfflineHisto 1060 data | grep -v EOF | awk '{print $5 " " $10}'`
echo "1061 77 "`doOfflineHisto 1061 data | grep -v EOF | awk '{print $5 " " $10}'`
echo "1063 78.5 "`doOfflineHisto 1063 data | grep -v EOF | awk '{print $5 " " $10}'`
