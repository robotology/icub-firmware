
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - brief
//   it contains types of the eth protocol used inside icub: the rop 
//

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_EPROT_ROP_H_
#define _EMBOT_EPROT_ROP_H_

#include "embot_common.h"
#include "embot_utils.h"
#include "embot_eprot.h"


    // description
    // a rop is formed by [header]-[<optional-payload>]
    // the header is formed by 8 bytes: header = [ [ctlr]-[opc]-[datasize]-[id32] ] 
    // the presence of the optional payload depends on header.ctrl and header.opc. 
    // the optional payload, if present, is formed by the following bytes:    
    // optional-payload = [optional-data]-[optiona-signature]-[optiona-time]
    // its size is: header.datasize bytes (if data is present) + 4 bytes (if signature is present) + 8 bytes (if time is present)
    
    
namespace embot { namespace eprot { namespace rop {
      
 
    // here are some types used inside a ROP, plus manipulating functions.
    
    
    // some simple types
    
    enum class OPC : uint8_t { none = 0, ask = 1, say = 2, set = 3, sig = 4, any = 7 };    
    using SIGN = std::uint32_t;
    using TIME = embot::common::Time;
    constexpr SIGN signatureNone = 0xffffffff;
    static_assert(sizeof(TIME) == 8, "embot::eprot::rop::TIME must have 8 size");

    enum class CONF : uint8_t { none = 0, nack = 1, ack = 2 };
    enum class PLUS : uint8_t { none = 0, signature = 1, time = 2, signaturetime = 3 }; 
    enum class RQST : uint8_t { none = 0, time = 1, confirmation = 2, timeconfirmation = 3 }; 

      
    // functions which manipulate the simple types
    
    constexpr PLUS getPLUS(bool hassignature, bool hastime) 
    { 
        constexpr PLUS tbl_bool2plus[2][2] = {{PLUS::none, PLUS::time}, {PLUS::signature, PLUS::signaturetime}};
        return tbl_bool2plus[hassignature?1:0][hastime?1:0]; 
    }
    
    constexpr RQST getRQST(bool reqconf, bool reqtime) 
    { 
        constexpr RQST tbl_bool2rqst[2][2] = {{RQST::none, RQST::time}, {RQST::confirmation, RQST::timeconfirmation}};
        return tbl_bool2rqst[reqconf?1:0][reqtime?1:0]; 
    }

    constexpr bool hasSIGN(PLUS p)
    {
        constexpr bool tbl_plus2sign[4] = { false, true, false, true };
        return tbl_plus2sign[embot::common::tointegral(p)];
    }
    
    constexpr bool hasTIME(PLUS p)
    {
        constexpr bool tbl_plus2tim[4] = { false, false, true, true };
        return tbl_plus2tim[embot::common::tointegral(p)];
    }
    
    constexpr size_t capacityfor(PLUS p)
    {
        constexpr uint8_t tbl_size4plus[4] = {0, sizeof(SIGN), sizeof(embot::common::Time), sizeof(SIGN)+sizeof(embot::common::Time)};
        return tbl_size4plus[embot::common::tointegral(p)];
    }
    
    constexpr bool hasdata(OPC o, CONF c = CONF::none)
    {   // if the confirmation is CONF::ack or CONF::nack then there is no data irrespectively of the opcode
        constexpr bool tbl_opc2bool[8] = {false, false, true, true, true, false, false, true};
        return (CONF::none == c) ? (tbl_opc2bool[embot::common::tointegral(o)]) : false;
    }
    
    constexpr size_t normalisedsizeofdata(size_t s)
    {   // size must always be multiple of 4
        s = (s + 3)/4;
        s = 4*s;
        return s;
    }

    constexpr bool reqCONF(RQST r)
    {
        constexpr bool tbl_rqst2conf[4] = { false, false, true, true };
        return tbl_rqst2conf[embot::common::tointegral(r)];
    }

    constexpr bool reqTIME(RQST r)
    {
        constexpr bool tbl_rqst2time[4] = { false, true, false, true };
        return tbl_rqst2time[embot::common::tointegral(r)];
    }
    
    
    // aggregate types organised in struct. 
    // they all have the relevant constexpr static size_t sizeofobject variable and a static assert because ... 
    // 1. they must be that size
    // 2. they must have that memory layout
    
    struct Header
    {
        struct Format
        {   // spec document uses CTRL. we use Format, which better describes its role because CTRL is sic... a macro contained inside ttydefaults.h
            constexpr static uint8_t versionzero = 0;
            
            // if i use "uint8_t confinfo : 2{0};" i have a warning: default member initializer for bit-field is a C++2a extension [-Wc++2a-extensions]
            
            // -> memory layout
            uint8_t confinfo : 2;// {0};           // it is formed by bits 0 and 1. it contains confirmation info as in CONF
            uint8_t plustime : 1;// {0};           // if 1 the ROP has the time field of 8 bytes
            uint8_t plussign : 1;// {0};           // if 1 the ROP has the signature field of 4 bytes 
            uint8_t rqsttime : 1;// {0};           // if 1 the ROP requests that a reply shall have the time field
            uint8_t rqstconf : 1;// {0};           // if 1 the ROP requests that the received shall send confirmation */
            uint8_t version  : 2;// {versionzero}; // it is formed by bits 7 and 6 of the first byte of the ROP. So far it must be versionzero      
            // <- memory layout
                       
            Format() : version(versionzero) { set(PLUS::none); set(RQST::none); set(CONF::none); }
            
            constexpr Format(PLUS p, RQST r, CONF c) : confinfo(0), plustime(0), plussign(0), rqsttime(0), rqstconf(0), version(versionzero) { set(p); set(r); set(c); }
                            
