
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_eprot_ropframe.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <algorithm>

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

#warning MEMO: redefine new / delete / new[] / delete[] operators fo the eth boards

namespace embot { namespace eprot { namespace ropframe {
    
struct coreImpl
{    
    uint8_t *theframe {nullptr};
    size_t capacityoftheframe {0};
    size_t sizeoftheframe {0};

    Header *ref2header {nullptr};
    uint8_t *ref2body {nullptr};
    uint8_t *ref2bodyend {nullptr};
    Footer *ref2footer {nullptr};    
    
    embot::eprot::rop::Stream *_ropstream2 {nullptr};
    
    constexpr static uint16_t minimumsize = Header::sizeofobject + Footer::sizeofobject;

    coreImpl()
    {
    }   

    ~coreImpl()
    {
    }

    uint16_t availablebytes() const
    {
        if(nullptr == ref2header)
        {
            return 0;
        }
        int a = capacityoftheframe - minimumsize - ref2header->sizeofbody;
        return (a>0) ? a : 0;
    }
    
    bool load(const embot::utils::Data &frame, embot::eprot::rop::Stream *rstream)
    {
        _ropstream2 = rstream;
       return load(frame, true);        
    }


    bool load(const embot::utils::Data &frame, bool andformat = true)
    {
        if((!frame.isvalid()) || (frame.capacity < minimumsize))
        {
            return false;
        }
       
        theframe = reinterpret_cast<uint8_t*>(frame.pointer);
        capacityoftheframe = frame.capacity;
        ref2header = reinterpret_cast<Header*>(theframe);
        sizeoftheframe = minimumsize + ref2header->sizeofbody;
        ref2body = theframe + Header::sizeofobject;

        if(false == andformat)
        {
            if((minimumsize + ref2header->sizeofbody) > frame.capacity)
            {
                unload();
                return false;
            }

            ref2bodyend = ref2body + ref2header->sizeofbody;
            ref2footer = reinterpret_cast<Footer*>(ref2bodyend);
            if(!ref2footer->isvalid())
            {
                unload();
                return true;
            }
        }
        else
        {
            // ok, now i format the memory of the roframe
            format();
        }
                                    
        return true;
    }

    bool unload()
    {
        theframe = nullptr;
        capacityoftheframe = 0;
        sizeoftheframe = 0;
        ref2header = nullptr;
        ref2body = nullptr;
        ref2bodyend = nullptr;
        ref2footer = nullptr;
        
        _ropstream2 = nullptr;

        return true;
    }

    bool format()
    {
        if(nullptr == ref2header)
        {
            return false;
        }

        ref2header->reset();
        sizeoftheframe = minimumsize;
        ref2bodyend = ref2body + ref2header->sizeofbody;
        ref2footer = reinterpret_cast<Footer*>(ref2bodyend);
        ref2footer->refresh();
        
        return true;
    }

    bool isvalid() const
    {
        if((nullptr == ref2header) || (nullptr == ref2footer))
        {
            return false;
        }

        // else i must have a well formatted ropframe. i check that        
        return ref2header->isvalid() && ref2footer->isvalid();
    }


    uint16_t getSize() const
    {
        if(nullptr == ref2header)
        {
            return 0;
        }  
        return minimumsize + ref2header->sizeofbody;       
    }

    uint16_t getNumberOfROPs() const
    {
        if(nullptr == ref2header)
        {
            return 0;
        }  
        return ref2header->numberofrops;     
    }

    uint16_t getSizeOfROPs() const
    {
        if(nullptr == ref2header)
        {
            return 0;
        }  
        return ref2header->sizeofbody;   
    }


