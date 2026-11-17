
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
// this file: the ROP (Remote Operation Protocol) of TETHER, version 6, shared by the rpc socket (theRPCmanager), the
// streaming socket (theSTRmanager) and by whoever builds or parses TETHER frames by itself (e.g. the Speedgoat link).
// theTETHERservice puts all together.
//
// wire format (little endian):
//
//   frame = FrameHeader (16 bytes) + numOfROPs ROPs, one after the other, w/ numOfROPs <= maxROPs
//   ROP   = Head (4 bytes) + data[size] + zero padding up to a multiple of 4
//   Head  = { opc, id, uint16 size }
//
//   FrameHeader::layout is 0: variable-length ROPs. a frame w/ any other value (e.g. the uniform slots of version 5)
//   is rejected. a frame is at most maxsizeofframe bytes (1472: a UDP datagram that does not fragment on Ethernet),
//   or less if the board says so in BrowseInfo::maxframesize.
//
//   opc    : bits 0-3 = CMD.
//            bits 4-7 = in ask, set, sig, run, ping: the options. bit 4 (OPTack) = the sender wants an ack / nak
//                       in ack, nak: the CMD that is acknowledged (an ack or a nak never asks for an ack)
//                       in say: 0
//   size   : the number of meaningful bytes of data, at most maxdatasize = 512
//   the padding bytes are written as zero and ignored by the receiver
//
// partition of the IDs:
//   0x00          no ID (e.g. ping<>)
//   0x01 - 0x9F   variables (Dictionary)        IDxxx   ask, set, sig     (159)
//   0xA0 - 0xDF   operations                    OPxxx   run                (64)
//   0xE0 - 0xFF   management, built in          MGxxx   ask, set
//
// an ack is light: it only tells "done". the only ack w/ data is the one of a run whose operation gives back a result
// (at most maxresultsize = 36 bytes). a nak tells "not done" and may carry a short text (at most maxtextsize = 36
// characters, w/out NUL) that tells why. a value travels only in say (the reply to ask) and in sig.
//
// rpc socket:
//   host -> ask<ID>                board -> say<ID, value> or nak<ID, ask, "why">
//   host -> set[+ack]<ID, value>   board -> ack<ID, set> or nak<ID, set, "why">
//   host -> set<ID, value>         no reply
//   host -> sig<ID, value>         no reply (same as set w/out +ack)
//   host -> run[+ack]<OP, arg>     board -> ack<OP, run, result (0 to 36 bytes)> or nak<OP, run, "why">
//   host -> run<OP, arg>           no reply
//   host -> ping<ID>               board -> ack<ID, ping>
//   a CMD that is not valid for the range of its ID gets a nak only if the sender expects a reply (ask or +ack).
//   all the replies to a frame go in one frame. a ROP that expects a reply is executed only if there is room for its
//   reply: else it is not executed and it gets nak<"no room">, or it is counted as dropped if not even that fits.
//   the host knows the sizes from the browse and must not ask more than a frame can answer.
//
// streaming socket:
//   board -> sig<ID, value>        one frame per tick w/ a sig for each streamed variable, to the host that started it
//   host -> ping<ID>               board -> ack<ID, ping>. anything else is discarded

namespace embot::app::eth::rop {

    using ID = uint8_t;

    // 6: variable-length ROPs w/ up to 512 bytes of data, a 4-byte head w/ a 16-bit size, light ack, BrowseInfo w/
    //    the limits of the board
    constexpr uint16_t protocolversion {6};

    enum class CMD : uint8_t { ask = 0, say = 1, set = 2, sig = 3, ack = 4, nak = 5, ping = 6, run = 7, none = 15 };

    // the options, in the high nibble of opc of ask, set, sig, run, ping
    constexpr uint8_t OPTnone {0x0};
    constexpr uint8_t OPTack {0x1};

