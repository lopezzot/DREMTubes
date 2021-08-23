#ifndef DreamDaqEventUnpacker_hh_
#define DreamDaqEventUnpacker_hh_

#include <vector>
#include <utility>

#include "DaqModuleUnpackers.hh"

class DreamDaqModule;
class DreamDaqEvent;

typedef std::vector<std::pair<DreamDaqModule *, DreamDaqModuleUnpacker *> > UnpakerSequence;

class DreamDaqEventUnpacker
{
public:
    DreamDaqEventUnpacker::DreamDaqEventUnpacker();
    DreamDaqEventUnpacker::DreamDaqEventUnpacker(DreamDaqEvent* event);
    DreamDaqEventUnpacker::~DreamDaqEventUnpacker();

    // The following function returns 0 if everything is OK
    int unpack(unsigned *eventData) const;

    // The following function returns the number of words used
    // on success or a negative error code on failure. "buflen"
    // argument is in words.
    int pack(unsigned *buf, unsigned buflen) const;

    void setDreamDaqEvent(DreamDaqEvent* event);

    // The following function returns 0 on success.
    // It should be called only when the event is known
    // to the packer/unpacker.
    int setPackingSequence(const unsigned *subEventIds, unsigned nIds);

private:
    // Event object
    DreamDaqEvent* event_;

    // Unpacker sequence
    UnpakerSequence packSeq_;

    // Various unpackers
    CaenV260Unpacker CaenV260_;
    CaenV792Unpacker CaenV792_;
    LeCroy1182Unpacker LeCroy1182_;
    LeCroy1176Unpacker LeCroy1176_;
    TemperatureSensorsUnpacker TemperatureSensors_;
    TDS7254BUnpacker TDS7254B_;

    // The following functions are purposedly made unavailable
    DreamDaqEventUnpacker::DreamDaqEventUnpacker(const DreamDaqEventUnpacker&);
    DreamDaqEventUnpacker& DreamDaqEventUnpacker::operator=(
        const DreamDaqEventUnpacker&);
};

#endif // DreamDaqEventUnpacker_hh_
