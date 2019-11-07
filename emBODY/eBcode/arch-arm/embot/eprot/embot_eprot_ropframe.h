
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - brief
//   it contains types of the eth protocol used inside icub: the rop 
//

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_EPROT_ROPFRAME_H_
#define _EMBOT_EPROT_ROPFRAME_H_

#include "embot_common.h"
#include "embot_utils.h"
#include "embot_eprot_rop.h"


    // description
    // a ropframe is formed by [header]-[<optional-body>]-[footer]
    // the header is formed by 24 bytes: header = [STARTCODE]-[sizeofbody]-[numberofrops]-[sequencenumber]-[ageofframe] 
    // the presence of the optional body depends on header.sizeofbody.
    // the body contains a number of rops specified by header.numerofrops aligned one after another
    // in stream format.
    // the footer is just a marker at the end of the ropframe
    

namespace embot { namespace eprot { namespace ropframe {

    
    // aggregate types organised in struct. 
    // they all have the relevant constexpr static size_t sizeofobject variable and a static assert because ... 
    // 1. they must be that size
    // 2. they must have that memory layout
    
    struct Header
    {
        constexpr static uint32_t STARTCODE = 0x12345678; 

        // -> memory layout
        uint32_t startcode {STARTCODE};
        uint16_t sizeofbody {0};
        uint16_t numberofrops {0};
        uint64_t sequencenumber {0};
        embot::common::Time ageofframe {0};
        // <- memory layout
        
        Header() = default;
            
        void reset() { startcode = STARTCODE; sizeofbody = 0; numberofrops = 0; sequencenumber = 0; ageofframe = 0; }
        void add_rop(uint16_t ropstreamsize) { numberofrops++; sizeofbody += ropstreamsize; }
        void set_age(embot::common::Time a) { ageofframe = a; }
        embot::common::Time get_age() const { return ageofframe; }
        void set_seq(uint64_t s) { sequencenumber = s; }
        uint64_t get_seq() const { return sequencenumber; }
        void inc_seq() { sequencenumber++; }
        
        constexpr bool isvalid() const { return STARTCODE == startcode; }
        
        constexpr static size_t sizeofobject = 24;
    }; static_assert(sizeof(Header) == Header::sizeofobject, "embot::eprot::ropframe::Header has wrong size. it must be 24");
    
    
    struct Footer
    {
        constexpr static uint32_t STOPCODE = 0x87654321; 
        
        // -> memory layout
        uint32_t stopcode {STOPCODE};
        // <- memory layout
        
        Footer() = default;
        void refresh() { stopcode = STOPCODE; }
        bool isvalid() const { return STOPCODE == stopcode; }
               
        constexpr static size_t sizeofobject = 4;
    }; static_assert(sizeof(Footer) == Footer::sizeofobject, "embot::eprot::rop::Footer has wrong size. it must be 4");
     
    
}}} // namespace embot { namespace eprot { namespace ropframe {



namespace embot { namespace eprot { namespace ropframe {   
 
    // in here are the objects which we use to describe a ropframe or to represent the binary stream or ...
    // they dont need to have an exact memory layout. they are:    
    // 1. Former: it can be used in former mode only. it uses an empty ropframe and adds to it the required ROPs
    // 2. Parser: it can be used in parser mode only. it accepts a full ropframe and parses the ROPs inside

    
   
    // ropframe::Former - description:
    // a. it is created as an empty shell to which we can load() external storage or unload() it.
    // b. we can format() the storage to be a valid ropframe w/ zero rops
    // c. we can pushback() to it a ropstream or a rop::Descriptor
    // d. we can set time and sequance number
    // e. we retrieve the frame
    // todo? add: get(tim, set) isvalid()
    
    class Former
    {
    public:
            
        Former();
        ~Former();

        bool load(const embot::utils::Data &storage);
        bool load(const embot::utils::Data &storage, embot::eprot::rop::Stream *rstream);
        bool unload();   
        bool pushback(const embot::utils::Data &ropstream, uint16_t &availablespace);             
        bool pushback(const embot::eprot::rop::Descriptor &ropdes, uint16_t &availablespace);
        bool set(embot::common::Time tim, uint64_t seq);    
        uint16_t getNumberOfROPs() const;    
        bool get(embot::utils::Data& ropframe) const;    
        bool format();
    
    private:    
        struct Impl;
        Impl *pImpl;         
    };
    
    
    // ropframe::Parser - description:
    // a. it is created as an empty shell to which we can load() a received ropframe and later unload() it.
    // b. we can check: if it isvalid(), its size, number of rops etc. 
    // c. we can parse all the ROPs inside by calling a user-defined callback
    
    class Parser
    {
    public:
            
        Parser();
        ~Parser();

        bool load(const embot::utils::Data &ropframe);
        bool unload();
        // const getters
        bool isvalid() const;         
        uint16_t getSize() const;
        uint16_t getNumberOfROPs() const;
        uint16_t sizeofROPS() const;
        embot::common::Time getTime() const;
        uint64_t getSequenceNumber() const;           
        bool parse(const embot::eprot::IPv4 &ipv4, embot::eprot::rop::fpOnROP onrop, uint16_t &numberofprocessed);        
                       
    private:    
        struct Impl;
        Impl *pImpl;     
    };
    

}}} // namespace embot { namespace eprot { namespace rop {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