    // the partition of the IDs
    constexpr ID IDnone {0x00};
    constexpr ID VARfirst {0x01};
    constexpr ID VARlast {0x9F};
    constexpr ID OPfirst {0xA0};
    constexpr ID OPlast {0xDF};
    constexpr ID MGfirst {0xE0};
    constexpr ID MGlast {0xFF};
    constexpr bool isvariable(ID id) { return (IDnone != id) && (id <= VARlast); }
    constexpr bool isop(ID id) { return (id >= OPfirst) && (id <= OPlast); }
    constexpr bool ismg(ID id) { return 0xE0 == (id & 0xE0); }

    // the management IDs, executed by theRPCmanager itself
    // - stream: 0xE0 - 0xE7
    constexpr ID MGstreamADD {0xE0};     // set<MGstreamADD, id>       : adds variable id to the stream. size = 1
    constexpr ID MGstreamREM {0xE1};     // set<MGstreamREM, id>       : removes variable id. size = 1
    constexpr ID MGstreamLIST {0xE2};    // ask<MGstreamLIST>          : say<MGstreamLIST, StreamList>
    constexpr ID MGstreamSTART {0xE3};   // set<MGstreamSTART, ms>     : uint16. 0 = the default period, else the period
                                         //                              in ms, inside [min, max] of theSTRmanager::Config.
                                         //                              the stream goes to the sender of this ROP
                                         // ask<MGstreamSTART>         : say<MGstreamSTART, ms>: the period applied, 0 if stopped
    constexpr ID MGstreamSTOP {0xE4};    // set<MGstreamSTOP>          : stops the stream. size = 0
    // - browse: 0xE8 - 0xEF
    constexpr ID MGbrowseINFO {0xE8};    // ask<MGbrowseINFO>          : say<MGbrowseINFO, BrowseInfo>
    constexpr ID MGbrowseITEM {0xE9};    // ask<MGbrowseITEM, index>   : say<MGbrowseITEM, BrowseItem>, nak past the end
    constexpr ID MGbrowseFIELD {0xEA};   // ask<MGbrowseFIELD, id, n>  : say<MGbrowseFIELD, BrowseField> or nak
    // - system: 0xF0 - 0xF7
    constexpr ID MGversion {0xF0};       // ask<MGversion>             : say<MGversion, Version>
    constexpr ID MGstats {0xF1};         // ask<MGstats>               : say<MGstats, Statistics>
    // - all the others are reserved

    // the sizes
    constexpr size_t headsize {4};
    constexpr uint16_t maxdatasize {512};       // of the value of a variable, of the argument of an operation
    constexpr uint16_t maxresultsize {36};      // of the result of an operation, carried by its ack
    constexpr uint16_t maxtextsize {36};        // of the text of a nak

    // the bytes of s rounded up to a multiple of 4
    constexpr size_t pad4(size_t s) { return (s + 3) & ~size_t {3}; }

    // the bytes taken in a frame by a ROP w/ s bytes of data
    constexpr size_t sizeofROP(size_t s) { return headsize + pad4(s); }

    // the biggest reply that is not a say: an ack w/ a result, or a nak w/ a text. 40 bytes
    constexpr size_t maxsizeofshortreply {sizeofROP((maxresultsize > maxtextsize) ? maxresultsize : maxtextsize)};


    // the 4 bytes at the start of every ROP
    struct Head
    {
        uint8_t opc {static_cast<uint8_t>(CMD::none)};      // CMD in bits 0-3, options or the referred CMD in bits 4-7
        ID id {IDnone};
        uint16_t size {0};                                  // number of meaningful bytes of data

        constexpr Head() = default;
        constexpr explicit Head(uint8_t o, ID i, uint16_t s) : opc(o), id(i), size(s) {}

        // ask, set, sig, run, ping (w/ options) or say (options must be OPTnone)
        static constexpr Head request(CMD c, uint8_t options, ID i, uint16_t s)
        {
            return Head {static_cast<uint8_t>(((options & 0x0F) << 4) | (static_cast<uint8_t>(c) & 0x0F)), i, s};
        }

