#include <cassert>
#include <cstdio>

#include "DreamDaqEventUnpacker.hh"
#include "DreamDaqEvent.h"
#include "DaqModuleUnpackers.hh"

#include "myFIFO-IOp.h"
#include "myRawFile.h"

DreamDaqEventUnpacker::DreamDaqEventUnpacker() :
    event_(0)
{
}

DreamDaqEventUnpacker::DreamDaqEventUnpacker(DreamDaqEvent* event) :
    event_(0)
{
    setDreamDaqEvent(event);
}

DreamDaqEventUnpacker::~DreamDaqEventUnpacker()
{
}

void DreamDaqEventUnpacker::setDreamDaqEvent(DreamDaqEvent* event)
{
    packSeq_.clear();
    if (event)
    {
        // The sequence of unpackers does not matter.
        // The sequence of packers can be made correct
        // by calling the "setPackingSequence" method.
        packSeq_.push_back(std::make_pair(&event->TDC_0,   &LeCroy1176_));
        packSeq_.push_back(std::make_pair(&event->ADC_C1,  &CaenV792_));
        packSeq_.push_back(std::make_pair(&event->ADC_C2,  &CaenV792_));
        packSeq_.push_back(std::make_pair(&event->ADC_L,   &LeCroy1182_));
        packSeq_.push_back(std::make_pair(&event->SCA_0,   &CaenV260_));
        packSeq_.push_back(std::make_pair(&event->SCOPE_0, &TDS7254B_));
        packSeq_.push_back(std::make_pair(&event->TSENS,   &TemperatureSensors_));
    }
    event_ = event;
}

int DreamDaqEventUnpacker::setPackingSequence(
    const unsigned *subEventIds, unsigned nIds)
{
    UnpakerSequence ordered;
    const unsigned n_packers = packSeq_.size();

    int missing_packer = 0;
    for (unsigned i=0; i<nIds; ++i)
    {
        const unsigned id = subEventIds[i];
        unsigned packer_found = 0;
        for (unsigned j=0; j<n_packers; ++j)
        {
            DreamDaqModule *module = packSeq_[j].first;
            if (module->subEventId() == id)
            {
                packer_found = 1;
                ordered.push_back(packSeq_[j]);
                break;
            }
        }
        if (!packer_found)
        {
            fprintf(stderr, "Failed to find unpacker for sub event id 0x%08x\n", id);
            fflush(stderr);
            missing_packer++;
        }
    }
    packSeq_ = ordered;

    return missing_packer;
}

int DreamDaqEventUnpacker::unpack(unsigned *eventData) const
{
    int status = 0;
    for (unsigned i=0; i<packSeq_.size(); ++i)
    {
        DreamDaqModule *module = packSeq_[i].first;
        const int ustat = packSeq_[i].second->unpack(
            event_->formatVersion, module, eventData);
        if (ustat == NODATA)
            module->setEnabled(false);
        else
        {
            module->setEnabled(true);
            if (ustat)
                status = 1;
        }
    }
    return status;
}

int DreamDaqEventUnpacker::pack(unsigned *buf, unsigned buflen) const
{
    // Create the event header
    EventHeader evh;
    evh.evmark = EVENTMARKER;
    evh.evhsiz = sizeof(EventHeader);
    evh.evnum = event_->eventNumber;
    evh.spill = event_->spillNumber;
    evh.tsec = event_->timeSec;
    evh.tusec = event_->timeUSec;

    // Pack the event header
    int wordcount = sizeof(EventHeader)/sizeof(unsigned);
    assert(buflen > (unsigned)wordcount);
    memcpy(buf, &evh, sizeof(EventHeader));

    // Pack the modules
    for (unsigned i=0; i<packSeq_.size(); ++i)
    {
        DreamDaqModule *module = packSeq_[i].first;
        if (module->enabled())
        {
            int sz = packSeq_[i].second->pack(
                event_->formatVersion, module,
                buf+wordcount, buflen-wordcount);
            if (sz < 0)
                return sz;
            else
                wordcount += sz;
        }
    }

    // Set the correct event size (in bytes)
    buf[offsetof(EventHeader, evsiz)/sizeof(unsigned)] = 
        wordcount*sizeof(unsigned);

    return wordcount;
}
