
/*
 * Copyright (C) 2026 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_ROP_H_
#define __EMBOT_APP_ETH_ROP_H_

#include "embot_core.h"
#include "embot_net_eth.h"
#include <cstring>
#include <type_traits>


// TETHER: the link between a host and a board, for diagnostics and accessory operations. a lightweight service over
// UDP for RPC, streaming and remote execution, with a self-describing Dictionary of variables and operations.
//
// this file: the ROP (Remote Operation Protocol) of TETHER, version 5, shared by the rpc socket (theRPCmanager) and
// the streaming socket (theSTRmanager). theTETHERservice puts all together.
//
// wire format (little endian):
//
//   frame = FrameHeader (16 bytes) + numOfROPs slots of sizeofROP bytes each, w/ numOfROPs <= FrameHeader::maxROPs
//   slot  = { opc, id, size, refcmd } + data[sizeofROP - 4]
//
//   sizeofROP is chosen by the sender of each frame: a multiple of 8 in [8, 40], the smallest one that holds the
//   biggest ROP of the frame. all the slots of a frame have the same size, so the parsing is O(1), and a frame of
//   small values stays small. a ROP carries at most ROP::datasize = 36 bytes.
//
//   opc    : bits 0-3 = CMD, bits 4-7 = options. bit 4 (OPTack) = the sender wants an ack / nak. bits 5-7 = 0
//   size   : the number of meaningful bytes of data
//   refcmd : in a ack / nak it tells which received CMD is acknowledged. in all other ROPs it is 0
//
// partition of the IDs:
//   0x00          no ID (e.g. ping<>)
//   0x01 - 0x9F   variables (Dictionary)        IDxxx   ask, set, sig     (159)
//   0xA0 - 0xDF   operations                    OPxxx   run                (64)
//   0xE0 - 0xFF   management, built in          MGxxx   ask, set
//
// rpc socket:
//   host -> ask<ID>                board -> say<ID, value> or nak<ID, ask, "why">
//   host -> set[+ack]<ID, value>   board -> ack<ID, set, value now held> or nak<ID, set, "why">
//   host -> set<ID, value>         no reply
//   host -> sig<ID, value>         no reply (same as set w/out +ack)
//   host -> run[+ack]<OP, arg>     board -> ack<OP, run, result> or nak<OP, run, "why">
//   host -> run<OP, arg>           no reply
//   host -> ping<ID>               board -> ack<ID, ping>
//   a CMD that is not valid for the range of its ID gets a nak only if the sender expects a reply (ask or +ack)
//
// nak: its data, if any, is a text w/out NUL (size = number of characters, at most ROP::datasize = 36) that tells why.
// the board writes it (an operation w/ Result::fail("..."), or the managers w/ their own standard texts)
//
// streaming socket:
//   board -> sig<ID, value>        one frame per tick w/ a sig for each streamed variable, to the host that started it
//   host -> ping<ID>               board -> ack<ID, ping>. anything else is discarded

namespace embot::app::eth::rop {

    using ID = uint8_t;

    // 5: a nak carries a text. the browse describes also the result of an operation
    constexpr uint16_t protocolversion {5};

    struct ROP
    {
        enum class CMD : uint8_t { ask = 0, say = 1, set = 2, sig = 3, ack = 4, nak = 5, ping = 6, run = 7, none = 15 };

        // options, in the high nibble of opc
        static constexpr uint8_t OPTnone {0x0};
        static constexpr uint8_t OPTack {0x1};

        // partition of the IDs
        static constexpr ID IDnone {0x00};
        static constexpr ID VARfirst {0x01};
        static constexpr ID VARlast {0x9F};
        static constexpr ID OPfirst {0xA0};
        static constexpr ID OPlast {0xDF};
        static constexpr ID MGfirst {0xE0};
        static constexpr ID MGlast {0xFF};
        static constexpr bool isvariable(ID id) { return (IDnone != id) && (id <= VARlast); }
        static constexpr bool isop(ID id) { return (id >= OPfirst) && (id <= OPlast); }
        static constexpr bool ismg(ID id) { return 0xE0 == (id & 0xE0); }

        // management IDs, executed by theRPCmanager itself
        // - stream: 0xE0 - 0xE7
        static constexpr ID MGstreamADD {0xE0};     // set<MGstreamADD, id>       : adds variable id to the stream. size = 1
        static constexpr ID MGstreamREM {0xE1};     // set<MGstreamREM, id>       : removes variable id. size = 1
        static constexpr ID MGstreamLIST {0xE2};    // ask<MGstreamLIST>          : say<MGstreamLIST, StreamList>
        static constexpr ID MGstreamSTART {0xE3};   // set<MGstreamSTART, ms>     : uint16. 0 = the default period, else the period
                                                    //                              in ms, inside [min, max] of theSTRmanager::Config.
                                                    //                              the stream goes to the sender of this ROP.
                                                    //                              the ack carries the period applied, in ms
                                                    // ask<MGstreamSTART>         : say<MGstreamSTART, ms>, 0 if stopped
        static constexpr ID MGstreamSTOP {0xE4};    // set<MGstreamSTOP>          : stops the stream. size = 0
        // - browse: 0xE8 - 0xEF
        static constexpr ID MGbrowseINFO {0xE8};    // ask<MGbrowseINFO>          : say<MGbrowseINFO, BrowseInfo>
        static constexpr ID MGbrowseITEM {0xE9};    // ask<MGbrowseITEM, index>   : say<MGbrowseITEM, BrowseItem>, nak past the end
        static constexpr ID MGbrowseFIELD {0xEA};   // ask<MGbrowseFIELD, id, n>  : say<MGbrowseFIELD, BrowseField> or nak
        // - system: 0xF0 - 0xF7
        static constexpr ID MGversion {0xF0};       // ask<MGversion>             : say<MGversion, Version>
        static constexpr ID MGstats {0xF1};         // ask<MGstats>               : say<MGstats, Statistics>
        // - all the others are reserved

        static constexpr size_t headersize {4};
        static constexpr size_t datasize {36};
        static constexpr size_t maxsizeofROP {headersize + datasize};   // 40
        static constexpr size_t minsizeofROP {8};

        // the size of the slot that holds a ROP w/ s bytes of data: 4 + s rounded up to a multiple of 8
        static constexpr uint8_t slotsize(size_t s) { return static_cast<uint8_t>((headersize + s + 7) & ~size_t {7}); }
        static constexpr bool isvalidslotsize(size_t s) { return (s >= minsizeofROP) && (s <= maxsizeofROP) && (0 == (s & 7)); }

        uint8_t opc {static_cast<uint8_t>(CMD::none)};  // CMD in bits 0-3, options in bits 4-7
        ID id {IDnone};
        uint8_t size {0};                               // number of meaningful bytes inside data[]
        uint8_t refcmd {0};                             // used by ack / nak
        uint8_t data[datasize] {0};

        ROP() = default;

        explicit ROP(CMD c, uint8_t options, ID i, uint8_t s, const void *d, CMD r)
        {
            load(c, options, i, s, d, r);
        }

        // it copies s bytes from d into data[], so d does not need to stay alive after the call.
        // it returns false and leaves the ROP as CMD::none if s > datasize
        bool load(CMD c, uint8_t options, ID i, uint8_t s, const void *d, CMD r)
        {
            clear();
            if(s > datasize)
            {
                return false;
            }
            opc = static_cast<uint8_t>(((options & 0x0F) << 4) | (static_cast<uint8_t>(c) & 0x0F));
            id = i;
            size = s;
            refcmd = ((CMD::ack == c) || (CMD::nak == c)) ? (static_cast<uint8_t>(r) & 0x0F) : 0;
            if((nullptr != d) && (s > 0))
            {
                std::memcpy(data, d, s);
            }
            return true;
        }

        void clear()
        {
            opc = static_cast<uint8_t>(CMD::none);
            id = IDnone;
            size = 0;
            refcmd = 0;
            std::memset(data, 0, datasize);
        }

        CMD cmd() const { return static_cast<CMD>(opc & 0x0F); }
        uint8_t options() const { return (opc >> 4) & 0x0F; }
        bool ackrequired() const { return 0 != (options() & OPTack); }
        CMD referredcmd() const { return static_cast<CMD>(refcmd & 0x0F); }

        // it copies data[] into a T. use it instead of a reinterpret_cast so that alignment is never an issue
        template<typename T>
        T value() const
        {
            static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");
            static_assert(sizeof(T) <= datasize, "T is too big for a ROP");
            T v {};
            std::memcpy(&v, data, sizeof(T));
            return v;
        }
    };

    static_assert(sizeof(ROP) == ROP::maxsizeofROP, "ROP must have no padding");


    struct FrameHeader
    {
        static constexpr uint32_t signatureOK {0xFEEDC0DE};     // "feed code". on the wire: DE C0 ED FE
        static constexpr uint8_t maxROPs {32};                  // see the static_asserts after maxsizeofframe

        uint32_t signature {signatureOK};
        uint8_t sizeofROP {ROP::minsizeofROP};                  // the size of every slot of the frame
        uint8_t numOfROPs {0};
        uint16_t filler {0};
        uint64_t timeinmicroseconds {0};

        FrameHeader() = default;

        explicit FrameHeader(uint8_t sizeofrop, uint8_t n, uint64_t t)
            : signature(signatureOK), sizeofROP(sizeofrop), numOfROPs(n), filler(0), timeinmicroseconds(t) {}
    };

    static_assert(sizeof(FrameHeader) == 16, "FrameHeader must be 16 bytes");

    constexpr size_t maxsizeofframe {sizeof(FrameHeader) + FrameHeader::maxROPs * ROP::maxsizeofROP};   // 1296

    // the payload of a UDP datagram that does not fragment on an Ethernet MTU of 1500: 1500 - 20 (IPv4) - 8 (UDP)
    constexpr size_t maxudppayload {1472};

    // MGstreamLIST gives count + one ID per streamed variable in the data of a ROP
    static_assert(FrameHeader::maxROPs <= ROP::maxsizeofROP - ROP::headersize - 1,
                  "TETHER: FrameHeader::maxROPs is too big: MGstreamLIST (1 byte of count + one ID per variable) must fit the data of a ROP");
    // a frame must fit a UDP datagram, or it is fragmented
    static_assert(maxsizeofframe <= maxudppayload,
                  "TETHER: FrameHeader::maxROPs is too big: FrameHeader + maxROPs * maxsizeofROP does not fit the payload of a UDP datagram");


    enum class Error : uint8_t { none, notinitialised, nullpayload, toosmall, wrongsignature, wrongsizeofROP, toomanyROPs, wrongsize };

    // it validates a received frame and copies its header
    inline Error checkframe(const void *payload, size_t size, FrameHeader &header)
    {
        if(nullptr == payload)
        {
            return Error::nullpayload;
        }
        if(size < sizeof(FrameHeader))
        {
            return Error::toosmall;
        }
        // copy: the payload may not be 8-byte aligned
        std::memcpy(&header, payload, sizeof(FrameHeader));
        if(FrameHeader::signatureOK != header.signature)
        {
            return Error::wrongsignature;
        }
        if(false == ROP::isvalidslotsize(header.sizeofROP))
        {
            return Error::wrongsizeofROP;
        }
        if(header.numOfROPs > FrameHeader::maxROPs)
        {
            return Error::toomanyROPs;
        }
        if(size != (sizeof(FrameHeader) + header.numOfROPs * static_cast<size_t>(header.sizeofROP)))
        {
            return Error::wrongsize;
        }
        return Error::none;
    }

    // it copies slot n of a frame already validated by checkframe() into rop, w/ the bytes past the slot zeroed.
    // the ROP is malformed if rop.size > header.sizeofROP - ROP::headersize
    inline void extract(const void *payload, const FrameHeader &header, uint8_t n, ROP &rop)
    {
        rop.clear();
        const uint8_t *slot {static_cast<const uint8_t *>(payload) + sizeof(FrameHeader) + n * static_cast<size_t>(header.sizeofROP)};
        std::memcpy(&rop, slot, header.sizeofROP);
    }


    // the names of variables and operations: [A-Za-z_][A-Za-z0-9_]*, 1 to 28 chars. they are the keys of the host
    constexpr size_t maxnamesize {28};

    constexpr bool isvalidname(const char *name)
    {
        if(nullptr == name)
        {
            return false;
        }
        size_t n {0};
        for(; (n <= maxnamesize) && (0 != name[n]); n++)
        {
            const char c {name[n]};
            const bool letter {((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z')) || ('_' == c)};
            const bool digit {(c >= '0') && (c <= '9')};
            if((false == letter) && (false == (digit && (n > 0))))
            {
                return false;
            }
        }
        return (n > 0) && (n <= maxnamesize);
    }

    constexpr bool samename(const char *a, const char *b)
    {
        for(size_t i = 0; i <= maxnamesize; i++)
        {
            if(a[i] != b[i])
            {
                return false;
            }
            if(0 == a[i])
            {
                return true;
            }
        }
        return true;
    }


    // the values carried by the management IDs

    struct StreamList                                   // MGstreamLIST
    {
        uint8_t count {0};
        ID ids[FrameHeader::maxROPs] {0};

        StreamList() = default;
    };

    struct Version                                      // MGversion
    {
        uint16_t protocol {protocolversion};
        uint16_t reserved {0};
        uint32_t application {0};                       // the version of the application, as given to the service. 0 = not given

        Version() = default;
        explicit Version(uint16_t p, uint32_t a) : protocol(p), reserved(0), application(a) {}
    };

    struct Statistics                                   // MGstats: the counters of theRPCmanager and theSTRmanager
    {
        uint32_t rpcrxframes {0};
        uint32_t rpcrxwrongframes {0};
        uint32_t rpcrxROPs {0};
        uint32_t rpcdroppedROPs {0};
        uint32_t strtxframes {0};
        uint32_t strreadfailures {0};
        uint32_t strrxpings {0};
        uint32_t strrxdiscarded {0};
        uint32_t strkeepalivestops {0};

        Statistics() = default;
    };

    struct BrowseInfo                                   // MGbrowseINFO
    {
        uint8_t numberofvariables {0};
        uint8_t numberofops {0};
        uint8_t maxrops {FrameHeader::maxROPs};         // the max number of ROPs of a frame of this board
        uint8_t reserved {0};
        uint32_t crc32 {0};                             // of all the BrowseItem, then the BrowseField of each item, as zlib

        BrowseInfo() = default;
        explicit BrowseInfo(uint8_t nv, uint8_t no, uint8_t mr, uint32_t c) : numberofvariables(nv), numberofops(no), maxrops(mr), reserved(0), crc32(c) {}
    };

    struct BrowseItem                                   // MGbrowseITEM: index in [0, numberofvariables + numberofops)
    {
        static constexpr uint8_t kindVariable {1};
        static constexpr uint8_t kindOP {2};
        static constexpr size_t namesize {28};

        // the result of an operation, packed in the byte `access`: bits 0-5 = its size, bits 6-7 = its kind
        static constexpr uint8_t resultnone {0};        // no result (size 0), or raw bytes (size > 0)
        static constexpr uint8_t resultscalar {1};      // a named scalar (or array): 1 field
        static constexpr uint8_t resultstruct {2};      // a struct: numberofresultfields fields
        static constexpr uint8_t resultinfo(uint8_t size, uint8_t kind) { return static_cast<uint8_t>((kind << 6) | (size & 0x3F)); }
        static constexpr uint8_t resultsizeof(uint8_t info) { return info & 0x3F; }
        static constexpr uint8_t resultkindof(uint8_t info) { return info >> 6; }

        uint8_t index {0};
        ID id {ROP::IDnone};
        uint8_t kind {0};
        uint8_t size {0};                               // of the variable, or of the argument of the operation
        uint8_t access {0};                             // a variable: SharedVariables::Access. an operation: resultinfo()
        uint8_t type {0};                               // SharedVariables::Type, of the variable or of the argument
        uint8_t numberoffields {0};                     // a variable: > 0 only for Type::structure. an operation: 1 for
                                                        // a named scalar (or array) argument, n for a struct, 0 if none
        uint8_t numberofresultfields {0};               // an operation: as numberoffields, for the result. else 0
        char name[namesize] {0};                        // NUL padded, not NUL terminated if 28 chars long

        // the fields of MGbrowseFIELD: first the ones of the variable (or of the argument), then the ones of the result
        constexpr uint8_t totalfields() const { return static_cast<uint8_t>(numberoffields + numberofresultfields); }

        BrowseItem() = default;
        explicit BrowseItem(uint8_t idx, ID i, uint8_t k, uint8_t s, uint8_t a, uint8_t t, uint8_t nf, uint8_t nrf, const char *n)
            : index(idx), id(i), kind(k), size(s), access(a), type(t), numberoffields(nf), numberofresultfields(nrf)
        {
            for(size_t j = 0; (nullptr != n) && (j < namesize) && (0 != n[j]); j++)
            {
                name[j] = n[j];
            }
        }
    };

    struct BrowseField                                  // MGbrowseFIELD: a field of a variable, or of the argument or the result of an operation
    {
        static constexpr size_t namesize {28};

        ID id {ROP::IDnone};
        uint8_t n {0};                                  // in [0, BrowseItem::totalfields())
        uint8_t type {0};                               // SharedVariables::Type, a scalar
        uint8_t count {0};                              // > 1 for an array
        uint8_t offset {0};                             // from the start of the variable (or of the argument)
        uint8_t reserved[3] {0};
        char name[namesize] {0};

        BrowseField() = default;
        explicit BrowseField(ID i, uint8_t nn, uint8_t t, uint8_t c, uint8_t o, const char *nm)
            : id(i), n(nn), type(t), count(c), offset(o)
        {
            for(size_t j = 0; (nullptr != nm) && (j < namesize) && (0 != nm[j]); j++)
            {
                name[j] = nm[j];
            }
        }
    };

    static_assert(sizeof(StreamList) == 1 + FrameHeader::maxROPs, "StreamList must be 1 + maxROPs bytes");
    static_assert(sizeof(Version) == 8, "Version must be 8 bytes");
    static_assert(sizeof(BrowseInfo) == 8, "BrowseInfo must be 8 bytes");
    static_assert(sizeof(Statistics) == ROP::datasize, "Statistics must fill a ROP");
    static_assert(sizeof(BrowseItem) == ROP::datasize, "BrowseItem must fill a ROP");
    static_assert(sizeof(BrowseField) == ROP::datasize, "BrowseField must fill a ROP");

} // namespace embot::app::eth::rop


#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
