#ifndef DreamDaqModuleUnpacker_hh_
#define DreamDaqModuleUnpacker_hh_

class DreamDaqModule;

class DreamDaqModuleUnpacker
{
public:
    DreamDaqModuleUnpacker::DreamDaqModuleUnpacker();
    virtual DreamDaqModuleUnpacker::~DreamDaqModuleUnpacker();

    // The following should return 0 on success, error code on failure
    virtual int unpack(unsigned daqVersion,
                       DreamDaqModule *module,
                       const unsigned *evdata) = 0;

    // The following function returns the number of words used
    // on success or some negative error code on failure.
    // "buflen" arguments is in words (which is sizeof(unsigned)).
    // This function should not write beyond the length
    // of the buffer.
    virtual int pack(unsigned daqVersion,
                     const DreamDaqModule *module,
                     unsigned *buffer, unsigned buflen) = 0;

    int status() const;

protected:
    void setStatus(int value);

private:
    int lastStatus_;
};

#endif // DreamDaqModuleUnpacker_hh_
