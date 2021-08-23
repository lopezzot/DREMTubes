#include <cassert>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <unistd.h>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"

#include "myRawFile.h"

#include "DreamRunInfo.h"
#include "DreamDaqEvent.h"
#include "DreamDaqEventUnpacker.hh"
#include "DreamRawEventBrowser.hh"

#include "dream_md5.h"

#define EVENT_BUFFER_LEN 10000

using namespace std;

void print_usage(const char *prog)
{
    cout << "\nUsage: " << prog << " inputFile outputFile\n" << endl;
}

int main(int argc, char *argv[])
{
    DreamDaqEvent *event = 0;
    DreamDaqEventUnpacker *unpacker = 0;
    int status = EXIT_FAILURE;

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

    // Open the input file
    if (RawFileOpen(inputFile))
        exit(EXIT_FAILURE);

    // Initialize ROOT
    TROOT root("dream2root", "DREAM raw to root converter");

    // Open the output file
    TFile ofile(outputFile, "RECREATE");
    if (!ofile.IsOpen())
    {
        cerr << "Failed to open file " << outputFile << endl;
        exit(EXIT_FAILURE);
    }

    // Build the event tree
    TTree *tree = new TTree("Events", "DREAM Event Data");
    tree->Branch("Event", "DreamDaqEvent", &event, 64000);

    // Process events
    unsigned buf[EVENT_BUFFER_LEN];
    unsigned event_count, daqDataFormatVersion, runNumber;
    for (event_count=0;;++event_count)
    {
        const int evsize = RawFileReadEventData(buf);
        if (evsize == RAWDATAEOF)
        {
            status = EXIT_SUCCESS;
            break;
        }
        else if (evsize == RAWDATAUNEXPECTEDEOF)
        {
            cerr << "ERROR: unexpected EOF at event " << event_count
                 << " in file " << inputFile << endl;
            break;
        }
        else
            assert(evsize <= EVENT_BUFFER_LEN);

        // Collect the run info after we read the first event
        if (event_count == 0)
        {
            unsigned char md5[16];
            const int md5status = md5_file_digest(inputFile, md5);
            assert(md5status == 0);

            // printf("md5sum is ");
	    // for (unsigned j = 0; j < 16; ++j) {
            //     printf("%02x", md5[j]);
	    // }
            // printf("\n");

            std::vector<unsigned> moduleSequence;
            const int seqStaus = DreamRawEventBrowser::subEventSequence(
                (unsigned char *)buf, evsize, &moduleSequence);
            if (seqStaus)
            {
                cerr << "ERROR: the first event in file "
                     << inputFile << " is corrupt" << endl;
                ofile.Close();
                unlink(outputFile);
                exit(EXIT_FAILURE);
            }

            // printf("ID sequence is:");
            // for (unsigned i=0; i<moduleSequence.size(); ++i)
            //     printf(" 0x%08x", *(&moduleSequence[0] + i));
            // printf("\n");

            runNumber = GetRunNumber();

            // Here, we need to figure out the data format version.
            daqDataFormatVersion = 0;
            DreamRunInfo run("RunInfo", "DREAM Run Info",
                             daqDataFormatVersion,
                             runNumber, GetBegTime(),
                             GetEndTime(), GetTotEvts(),
                             &moduleSequence[0],
                             moduleSequence.size(), md5);
            run.Write();

            // Build the event
            event = new DreamDaqEvent(daqDataFormatVersion);

            // Associate an event unpacker with the event
            unpacker = new DreamDaqEventUnpacker(event);
        }

        // Collect the event info
        event->reset();
        event->runNumber = runNumber;
        event->eventNumber = GetEventNumber();
        event->spillNumber = GetSpillNumber();
        event->timeSec = GetEventTimes();
        event->timeUSec = GetEventTimeu();
        if (unpacker->unpack(buf))
            cerr << "ERROR: failed to unpack event " << event_count
                 << " in file " << inputFile << endl;
        tree->Fill();
    }

    RawFileClose();
    ofile.Write();
    ofile.Close();

    cout << "Wrote " << event_count << " events to file "
         << outputFile << endl;
    exit(status);
}