    bool pushback(const embot::utils::Data &ropstream, uint16_t &availablespace)
    {
        if(nullptr == ref2header)
        {
            availablespace = 0;
            return false;
        } 

        if(!ropstream.isvalid())
        {
            availablespace = availablebytes();
            return false;
        }

        // can i add extra ropstream.capacity bytes?
        if((ropstream.capacity + ref2header->sizeofbody + minimumsize) > capacityoftheframe)
        {
            availablespace = availablebytes();
            return false;
        }

        // yes, i can
 
        std::memmove(ref2bodyend, ropstream.pointer, ropstream.capacity);        
        ref2header->add_rop(ropstream.capacity);

        ref2bodyend = ref2body + ref2header->sizeofbody;
        ref2footer = reinterpret_cast<Footer*>(ref2bodyend);
        ref2footer->refresh();

        return true;
    }

    bool pushback(const embot::eprot::rop::Descriptor &ropdes, uint16_t &availablespace)
    {
        if((nullptr == ref2header) || (nullptr == _ropstream2))
        {
            availablespace = 0;
            return false;
        } 
        
        size_t cap = _ropstream2->capacityfor(ropdes);
        // can the _ropstream2 host it?
        if(cap > _ropstream2->getcapacity())
        {
            return false;
        }
        
        // can this object host it?
        if((cap + ref2header->sizeofbody + minimumsize) > capacityoftheframe)
        {
            availablespace = availablebytes();
            return false;
        }
               
        if(false == _ropstream2->load(ropdes))
        {
            availablespace = availablebytes();
            return false;            
        }
        
        uint8_t *pointer = nullptr;
        size_t size = 0;
        _ropstream2->retrieve(&pointer, size);

        std::memmove(ref2bodyend, pointer, size);        
        ref2header->add_rop(size);

        ref2bodyend = ref2body + ref2header->sizeofbody;
        ref2footer = reinterpret_cast<Footer*>(ref2bodyend);
        ref2footer->refresh();

        return true;
    }

    bool setTime(embot::common::Time t)
    {
        if(nullptr == ref2header)
        {
            return false;
        }
        ref2header->set_age(t); 
        return true;
    }
    
    embot::common::Time getTime() const
    {
        if(nullptr == ref2header)
        {
            return 0;
        }
        return ref2header->get_age(); 
    }

    bool setSequenceNumber(uint64_t s)
    {
        if(nullptr == ref2header)
        {
            return false;
        }
        ref2header->set_seq(s); 
        return true;
    }

    bool incSequenceNumber()
    {
        if(nullptr == ref2header)
        {
            return false;
        }
        ref2header->inc_seq(); 
        return true;
    }

    uint64_t getSequenceNumber() const
    {
        if(nullptr == ref2header)
        {
            return 0;
        }
        return ref2header->get_seq();         
    }


    bool parse(const embot::eprot::IPv4 &ipv4, embot::eprot::rop::fpOnROP onrop, uint16_t &numberofprocessed)
    {
        numberofprocessed = 0;

        if(nullptr == onrop)
        {
            return false;
        }

        if(nullptr == ref2header)
        {
            return false;
        }

        size_t nbytes = 0;
        embot::eprot::rop::Descriptor des{};
        uint8_t *body = ref2body;
        uint16_t avail = ref2header->sizeofbody; 
        for(auto i=0; i<ref2header->numberofrops; i++)
        {
            uint16_t consumed = 0;
            embot::utils::Data stream(body, avail);
            if(false == des.load(stream, consumed))
            {
                break;
            }
            
            numberofprocessed += consumed;
            body += consumed;            
            onrop(ipv4, des);
        }

        return true;
    }

    bool get(embot::utils::Data& frame, uint16_t &capacity) const
    {
        frame.capacity = getSize();
        frame.pointer = theframe;
        capacity = capacityoftheframe;
        return true;
    }

};


}}} // namespace embot { namespace eprot { namespace ropframe {


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


// --- ropframe which only decodes

struct embot::eprot::ropframe::Parser::Impl : public embot::eprot::ropframe::coreImpl
{ 
};

embot::eprot::ropframe::Parser::Parser()
: pImpl(new Impl())
{ 
}

embot::eprot::ropframe::Parser::~Parser()
{   
    delete pImpl;
}

