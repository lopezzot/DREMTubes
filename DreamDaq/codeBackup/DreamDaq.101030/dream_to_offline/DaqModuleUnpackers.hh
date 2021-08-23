#ifndef DaqModuleUnpackers_hh_
#define DaqModuleUnpackers_hh_

#include "DreamDaqModuleUnpacker.hh"

class CaenV260Unpacker : public DreamDaqModuleUnpacker
{
public:
    int unpack(unsigned daqVersion,
               DreamDaqModule *module, const unsigned *evdata);
    int pack(unsigned daqVersion, const DreamDaqModule *module,
             unsigned *buffer, unsigned buflen);
};

class CaenV792Unpacker : public DreamDaqModuleUnpacker
{
public:
    int unpack(unsigned daqVersion,
               DreamDaqModule *module, const unsigned *evdata);
    int pack(unsigned daqVersion, const DreamDaqModule *module,
             unsigned *buffer, unsigned buflen);
};

class LeCroy1176Unpacker : public DreamDaqModuleUnpacker
{
public:
    int unpack(unsigned daqVersion,
               DreamDaqModule *module, const unsigned *evdata);
    int pack(unsigned daqVersion, const DreamDaqModule *module,
             unsigned *buffer, unsigned buflen);
};

class LeCroy1182Unpacker : public DreamDaqModuleUnpacker
{
public:
    int unpack(unsigned daqVersion,
               DreamDaqModule *module, const unsigned *evdata);
    int pack(unsigned daqVersion, const DreamDaqModule *module,
             unsigned *buffer, unsigned buflen);
};

class TDS7254BUnpacker : public DreamDaqModuleUnpacker
{
public:
    int unpack(unsigned daqVersion,
               DreamDaqModule *module, const unsigned *evdata);
    int pack(unsigned daqVersion, const DreamDaqModule *module,
             unsigned *buffer, unsigned buflen);
};

class TemperatureSensorsUnpacker : public DreamDaqModuleUnpacker
{
public:
    int unpack(unsigned daqVersion,
               DreamDaqModule *module, const unsigned *evdata);
    int pack(unsigned daqVersion, const DreamDaqModule *module,
             unsigned *buffer, unsigned buflen);
};

#endif // DaqModuleUnpackers_hh_
