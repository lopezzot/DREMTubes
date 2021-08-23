g++ -O2 -o merge_drs `root-config --libs --cflags ` merge_drs_comp.cxx