bool embot::eprot::ropframe::Parser::load(const embot::utils::Data &frame)
{
    return pImpl->load(frame, false);
}

bool embot::eprot::ropframe::Parser::unload()
{
    return pImpl->unload();
}

bool embot::eprot::ropframe::Parser::parse(const embot::eprot::IPv4 &ipv4, embot::eprot::rop::fpOnROP onrop, uint16_t &numberofprocessed)
{
    return pImpl->parse(ipv4, onrop, numberofprocessed);
}

uint64_t  embot::eprot::ropframe::Parser::getSequenceNumber() const
{
    return pImpl->getSequenceNumber();
}

bool embot::eprot::ropframe::Parser::isvalid() const
{
    return pImpl->isvalid();
}

uint16_t embot::eprot::ropframe::Parser::getNumberOfROPs() const
{
    return pImpl->getNumberOfROPs();
}

// --- ropframe which only encodes

struct embot::eprot::ropframe::Former::Impl : public embot::eprot::ropframe::coreImpl
{  
};

embot::eprot::ropframe::Former::Former()
: pImpl(new Impl())
{ 
}

embot::eprot::ropframe::Former::~Former()
{   
    delete pImpl;
}

bool embot::eprot::ropframe::Former::load(const embot::utils::Data &frame)
{
    return pImpl->load(frame, true);
}

bool embot::eprot::ropframe::Former::load(const embot::utils::Data &frame, embot::eprot::rop::Stream *rstream)
{
    return pImpl->load(frame, rstream);
}

bool embot::eprot::ropframe::Former::unload()
{
    return pImpl->unload();
}

bool embot::eprot::ropframe::Former::format()
{
    return pImpl->format();
}

bool embot::eprot::ropframe::Former::set(embot::common::Time tim, uint64_t seq)
{
    return pImpl->setSequenceNumber(seq) && pImpl->setTime(tim);
}


bool embot::eprot::ropframe::Former::get(embot::utils::Data& frame) const
{
    uint16_t capacity = 0;
    return pImpl->get(frame, capacity);
}

uint16_t embot::eprot::ropframe::Former::getNumberOfROPs() const
{
    return pImpl->getNumberOfROPs();
}

bool embot::eprot::ropframe::Former::pushback(const embot::utils::Data &ropstream, uint16_t &availablespace)
{
    return pImpl->pushback(ropstream, availablespace);
}

bool embot::eprot::ropframe::Former::pushback(const embot::eprot::rop::Descriptor &ropdes, uint16_t &availablespace)
{
    return pImpl->pushback(ropdes, availablespace);
}


// --- ropframe which does everything

#if 0   
namespace embot { namespace eprot { namespace ropframe {  

    // 3. ROPframe: it can be uses in parser mode or former mode.

    // the ROPframe can be uses in two modes: in encode mode or in decode mode
    // 1.   encode mode
    //      the object is created as an empty shell. 
    //      we give to it storage with load() using andformat = true.
    //      we add as many ropstreams / rops we need w/ calls to pushback() 
    //      when it is time to transmit the ropframe, we retrieve it w/ get() which copies into external ram
    //      then we format the internal data w/ format() so that we can put new rops inside
    // 2.   decode mode
    //      the object is created as an empty shell.
    //      when a udp packet is received, we load it with load({udp.data, udp.size}, andformat = false)
    //      we get info using the const getters (isvalid, getsize, etc.)
    //      if valid, and if there are rops inside: we parse with parse() where each rop is processed by
    //      function oprop(); 
    

    class ROPframe
    {

    public:

        constexpr static uint16_t minimumsize = Header::sizeofobject + Footer::sizeofobject;

        ROPframe(); // empty shell
        ~ROPframe();

        bool load(const embot::utils::Data &frame, bool andformat = true); // it loads a received udp packet 
        bool unload();

        bool format(); // it formats the object to have a legal header-footer and zero rops. 