            constexpr void set(PLUS p) { plustime = hasTIME(p); plussign = hasSIGN(p); }
            constexpr void set(RQST r) { rqsttime = reqTIME(r); rqstconf = reqCONF(r); }
            constexpr void set(CONF c) { confinfo = embot::common::tointegral(c); }
                        
            constexpr PLUS getPLUS() const { return embot::eprot::rop::getPLUS(plussign, plustime); }
            constexpr RQST getRQST() const { return embot::eprot::rop::getRQST(rqstconf, rqsttime); }
            constexpr CONF getCONF() const { return (3 == confinfo) ? CONF::none : static_cast<CONF>(confinfo); }             
            
            constexpr bool isPLUStime() const { return plustime; }
            constexpr bool isPLUSsignature() const { return plussign; }           
            constexpr bool hasRQSTtime() const { return rqsttime; }
            constexpr bool hasRQSTconfirmation() const { return rqstconf; }
            
            constexpr void fill(PLUS p = PLUS::none, RQST r = RQST::none, CONF c = CONF::none) { set(p); set(r); set(c); }
            constexpr void extract(PLUS& p, RQST& r, CONF &c) const { p = getPLUS(); r = getRQST(); c = getCONF(); } 
            constexpr static size_t sizeofobject = 1;
        }; static_assert(sizeof(Format) == Format::sizeofobject, "embot::eprot::rop::Header::Format has wrong size. it must be 1");
                

        // -> memory layout   
        Format      fmt {};
        OPC         opc {OPC::none};
        uint16_t    datasize {0};
        ID32        id32;
        // <- memory layout
        
        
        Header() = default;        
        constexpr Header(const Format &f, OPC o, uint16_t s, ID32 i) : fmt(f), opc(o), datasize(s), id32(i) {}
        
        constexpr static size_t sizeofobject = 8;
    }; static_assert(sizeof(Header) == Header::sizeofobject, "embot::eprot::rop::Header has wrong size. it must be 8");    
    
}}} // namespace embot { namespace eprot { namespace rop {



namespace embot { namespace eprot { namespace rop {
    
    // in here are the objects which we use to describe a rop or to represent the binary stream or ...
    // they dont need to have an exact memory layout 
    
    struct Descriptor
    {
        OPC                 opcode {OPC::none};             
        embot::eprot::ID32  id32 {embot::eprot::ID32none};  
        embot::utils::Data  value {nullptr, 0};             
        SIGN                signature {signatureNone};
        embot::common::Time time {embot::common::timeNone};
        CONF                conf {CONF::none};
        PLUS                plus {PLUS::none};
        RQST                rqst {RQST::none};

        Descriptor() = default;
        constexpr Descriptor(OPC o, embot::eprot::ID32 i, const embot::utils::Data &v, SIGN s = signatureNone, embot::common::Time t = embot::common::timeNone,
                             PLUS p = PLUS::none, RQST r = RQST::none, CONF c = CONF::none) :
                             opcode(o), id32(i), value(v), signature(s), time(t), plus(p), rqst(r), conf(c) { }
        constexpr bool isvalid() const { return ((embot::eprot::ID32none != id32) && (OPC::none != opcode) && (value.isvalid())); }

        constexpr bool hassignature() const { return hasSIGN(plus); }
        constexpr bool hastime() const { return hasTIME(plus); }

        void reset() { 
            opcode = OPC::none; id32 = embot::eprot::ID32none; value = {nullptr, 0}; signature = signatureNone; time = embot::common::timeNone; 
            conf = CONF::none; plus = PLUS::none; rqst = RQST::none; 
        }
        
        void fill(OPC o, embot::eprot::ID32 i, const embot::utils::Data &v, SIGN s = signatureNone, embot::common::Time t = embot::common::timeNone,
                  PLUS p = PLUS::none, RQST r = RQST::none, CONF c = CONF::none) {
            opcode = o; id32 = i; value = v; signature = s; time = t; plus = p; rqst = r; conf = c;
        }
        
        bool load(embot::utils::Data &stream, uint16_t &consumed);
    };
    
    using fpOnROP = bool (*)(const embot::eprot::IPv4 &ipv4, const embot::eprot::rop::Descriptor &rop);
      
       

    class Stream 
    {
    public:
               
        Stream(size_t capacity);  
        ~Stream();

        constexpr static size_t minimumsize = Header::sizeofobject; 
    
        constexpr static size_t capacityfor(OPC opc, size_t sizeofdata, PLUS plus = PLUS::none, CONF conf = CONF::none)
        {
            return Header::sizeofobject + (embot::eprot::rop::hasdata(opc, conf) ? embot::eprot::rop::normalisedsizeofdata(sizeofdata) : 0) + embot::eprot::rop::capacityfor(plus);
        }
        
        constexpr static size_t capacityfor(const Descriptor &des)
        {
            return Header::sizeofobject + (embot::eprot::rop::hasdata(des.opcode, des.conf) ? embot::eprot::rop::normalisedsizeofdata(des.value.capacity) : 0) + embot::eprot::rop::capacityfor(des.plus);
        }
        
        size_t getcapacity() const;

        bool load(const Descriptor &des);
        bool update(const embot::utils::Data &data, const embot::eprot::rop::SIGN signature = embot::eprot::rop::signatureNone, const embot::common::Time time = embot::common::timeNone);
        
        // direct pointer
        bool retrieve(uint8_t **data, size_t &size);      

    private:    
        struct Impl;
        Impl *pImpl;
    };  



}}} // namespace embot { namespace eprot { namespace rop {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
