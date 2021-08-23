#include <cassert>
#include <cstring>

#include "DaqModuleUnpackers.hh"

#include "CaenV260.h"
#include "CaenV792.h"
#include "LeCroy1176.h"
#include "LeCroy1182.h"
#include "TDS7254B.h"
#include "TemperatureSensors.h"

#include "myRawFile.h"
#include "myOscDataFile.h"
#include "myFIFO-IOp.h"

#define SUB_EVENT_MARKER 0xacabacab

#define check_header_size do {\
    const unsigned hSize = sizeof(SubEventHeader)/sizeof(unsigned);\
    if (buflen < hSize) {\
        setStatus(-1);\
        return -1;\
    }\
    buffer += hSize;\
    buflen -= hSize;\
} while(0);

#define write_header do {\
    const unsigned hSize = sizeof(SubEventHeader)/sizeof(unsigned);\
    pack_subevent_header(mod->subEventId(), wc,\
                         buffer - hSize);\
    setStatus(wc + hSize);\
    return wc + hSize;\
} while(0);

template <class F1, class F2, class T1, class T2>
static void copy_var_buffer(const F1 *from1, const F2 *from2,
                            const unsigned nFrom,
                            T1 **to1, T2 **to2, UInt_t *sizeTo)
{
    if (*sizeTo < nFrom)
    {
        delete [] *to1;
        delete [] *to2;
        *to1 = new T1[nFrom];
        *to2 = new T2[nFrom];
        *sizeTo = nFrom;
    }
    for (unsigned i=0; i<nFrom; ++i)
        (*to1)[i] = from1[i];
    for (unsigned i=0; i<nFrom; ++i)
        (*to2)[i] = from2[i];    
}

template <class F1, class T1>
static void copy_var_buffer_1(const F1 *from1, const unsigned nFrom,
                              T1 **to1, UInt_t *sizeTo)
{
    if (*sizeTo < nFrom)
    {
        delete [] *to1;
        *to1 = new T1[nFrom];
        *sizeTo = nFrom;
    }
    for (unsigned i=0; i<nFrom; ++i)
        (*to1)[i] = from1[i];
}

static void pack_subevent_header(const unsigned int id,
                                 const unsigned int evsize,
                                 unsigned *buf)
{
    SubEventHeader sh;
    sh.semk = SUB_EVENT_MARKER;
    sh.sevhsiz = sizeof(SubEventHeader);
    sh.id = id;
    sh.size = sizeof(SubEventHeader) + evsize*sizeof(unsigned);
    memcpy(buf, &sh, sizeof(SubEventHeader));
}

int CaenV260Unpacker::unpack(unsigned daqVersion,
                             DreamDaqModule *module,
                             const unsigned *evdata)
{
    CaenV260 *mod = dynamic_cast<CaenV260 *>(module);
    assert(mod);

    const unsigned *addr = SubEventSeek(mod->subEventId(), evdata);
    mySCA scaData;
    const int status = DecodeV260(addr, &scaData);

    if (!status)
    {
        copy_var_buffer(scaData.channel, scaData.counts, scaData.index,
                        &mod->channel, &mod->count, &mod->nAlloc_);
        mod->nHits = scaData.index;
    }

    setStatus(status);
    return status;
}

int CaenV260Unpacker::pack(unsigned daqVersion,
                           const DreamDaqModule *module,
                           unsigned *buffer, unsigned buflen)
{
    const CaenV260 *mod = dynamic_cast<const CaenV260 *>(module);
    assert(mod);

    unsigned wc = 0;
    check_header_size;
    if (buflen < (unsigned)mod->nHits)
        return -1;
    for (; wc < (unsigned)mod->nHits; ++wc)
        buffer[wc] = (mod->channel[wc] << 24) | mod->count[wc] | 0xf0000000;
    write_header;
}

int CaenV792Unpacker::unpack(unsigned daqVersion,
                             DreamDaqModule *module,
                             const unsigned *evdata)
{
    CaenV792 *mod = dynamic_cast<CaenV792 *>(module);
    assert(mod);

    const unsigned *addr = SubEventSeek(mod->subEventId(), evdata);
    myADCN adcnData;
    int status = DecodeV792AC(addr, &adcnData);

    if (!status)
    {
        if (mod->nAlloc_ < adcnData.index)
        {
            delete [] mod->flag;
            mod->flag = new Char_t[adcnData.index];
        }
        for (unsigned i=0; i<adcnData.index; ++i)
        {
            if (adcnData.ov[i] && adcnData.un[i])
            {
                // Both overflow and underflow bits are set
                status = BADDATA;
                mod->flag[i] = -2;
            }
            else if (adcnData.ov[i])
                mod->flag[i] = 1;
            else if (adcnData.un[i])
                mod->flag[i] = -1;
            else
                mod->flag[i] = 0;
        }
        copy_var_buffer(adcnData.channel, adcnData.data, adcnData.index,
                        &mod->channel, &mod->count, &mod->nAlloc_);
        mod->nHits = adcnData.index;
        mod->EOB = *(addr + sizeof(SubEventHeader)/sizeof(unsigned) +
                     adcnData.index + 1);
    }

    setStatus(status);
    return status;
}