        // ack or nak: they carry the referred CMD in place of the options
        static constexpr Head reply(CMD c, ID i, CMD referred, uint16_t s)
        {
            return Head {static_cast<uint8_t>(((static_cast<uint8_t>(referred) & 0x0F) << 4) | (static_cast<uint8_t>(c) & 0x0F)), i, s};
        }

        constexpr CMD cmd() const { return static_cast<CMD>(opc & 0x0F); }
        constexpr bool isreply() const { return (CMD::ack == cmd()) || (CMD::nak == cmd()); }
        // the options. 0 for an ack or a nak
        constexpr uint8_t options() const { return isreply() ? 0 : ((opc >> 4) & 0x0F); }
        constexpr bool ackrequired() const { return 0 != (options() & OPTack); }
        // the CMD acknowledged by an ack or a nak. CMD::none for all the others
        constexpr CMD referredcmd() const { return isreply() ? static_cast<CMD>((opc >> 4) & 0x0F) : CMD::none; }
        // the sender expects a reply
        constexpr bool wantsreply() const { return (CMD::ask == cmd()) || ackrequired(); }

        // copies from / to the wire, w/out any alignment requirement (little endian: the platforms of TETHER)
        static Head load(const void *wire)
        {
            Head h {};
            std::memcpy(&h, wire, sizeof(Head));
            return h;
        }

        void store(void *wire) const
        {
            std::memcpy(wire, this, sizeof(Head));
        }
    };

    static_assert(sizeof(Head) == headsize, "Head must be 4 bytes");


    struct FrameHeader
    {
        static constexpr uint32_t signatureOK {0xFEEDC0DE};     // "feed code". on the wire: DE C0 ED FE
        static constexpr uint8_t layoutVariable {0};            // variable-length ROPs (version 6). 8 to 40 was version 5
        static constexpr uint8_t maxROPs {32};

        uint32_t signature {signatureOK};
        uint8_t layout {layoutVariable};
        uint8_t numOfROPs {0};
        uint16_t filler {0};
        uint64_t timeinmicroseconds {0};

        FrameHeader() = default;

        explicit FrameHeader(uint8_t n, uint64_t t)
            : signature(signatureOK), layout(layoutVariable), numOfROPs(n), filler(0), timeinmicroseconds(t) {}
    };

    static_assert(sizeof(FrameHeader) == 16, "FrameHeader must be 16 bytes");

    constexpr size_t sizeofframeheader {sizeof(FrameHeader)};

    // the payload of a UDP datagram that does not fragment on an Ethernet MTU of 1500: 1500 - 20 (IPv4) - 8 (UDP)
    constexpr size_t maxudppayload {1472};

    // the biggest frame. a board may accept and produce smaller frames, down to minsizeofframe: it tells its own limit
    // in BrowseInfo::maxframesize
    constexpr size_t maxsizeofframe {maxudppayload};
    constexpr size_t minsizeofframe {128};

    // the most data that a single ROP can carry in a frame of the biggest size
    static_assert(sizeofframeheader + sizeofROP(maxdatasize) <= maxsizeofframe, "TETHER: a ROP w/ maxdatasize bytes must fit the biggest frame");
    // the smallest frame holds at least two short replies (e.g. a BrowseItem and a nak)
    static_assert(sizeofframeheader + 2 * maxsizeofshortreply <= minsizeofframe, "TETHER: minsizeofframe is too small");


    enum class Error : uint8_t
    {
        none,
        notinitialised,
        nullpayload,
        toosmall,           // less than a FrameHeader
        toobig,             // more than the max size of a frame of the receiver
        wrongsignature,
        wronglayout,        // FrameHeader::layout is not layoutVariable (e.g. a frame of version 5)
        toomanyROPs,
        wrongsize           // the ROPs do not fill the frame exactly, or a ROP goes past its end, or carries > maxdatasize
    };


    // a received ROP: its head and its data, which stay inside the received payload (no copy)
    struct View
    {
        Head head {};
        const uint8_t *data {nullptr};

        View() = default;
        explicit View(const Head &h, const uint8_t *d) : head(h), data(d) {}

