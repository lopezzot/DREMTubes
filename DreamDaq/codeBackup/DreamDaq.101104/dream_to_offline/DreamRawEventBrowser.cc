#include <cassert>

#include "DreamRawEventBrowser.hh"
#include "myFIFO-IOp.h"

int DreamRawEventBrowser::subEventSequence(const unsigned char* buf,
                                           const unsigned evsize,
                                           std::vector<unsigned>* sequence)
{
    const unsigned char *bufend = buf + evsize*sizeof(unsigned);
    sequence->clear();
    while (buf < bufend)
    {
        const SubEventHeader *sh = (const SubEventHeader *)buf;
        if (sh->semk != SUBEVMARK)
            return 1;
        sequence->push_back(sh->id);
        buf += sh->size;
    }
    return 0;
}
