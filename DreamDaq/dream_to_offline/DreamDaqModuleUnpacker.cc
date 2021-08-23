#include "DreamDaqModuleUnpacker.hh"

DreamDaqModuleUnpacker::DreamDaqModuleUnpacker() :
    lastStatus_(0)
{
}

DreamDaqModuleUnpacker::~DreamDaqModuleUnpacker()
{
}

void DreamDaqModuleUnpacker::setStatus(int value)
{
    lastStatus_ = value;
}

int DreamDaqModuleUnpacker::status() const
{
    return lastStatus_;
}