int CaenV792Unpacker::pack(unsigned daqVersion,
                           const DreamDaqModule *module,
                           unsigned *buffer, unsigned buflen)
{
    const CaenV792 *mod = dynamic_cast<const CaenV792 *>(module);
    assert(mod);

    unsigned wc = 0;
    check_header_size;
    if (buflen < (unsigned)mod->nHits+2)
        return -1;
    buffer[wc++] = 0xfa000000 | ((unsigned)mod->nHits << 8);
    for (int i=0; i<mod->nHits; ++wc, ++i)
    {
        buffer[wc] = (mod->channel[i] << 16) | mod->count[i] | 0xf8004000;
        switch (mod->flag[i])
        {
        case -2:
            buffer[wc] |= (3 << 12);
            break;
        case -1:
            buffer[wc] |= (1 << 13);
            break;
        case 1:
            buffer[wc] |= (1 << 12);
            break;
        default:
            break;
        }
    }
    buffer[wc++] = mod->EOB;
    write_header;
}

int TemperatureSensorsUnpacker::unpack(unsigned daqVersion, 
                                       DreamDaqModule *module,
                                       const unsigned *evdata)
{
    TemperatureSensors *mod = dynamic_cast<TemperatureSensors *>(module);
    assert(mod);

    const unsigned *addr = SubEventSeek(mod->subEventId(), evdata);
    myTH03 th03Data;
    const int status = DecodeTH03(addr, &th03Data);

    if (!status)
    {
        copy_var_buffer(th03Data.channel, th03Data.data, th03Data.index,
                        &mod->channel, &mod->data, &mod->nAlloc_);
        mod->nHits = th03Data.index;
        const unsigned *record = addr + sizeof(SubEventHeader)/sizeof(unsigned);
        for (unsigned i=0; i<3; ++i)
            mod->record[i] = record[i];
    }

    setStatus(status);
    return status;
}

int TemperatureSensorsUnpacker::pack(unsigned daqVersion,
                                     const DreamDaqModule *module,
                                     unsigned *buffer, unsigned buflen)
{
    const TemperatureSensors *mod = 
        dynamic_cast<const TemperatureSensors *>(module);
    assert(mod);

    unsigned wc = 0;
    check_header_size;
    if (buflen < 3)
        return -1;
    for (unsigned i=0; i<3; ++i)
        buffer[wc++] = mod->record[i];
    write_header;
}

int LeCroy1176Unpacker::unpack(unsigned daqVersion, 
                               DreamDaqModule *module,
                               const unsigned *evdata)
{
    LeCroy1176 *mod = dynamic_cast<LeCroy1176 *>(module);
    assert(mod);

    const unsigned *addr = SubEventSeek(mod->subEventId(), evdata);
    myTDC tdcData;
    const int status = DecodeL1176(addr, &tdcData);

    if (!status)
    {
        UInt_t sz = mod->nAlloc_;
        copy_var_buffer(tdcData.channel, tdcData.data, tdcData.index,
                        &mod->channel, &mod->count, &sz);
        copy_var_buffer(tdcData.valid, tdcData.edge, tdcData.index,
                        &mod->valid, &mod->edge, &mod->nAlloc_);
        mod->nHits = tdcData.index;
    }

    setStatus(status);
    return status;
}

int LeCroy1176Unpacker::pack(unsigned daqVersion,
                             const DreamDaqModule *module,
                             unsigned *buffer, unsigned buflen)
{
    const LeCroy1176 *mod = dynamic_cast<const LeCroy1176 *>(module);
    assert(mod);

    unsigned wc = 0;
    check_header_size;
    if (buflen < (unsigned)mod->nHits+1)
        return -1;
    buffer[wc++] = 0xDEAAED00;
    for (int i=0; i<mod->nHits; ++wc, ++i)
    {
        unsigned iedge = mod->edge[i] ? 1 : 0;
        unsigned ivalid = mod->valid[i] ? 1 : 0;
        buffer[wc] = mod->count[i] | (iedge << 16) | 
            (mod->channel[i] << 17) | (ivalid << 21);
        if (!(i == mod->nHits - 1))
            buffer[wc] |= (1 << 23);
    }
    write_header;
}

