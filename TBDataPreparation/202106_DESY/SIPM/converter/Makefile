CXX:=g++
CXXFLAGS:=$(shell root-config --cflags) $(shell root-config --libs) -O3 -march=native -std=c++17
INCLUDE:=readerfast.h hardcodedfast.h

readerfast: readerfast.cpp
	$(CXX) readerfast.cpp $(CXXFLAGS) -o dataconverter