        CMD cmd() const { return head.cmd(); }
        ID id() const { return head.id; }
        uint16_t size() const { return head.size; }
        bool ackrequired() const { return head.ackrequired(); }
        bool wantsreply() const { return head.wantsreply(); }
        CMD referredcmd() const { return head.referredcmd(); }

        // it copies the data into a T. use it instead of a reinterpret_cast so that alignment is never an issue.
        // the caller checks that size() == sizeof(T) (or at least >= sizeof(T)). the bytes past size() are not read
        template<typename T>
        T value() const
        {
            static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");
            static_assert(sizeof(T) <= maxdatasize, "T is too big for a ROP");
            T v {};
            const size_t n {(sizeof(T) < head.size) ? sizeof(T) : head.size};
            if((nullptr != data) && (n > 0))
            {
                std::memcpy(&v, data, n);
            }
            return v;
        }
    };


    // it validates a received frame (header and all its ROPs) and then gives its ROPs one after the other.
    // a frame is either valid as a whole or rejected as a whole: nothing of a malformed frame is executed.
    //   Reader reader {payload, size, maxframesize};
    //   if(Error::none != reader.error()) { ... }
    //   View v {};
    //   while(reader.next(v)) { ... }
    class Reader
    {
    public:

        explicit Reader(const void *payload, size_t size, size_t maxsize)
            : _payload(static_cast<const uint8_t *>(payload)), _size(size)
        {
            _error = check(maxsize);
        }

        Error error() const { return _error; }
        const FrameHeader &header() const { return _header; }

        // false at the end, or if the frame is not valid
        bool next(View &view)
        {
            if((Error::none != _error) || (_index >= _header.numOfROPs))
            {
                return false;
            }
            const Head h {Head::load(_payload + _offset)};
            view = View {h, _payload + _offset + headsize};
            _offset += sizeofROP(h.size);
            _index++;
            return true;
        }

    private:

        // the checks of the walk, in order: room for a head, size <= maxdatasize, room for the data and its padding,
        // and at the end the ROPs fill the frame exactly
        Error check(size_t maxsize)
        {
            if(nullptr == _payload)
            {
                return Error::nullpayload;
            }
            if(_size < sizeofframeheader)
            {
                return Error::toosmall;
            }
            if(_size > maxsize)
            {
                return Error::toobig;
            }
            // copy: the payload may not be 8-byte aligned
            std::memcpy(&_header, _payload, sizeofframeheader);
            if(FrameHeader::signatureOK != _header.signature)
            {
                return Error::wrongsignature;
            }
            if(FrameHeader::layoutVariable != _header.layout)
            {
                return Error::wronglayout;
            }
            if(_header.numOfROPs > FrameHeader::maxROPs)
            {
                return Error::toomanyROPs;
            }
            size_t offset {sizeofframeheader};
            for(uint8_t n = 0; n < _header.numOfROPs; n++)
            {
                if((offset + headsize) > _size)
                {
                    return Error::wrongsize;
                }
                const Head h {Head::load(_payload + offset)};
                if(h.size > maxdatasize)
                {
                    return Error::wrongsize;
                }
                if((offset + sizeofROP(h.size)) > _size)
                {
                    return Error::wrongsize;
                }
                offset += sizeofROP(h.size);
            }
            if(offset != _size)
            {
                return Error::wrongsize;
            }
            _offset = sizeofframeheader;
            return Error::none;
        }

        const uint8_t *_payload {nullptr};
        size_t _size {0};
        FrameHeader _header {};
        Error _error {Error::notinitialised};
        size_t _offset {sizeofframeheader};
        uint8_t _index {0};
    };


    // it appends ROPs to a frame in a buffer of capacity bytes (the max size of a frame of the sender), and then
    // closes the frame w/ its header. it never writes past capacity.
    //   Writer writer {buffer, capacity};
    //   writer.add(Head::reply(CMD::ack, id, CMD::set, 0), nullptr);
    //   // or, to fill the data in place:
    //   uint8_t *p {writer.reserve(s)};  if(nullptr != p) { fill p w/ s bytes; writer.commit(Head::request(CMD::say, OPTnone, id, s)); }
    //   size_t size {writer.close(embot::core::now())};
    class Writer
    {
    public:

