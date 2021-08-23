#ifndef DreamRawEventBrowser_hh_
#define DreamRawEventBrowser_hh_

#include <vector>

class DreamRawEventBrowser
{
public:
    // The following function returns 0 on success
    // and an error code on failure. "buf" is assumed
    // to point just after the event header.
    //
    // evsize is the event size in words.
    static int subEventSequence(const unsigned char* buf,
                                unsigned evsize,
                                std::vector<unsigned>* sequence);
};

#endif // DreamRawEventBrowser_hh_
