#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cstddef>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

#include "myRawFile.h"

#include "DreamRunInfo.h"
#include "DreamDaqEvent.h"
#include "DreamDaqEventUnpacker.hh"

#include "dream_md5.h"

#define EVENT_BUFFER_LEN 10000
#define RUN_HEADER_ID 0xaabbccdd

using namespace std;

void write_run_header(FILE *out, const DreamRunInfo* run)
{
    RunHeader rh;
    rh.magic = RUN_HEADER_ID;
    rh.ruhsiz = sizeof(RunHeader);
    rh.runnumber = run->runNumber;
    rh.evtsinrun = run->nEvents;
    rh.begtim = run->startTime;
    rh.endtim = run->stopTime;
    fwrite(&rh, sizeof(RunHeader), 1, out);
}

void print_usage(const char *prog)
{
    cout << "\nUsage: " << prog << " inputFile outputFile\n" << endl;
}

int main(int argc, char *argv[])
{
    DreamDaqEvent *event = 0;
    DreamDaqEventUnpacker unpacker;
    FILE *out = 0;

    char *progname = strrchr(argv[0], '/');
    if (progname)
        ++progname;
    else
        progname = argv[0];

    if (argc != 3)
    {
        print_usage(progname);
        exit(EXIT_FAILURE);
    }

    const char *inputFile = argv[1];
    const char *outputFile = argv[2];

    // Initialize ROOT
    TROOT root("root2dream", "Root to DREAM raw converter");

    // Open the input file
    TFile infile(inputFile, "READ");
    if (!infile.IsOpen())
    {
        cerr << "Failed to open input file " << inputFile << endl;
        exit(EXIT_FAILURE);
    }

    // Find the run info and the event tree
    const DreamRunInfo *run = (DreamRunInfo *)infile.Get("RunInfo");
    if (run == 0)
    {
        cerr << "No DREAM run info in file " << inputFile << endl;
        exit(EXIT_FAILURE);
    }

    TTree *tree = (TTree *)infile.Get("Events");
    if (tree == 0)
    {
        cerr << "No DREAM event data in file " << inputFile << endl;
        exit(EXIT_FAILURE);
    }

    // Build the event object
    tree->SetBranchAddress("Event", &event);

    // Open the output file
    out = fopen(outputFile, "w");
    if (out == 0)
    {
        cerr << "Failed to open output file " << outputFile << endl;
        exit(EXIT_FAILURE);
    }

    // Write out the run header
    write_run_header(out, run);

    // Process events
    unsigned eventbuf[EVENT_BUFFER_LEN];
    const unsigned nentries = tree->GetEntries();
    for (unsigned iev=0; iev<nentries; ++iev)
    {
        tree->GetEntry(iev, 1);
        unpacker.setDreamDaqEvent(event);
        const int status = unpacker.setPackingSequence(
            run->subEventSequence, run->nSubEvents);
        if (status)
        {
            cerr << "ERROR: can't find all unpackers for event "
                 << iev << endl;
            exit(EXIT_FAILURE);
        }
        int sz = unpacker.pack(eventbuf, sizeof(eventbuf)/sizeof(eventbuf[0]));
        if (sz == -1)
        {
            cerr << "ERROR: insufficient event buffer size" << endl;
            exit(EXIT_FAILURE);
        }
        assert(sz >= 0);
        fwrite(eventbuf, sz*sizeof(eventbuf[0]), 1, out);
    }

    infile.Close();
    fclose(out);

    // Check the MD5 checksum against the one in the root file
    unsigned char md5[16];
    const int md5status = md5_file_digest(outputFile, md5);
    assert(md5status == 0);
    assert(sizeof(unsigned char) == sizeof(UChar_t));

    if (memcmp(md5, run->md5digest, 16*sizeof(UChar_t)))
    {
        cerr << "ERROR: bad output file checksum" << endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        cout << "Wrote " << nentries << " events to file "
             << outputFile << endl;
        exit(EXIT_SUCCESS);
    }
}
