
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_eprot_rop.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <algorithm>

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


struct embot::eprot::rop::Stream::Impl
{    
    uint8_t *stream {nullptr};
    Header *ref2header {nullptr};
    uint8_t *ref2data {nullptr};
    size_t capacityofstream {0};
    Descriptor descriptor {};
    size_t sizeofstream {0};
    
    Impl(size_t capacity)
    {
        if(capacity < Stream::minimumsize)
        {
            capacity = Stream::minimumsize;
        }
        capacityofstream = (capacity+3)/4;
        capacityofstream *= 4;
        stream = new uint8_t[capacityofstream];
        
        ref2header = reinterpret_cast<Header*>(stream);
        if(capacityofstream > Stream::minimumsize)
        {
            ref2data = stream + sizeof(Header);
        }
    }   

    ~Impl()
    {
        delete[] stream;
        // DONT delete ref2header or ref2data
        //constexpr bool force = true;
        //deinit(force);
    }
    
    void fillheader(Header *header, const embot::eprot::rop::Descriptor &des)
    {        
        header->fmt.fill(des.plus, RQST::none, CONF::none);
        header->opc = des.opcode;
        header->datasize = (des.value.capacity+3)/4;
        header->datasize *= 4;
        header->id32 = des.id32;
    }

    bool load(const Descriptor &des)
    {
        size_t required = Stream::capacityfor(des.opcode, des.value.capacity, des.plus);
        
        if(required > capacityofstream)
        {
            return false;
        }
        
        descriptor = des;
        sizeofstream = required;
        
        // ok, now i shape the memory of the ropstream
        
        // header
        fillheader(ref2header, des);
        
        // data
        if(des.value.isvalid())
        {   
            uint16_t nbytes = std::min(ref2header->datasize, static_cast<uint16_t>(des.value.capacity));
            std::memmove(ref2data, des.value.pointer, nbytes);
        }
        
        // signature
        if(des.hassignature())
        {
            std::memmove(ref2data+ref2header->datasize, &des.signature, sizeof(des.signature));
        }   
        
        // time
        if(des.hastime())
        {   // DONT attempt to use uint64_t* from ref2data+ref2header->datasize+4 and do a direct assignment because the memory is not guarantted to be 8-aligned.
            std::memmove(ref2data+ref2header->datasize+4, &des.time, sizeof(des.time));
        }
                     
        return true;
    }
    
    
    bool retrieve(uint8_t **data, size_t &size)
    {
        if(nullptr == data)
        {
            return false;
        }
        
        *data = stream;        
        size = sizeofstream;
        return true;
    }
 
    bool update(const embot::utils::Data &data, const embot::eprot::rop::SIGN signature = embot::eprot::rop::signatureNone, const embot::common::Time time = embot::common::timeNone)
    {
        bool r = false;
        
        if(data.isvalid())
        {
            uint16_t nbytes = std::min(ref2header->datasize, static_cast<uint16_t>(data.capacity));
            std::memmove(ref2data, data.pointer, nbytes);   
            r = true;
        }
    
        if((ref2header->fmt.isPLUSsignature()))
        {
            std::memmove(ref2data+ref2header->datasize, &signature, sizeof(signature));
            r = true;
        } 
        
        if((ref2header->fmt.isPLUStime()))
        {
            std::memmove(ref2data+ref2header->datasize+4, &time, sizeof(time));
            r = true;
        }
        
        return r;
    }
    
    size_t getcapacity() const
    {
        return capacityofstream;
    }

};


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------



embot::eprot::rop::Stream::Stream(size_t capacity)
: pImpl(new Impl(capacity))
{ 

}

embot::eprot::rop::Stream::~Stream()
{   
    delete pImpl;
}

bool embot::eprot::rop::Stream::load(const Descriptor &des)
{
    return pImpl->load(des);
}

bool embot::eprot::rop::Stream::update(const embot::utils::Data &data, const embot::eprot::rop::SIGN signature, const embot::common::Time time)
{
   return pImpl->update(data, signature, time);
}

bool embot::eprot::rop::Stream::retrieve(uint8_t **data, size_t &size)
{
    return pImpl->retrieve(data, size);
}    

size_t embot::eprot::rop::Stream::getcapacity() const
{
    return pImpl->getcapacity();
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