int LeCroy1182Unpacker::unpack(unsigned daqVersion, 
                               DreamDaqModule *module,
                               const unsigned *evdata)
{
    LeCroy1182 *mod = dynamic_cast<LeCroy1182 *>(module);
    assert(mod);

    const unsigned *addr = SubEventSeek(mod->subEventId(), evdata);
    myADC adcData;
    const int status = DecodeL1182(addr, &adcData);

    if (!status)
    {
        copy_var_buffer(adcData.channel, adcData.charge, adcData.index,
                        &mod->channel, &mod->count, &mod->nAlloc_);
        mod->nHits = adcData.index;
    }

    setStatus(status);
    return status;
}

int LeCroy1182Unpacker::pack(unsigned daqVersion,
                             const DreamDaqModule *module,
                             unsigned *buffer, unsigned buflen)
{
    const LeCroy1182 *mod = dynamic_cast<const LeCroy1182 *>(module);
    assert(mod);

    unsigned wc = 0;
    check_header_size;
    if (buflen < (unsigned)mod->nHits+2)
        return -1;
    buffer[wc++] = 0xFCBBCF00;
    for (int i=0; i<mod->nHits; ++i, ++wc)
        buffer[wc] = mod->count[i] | (mod->channel[i] << 12);
    buffer[wc++] = 0xABCCBA00;
    write_header;
}

static void fill_scope_channel(ScopeChannel *ch, const unsigned i,
                               const myTEKOSC& tekoscData)
{
    ch->scale = tekoscData.scale[i];
    ch->position = tekoscData.position[i];
    ch->timeDiff = tekoscData.tdiff[i];
    ch->enabled = tekoscData.chfla[i];
    if (ch->enabled)
    {
        ch->nHits = tekoscData.pts;
        if (ch->nAlloc_ < tekoscData.pts)
        {
            delete [] ch->data;
            ch->data = new Short_t[tekoscData.pts];
            ch->nAlloc_ = tekoscData.pts;
        }
        const int *odata = tekoscData.data + i*tekoscData.pts;
        for (unsigned k=0; k<tekoscData.pts; ++k)
            ch->data[k] = odata[k];
    }
    else
        ch->nHits = 0;
}

int TDS7254BUnpacker::unpack(unsigned daqVersion, 
                             DreamDaqModule *module,
                             const unsigned *evdata)
{
    TDS7254B *mod = dynamic_cast<TDS7254B *>(module);
    assert(mod);

    const unsigned *addr = SubEventSeek(mod->subEventId(), evdata);
    myTEKOSC tekoscData;
    const int status = DecodeTEKOSC(addr, &tekoscData);

    if (!status)
    {
        mod->samplingRate = tekoscData.samplerate;
        for (unsigned i=0; i<4; ++i)
            fill_scope_channel(&mod->ch[i], i, tekoscData);
    }

    setStatus(status);
    return status;
}

int TDS7254BUnpacker::pack(unsigned daqVersion,
                           const DreamDaqModule *module,
                           unsigned *buffer, unsigned buflen)
{
    const TDS7254B *mod = dynamic_cast<const TDS7254B *>(module);
    assert(mod);

    unsigned wc = 0;
    check_header_size;
    if (buflen < sizeof(struct oscheader)/sizeof(unsigned))
        return -1;

    unsigned nPts = 0;
    struct oscheader osch;
    osch.chn = 0;
    osch.chmask = 0;
    osch.pts = 0;
    osch.samrate = mod->samplingRate;
    for (unsigned i=0; i<4; ++i)
    {
        const ScopeChannel *ch = mod->ch + i;
        osch.scale[i] = ch->scale;
        osch.position[i] = ch->position;
        osch.timediff[i] = ch->timeDiff;
        if (ch->enabled)
        {
            ++osch.chn;
            osch.chmask |= (1 << i);
            if (osch.pts == 0)
                osch.pts = ch->nHits;
            nPts += ch->nHits;
        }
    }
    memcpy(buffer+wc, &osch, sizeof(struct oscheader));
    wc += sizeof(struct oscheader)/sizeof(unsigned);
    if (buflen-wc < osch.chn*(nPts/2))
        return -1;

    for (unsigned i=0; i<4; ++i)
    {
        const ScopeChannel *ch = mod->ch + i;
        if (ch->enabled)
        {
            // Assume that the number of points is even
            assert(ch->nHits % 2 == 0);
            Short_t *buf = (Short_t *)(buffer + wc);
            for (int j=0; j<ch->nHits; ++j)
                buf[j] = ch->data[j];
            wc += ch->nHits/2;
        }
    }

    write_header;
}
