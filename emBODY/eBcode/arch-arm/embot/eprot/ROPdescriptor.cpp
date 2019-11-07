
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


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------



bool embot::eprot::rop::Descriptor::load(embot::utils::Data &stream, uint16_t &consumed)
{
    reset();
    consumed = 0;
    if(false == stream.isvalid())
    {
        return false;
    }
    
    if(stream.capacity < Header::sizeofobject)
    {
        return false;
    }
    
    Header *rophead = reinterpret_cast<Header*>(stream.pointer);
    
    // now i need to see how big the ropstream is expected to be
    size_t expectedsize = Stream::capacityfor(rophead->opc, rophead->datasize, rophead->ctrl.getPLUS());
    if((expectedsize > stream.capacity) || (0 != (rophead->datasize % 4))) // it was < ...
    {
        return false;
    }
    
    // ok, we can consume the bytes
    consumed = expectedsize;
    
    // and we fill the fields of the Descriptor
    opcode = rophead->opc;
    id32 = rophead->id32;
    value.capacity = rophead->datasize;
    value.pointer = (0 == value.capacity) ? nullptr : (stream.getU08ptr() + sizeof(Header));
    uint16_t offset_time = 0;
    if(rophead->ctrl.isPLUSsignature())
    {
        offset_time = sizeof(signature);
        std::memmove(&signature, stream.getU08ptr() + sizeof(Header) + rophead->datasize, sizeof(signature));
    }
    if(rophead->ctrl.isPLUStime())
    {
        std::memmove(&time, stream.getU08ptr() + sizeof(Header) + rophead->datasize + offset_time, sizeof(time));
    }  
    
    rophead->ctrl.extract(plus, rqst, conf);
    
    return true;  
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