        explicit Writer(void *buffer, size_t capacity)
            : _buffer(static_cast<uint8_t *>(buffer)), _capacity((nullptr == buffer) ? 0 : capacity)
        {
        }

        // it forgets the ROPs added so far
        void reset()
        {
            _offset = sizeofframeheader;
            _count = 0;
            _reserved = 0;
        }

        uint8_t numberofROPs() const { return _count; }
        // the size of the frame so far, header included
        size_t size() const { return (_capacity >= sizeofframeheader) ? _offset : 0; }

        // true if a ROP w/ s bytes of data can still be added
        bool fits(size_t s) const
        {
            return (_capacity >= sizeofframeheader) && (_count < FrameHeader::maxROPs) && (s <= maxdatasize) &&
                   ((_offset + sizeofROP(s)) <= _capacity);
        }

        // it gives where the s bytes of data of the next ROP go, so that they can be written in place. the padding is
        // already zero. nullptr if it does not fit. the ROP exists only after commit()
        uint8_t *reserve(size_t s)
        {
            _reserved = 0;
            if(false == fits(s))
            {
                return nullptr;
            }
            uint8_t *data {_buffer + _offset + headsize};
            std::memset(data + s, 0, pad4(s) - s);
            _reserved = s;
            return data;
        }

        // it writes the head of the ROP whose data were reserved. head.size must not exceed the reserved bytes: the
        // bytes between head.size and the reserved size are zeroed. false if nothing was reserved
        bool commit(const Head &head)
        {
            if((0 == _reserved) && (false == fits(0)))
            {
                return false;
            }
            if(head.size > _reserved)
            {
                _reserved = 0;
                return false;
            }
            uint8_t *rop {_buffer + _offset};
            std::memset(rop + headsize + head.size, 0, pad4(_reserved) - head.size);
            head.store(rop);
            _offset += sizeofROP(head.size);
            _count++;
            _reserved = 0;
            return true;
        }

        // it appends a whole ROP and gives the address of its data inside the buffer (also when head.size is 0).
        // data == nullptr writes zeros. nullptr if it does not fit
        uint8_t *add(const Head &head, const void *data)
        {
            uint8_t *p {reserve(head.size)};
            if(nullptr == p)
            {
                return nullptr;
            }
            if((nullptr != data) && (head.size > 0))
            {
                std::memcpy(p, data, head.size);
            }
            else
            {
                std::memset(p, 0, head.size);
            }
            return commit(head) ? p : nullptr;
        }

        // it writes the header w/ the number of ROPs and the time, and gives the size of the frame
        size_t close(uint64_t timeinmicroseconds)
        {
            if(_capacity < sizeofframeheader)
            {
                return 0;
            }
            const FrameHeader header {_count, timeinmicroseconds};
            std::memcpy(_buffer, &header, sizeofframeheader);
            return _offset;
        }

    private:
        uint8_t *_buffer {nullptr};
        size_t _capacity {0};
        size_t _offset {sizeofframeheader};
        uint8_t _count {0};
        size_t _reserved {0};
    };


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
        uint16_t maxframesize {maxsizeofframe};         // the max size of a frame that this board accepts and produces
        uint16_t maxdatasize {rop::maxdatasize};        // the max data of a ROP (a variable, the argument of an operation)
        uint32_t crc32 {0};                             // of all the BrowseItem, then the BrowseField of each item, as zlib

