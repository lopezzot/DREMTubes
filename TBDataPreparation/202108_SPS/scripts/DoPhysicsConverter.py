import glob

datapath = "/eos/user/i/ideadr/TB2021_H8/mergedNtuple/"
datafls = [x[18:20] for x in glob.glob(datapath+"root")]
print datafls
