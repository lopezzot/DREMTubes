#!/bin/bash -f

mv "/home/dreamtest/working/daqguilog.txt" "/home/dreamtest/working/daqguilog.sav"
DaqControlInterface 2>&1 >/home/dreamtest/working/daqguilog.txt &

exit 0