        BrowseInfo() = default;
        explicit BrowseInfo(uint8_t nv, uint8_t no, uint8_t mr, uint16_t mf, uint16_t md, uint32_t c)
            : numberofvariables(nv), numberofops(no), maxrops(mr), reserved(0), maxframesize(mf), maxdatasize(md), crc32(c) {}
    };

    struct BrowseItem                                   // MGbrowseITEM: index in [0, numberofvariables + numberofops)
    {
        static constexpr uint8_t kindVariable {1};
        static constexpr uint8_t kindOP {2};
        static constexpr size_t namesize {maxnamesize};

        // the kind of the result of an operation
        static constexpr uint8_t resultnone {0};        // no result (size 0), or raw bytes (size > 0)
        static constexpr uint8_t resultscalar {1};      // a named scalar (or array): 1 field
        static constexpr uint8_t resultstruct {2};      // a struct: numberofresultfields fields

        uint8_t index {0};
        ID id {IDnone};
        uint8_t kind {0};
        uint8_t access {0};                             // a variable: SharedVariables::Access. an operation: 0
        uint8_t type {0};                               // SharedVariables::Type, of the variable or of the argument
        uint8_t numberoffields {0};                     // a variable: > 0 only for Type::structure. an operation: 1 for
                                                        // a named scalar (or array) argument, n for a struct, 0 if none
        uint8_t numberofresultfields {0};               // an operation: as numberoffields, for the result. else 0
        uint8_t resultkind {resultnone};                // an operation: resultnone, resultscalar, resultstruct. else 0
        uint16_t size {0};                              // of the variable, or of the argument of the operation
        uint16_t resultsize {0};                        // an operation: of its result, at most maxresultsize. else 0
        char name[namesize] {0};                        // NUL padded, not NUL terminated if 28 chars long

        // the fields of MGbrowseFIELD: first the ones of the variable (or of the argument), then the ones of the result
        constexpr uint8_t totalfields() const { return static_cast<uint8_t>(numberoffields + numberofresultfields); }

        BrowseItem() = default;
        explicit BrowseItem(uint8_t idx, ID i, uint8_t k, uint8_t a, uint8_t t, uint8_t nf, uint8_t nrf, uint8_t rk,
                            uint16_t s, uint16_t rs, const char *n)
            : index(idx), id(i), kind(k), access(a), type(t), numberoffields(nf), numberofresultfields(nrf), resultkind(rk),
              size(s), resultsize(rs)
        {
            for(size_t j = 0; (nullptr != n) && (j < namesize) && (0 != n[j]); j++)
            {
                name[j] = n[j];
            }
        }
    };

    struct BrowseField                                  // MGbrowseFIELD: a field of a variable, or of the argument or the result of an operation
    {
        static constexpr size_t namesize {maxnamesize};

        ID id {IDnone};
        uint8_t n {0};                                  // in [0, BrowseItem::totalfields())
        uint8_t type {0};                               // SharedVariables::Type, a scalar
        uint8_t reserved {0};
        uint16_t count {0};                             // > 1 for an array
        uint16_t offset {0};                            // from the start of the variable (or of the argument, of the result)
        char name[namesize] {0};

        BrowseField() = default;
        explicit BrowseField(ID i, uint8_t nn, uint8_t t, uint16_t c, uint16_t o, const char *nm)
            : id(i), n(nn), type(t), reserved(0), count(c), offset(o)
        {
            for(size_t j = 0; (nullptr != nm) && (j < namesize) && (0 != nm[j]); j++)
            {
                name[j] = nm[j];
            }
        }
    };

    static_assert(sizeof(StreamList) == 1 + FrameHeader::maxROPs, "StreamList must be 1 + maxROPs bytes");
    static_assert(sizeof(Version) == 8, "Version must be 8 bytes");
    static_assert(sizeof(BrowseInfo) == 12, "BrowseInfo must be 12 bytes");
    static_assert(sizeof(Statistics) == 36, "Statistics must be 36 bytes");
    static_assert(sizeof(BrowseItem) == 40, "BrowseItem must be 40 bytes");
    static_assert(sizeof(BrowseField) == 36, "BrowseField must be 36 bytes");
    // the management values are small: their say fits any frame of a board
    static_assert(sizeofframeheader + sizeofROP(sizeof(BrowseItem)) <= minsizeofframe, "the management values must fit the smallest frame");

} // namespace embot::app::eth::rop


#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
