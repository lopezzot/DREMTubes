import glob
import os

mrgpath = "/eos/user/i/ideadr/TB2021_Desy/mergedNtuple/"
recpath = "/eos/user/i/ideadr/TB2021_Desy/recoNtuple/"
 
my_list1 = ["108","113","116","117","118","119","120","121","122","132","180","182", "183", "184","185","186","187","188","189", "190","191","192","193","194","195","196","197","198","199","200","201","202","203","204","205","206","207","208","209","228","229","230","211","212","213","214","215","216","217","218","219","220","221","222","223","224","225","226","227","228","229","230"]
myconf ="1"


for fl in my_list1:
	cmnd1 = "root -l -b -q .x 'PhysicsConverter.C(\""+fl+"\"," + myconf + ")'"
	os.system(cmnd1)
	#cmnd2 = "mv physics_desy2021_run"+fl+".root "+phspath
	#os.system(cmnd2)

