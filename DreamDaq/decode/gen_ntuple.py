#!/usr/bin/env python

import os, sys, string
import stat, grp, pwd
import locale
import time

if len(sys.argv) == 1:
	print "Input run range that you want to generate Ntuples"

elif len(sys.argv) == 4 and sys.argv[2] == '-':
        run_range_lower = string.atoi(sys.argv[1])
        run_range_upper = string.atoi(sys.argv[3])

        if run_range_lower > run_range_upper:
		print "You have to input a run number for the lower bound first" 
		sys.exit(1)

        print "You will generate ntuples from the run %s to %s" %(run_range_lower, run_range_upper)

 	for i in range(run_range_lower, run_range_upper+1):
		print "Generaing Ntuple for the run %d" %(i)
		os.system("./myDaq2Ntu "+str(i)+" pedestal")
       		os.system("./myDaq2Ntu "+str(i)+" data")

else:
        for j in sys.argv[1:]:
        	print "Generating Ntuple for the run %s" %(j)
                os.system("./myDaq2Ntu "+j+" pedestal")
                os.system("./myDaq2Ntu "+j+" data")
                