        // const getters
        bool isvalid() const; // tells if data is well formatted (w/ zero or more rops)    
        uint16_t getSize() const;
        uint16_t getNumberOfROPs() const;    
        uint16_t getSizeOfROPs() const;
        embot::common::Time getTime() const;
        uint64_t getSequenceNumber() const;
    
        bool get(embot::utils::Data& frame, uint16_t &capacity) const; // copies the whole frame. e.g., for its tx
    
        // basic setters: time + sequencenumber
        bool setTime(embot::common::Time t);
        bool setSequenceNumber(uint64_t s);
        bool incSequenceNumber();
        
        // pushback a ropstream to the frame
        bool pushback(const embot::utils::Data &ropstream, uint16_t &availablespace);

        // add a ropdescriptor to the frame
//        bool append(const embot::eprot::rop::Descriptor &ropdescr, uint16_t &availablespace);

        // internally extracts all the rop descriptors in the body and calls onrop() for each of them 
        bool parse(const embot::eprot::IPv4 &ipv4, embot::eprot::rop::fpOnROP onrop, uint16_t &numberofprocessed);


//        // i must now extract one rop at a time. i dont want to give a rop number to extract because it would be heavy to compute their positions beforehands
//        // hence, i use extractstart() and extract()
//        bool extractstart(uint16_t &remainingrops, uint16_t &remainingbytes);

//        // it return true if extraction was ok. for the last rop it returns true and remainingrops = 0 and remainingbytes = 0.
//        // it returns false if it is called when no more rops or data are extacted OR IF the Descriptor is not valid.
//        bool extract(embot::eprot::rop::Descriptor &ropdescr, uint16_t &remainingrops, uint16_t &remainingbytes); // retrieve the next ropdes

//        //bool append(const embot::eprot::rop::Descriptor &ropdescr, uint16_t &availablespace);   // append a single rop

//        // bool append(const ROPframe &rhv, uint16_t &availablespace); // append all the rops inside rhv


    private:    
        struct Impl;
        Impl *pImpl;
    };

}}} // namespace embot { namespace eprot { namespace ropframe {  
#endif

#if 0
struct embot::eprot::ropframe::ROPframe::Impl : public embot::eprot::ropframe::coreImpl
{
};


embot::eprot::ropframe::ROPframe::ROPframe()
: pImpl(new Impl())
{ 

}

embot::eprot::ropframe::ROPframe::~ROPframe()
{   
    delete pImpl;
}

bool embot::eprot::ropframe::ROPframe::load(const embot::utils::Data &frame, bool andformat)
{
    return pImpl->load(frame, andformat);
}

bool embot::eprot::ropframe::ROPframe::unload()
{
    return pImpl->unload();
}


bool embot::eprot::ropframe::ROPframe::format()
{
    return pImpl->format();
}

bool embot::eprot::ropframe::ROPframe::parse(const embot::eprot::IPv4 &ipv4, embot::eprot::rop::fpOnROP onrop, uint16_t &numberofprocessed)
{
    return pImpl->parse(ipv4, onrop, numberofprocessed);
}

uint64_t  embot::eprot::ropframe::ROPframe::getSequenceNumber() const
{
    return pImpl->getSequenceNumber();
}

bool  embot::eprot::ropframe::ROPframe::setSequenceNumber(uint64_t s)
{
    return pImpl->setSequenceNumber(s);
}

bool embot::eprot::ropframe::ROPframe::isvalid() const
{
    return pImpl->isvalid();
}

bool embot::eprot::ropframe::ROPframe::get(embot::utils::Data& frame, uint16_t &capacity) const
{
    return pImpl->get(frame, capacity);
}


bool embot::eprot::ropframe::ROPframe::setTime(embot::common::Time t)
{
    return pImpl->setTime(t);
}

uint16_t embot::eprot::ropframe::ROPframe::getNumberOfROPs() const
{
    return pImpl->getNumberOfROPs();
}

bool embot::eprot::ropframe::ROPframe::pushback(const embot::utils::Data &ropstream, uint16_t &availablespace)
{
    return pImpl->pushback(ropstream, availablespace);
}
#endif

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

