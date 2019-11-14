
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "DiagnosticsNode.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#warning MEMO: implement embot::sys::now() for the eth boards
    
#include "EOVtheSystem.h"

#include "embot_eprot_rop.h"
#include "embot_eprot_ropframe.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::app::DiagnosticsNode::Impl
{    
    Config config {};    
    bool initted {false}; 

    embot::eprot::ropframe::Former *_ropframeformer {nullptr};
    // used by _ropframeformer
    uint8_t *ropframedata {nullptr};
    embot::utils::Data buffer {nullptr, 0};
    uint8_t *bufferdata {nullptr};
    size_t buffercapacity {0};
    uint64_t sequencenumber {0};
    // used by _ropframeformer
    embot::eprot::rop::Stream *_ropstream {nullptr};
   
    // preformed ropstream for Info & InfoBasic     
    embot::eprot::rop::Stream *ropstr_info {nullptr};
    embot::eprot::rop::Stream *ropstr_infobasic {nullptr};
    
    
    Impl() = default;   

    ~Impl()
    {
        constexpr bool force = true;
        deinit(force);
    }

    bool deinit(bool force = false)
    {
        if(!force)
        {
            if(!initted)
            {
                return false;
            }
        }

        if(nullptr != bufferdata)
        {
            delete[] bufferdata;
            bufferdata = nullptr;
            buffercapacity = 0;
        }

        if(nullptr != ropframedata)
        {
            delete[] ropframedata;
            ropframedata = nullptr;
        }

        if(nullptr != _ropframeformer)
        {
            //eo_ropframe_Delete(ropframe);
            delete _ropframeformer;
            _ropframeformer = nullptr;
        }

        if(nullptr != ropstr_info)
        {
            delete ropstr_info;
            ropstr_info = nullptr;
        }       
        
        if(nullptr != ropstr_infobasic)
        {
            delete ropstr_infobasic;
            ropstr_infobasic = nullptr;
        }
        
        if(nullptr != _ropstream)
        {
            delete _ropstream;
            _ropstream =  nullptr;
        }

        initted = false;

        return true;
    }

    bool init(const Config &c)
    {
        if(initted)
        {
            return false;
        }
       
        if(false == c.isvalid())
        {
            return false;
        }
        
        config = c;
        _ropframeformer = new embot::eprot::ropframe::Former; // empty shell
        uint16_t nbytes4frame = config.ropframecapacity;
        ropframedata = new uint8_t[nbytes4frame];
        // give memory to the empty shell, so that it contains the header-ropspace-footer + a service ropstream
        _ropstream = new embot::eprot::rop::Stream(config.singleropstreamcapacity);
        _ropframeformer->load({ropframedata, nbytes4frame}, _ropstream);         
        // and give memory to my buffer
        buffercapacity = nbytes4frame;
        bufferdata = new uint8_t[buffercapacity];
        buffer.clear(); // so that it is not valid
        
        
        constexpr embot::eprot::rop::PLUS plusMODE = embot::eprot::rop::PLUS::none;
        constexpr embot::eprot::rop::SIGN signature = embot::eprot::rop::signatureNone;
        constexpr embot::common::Time time = embot::common::timeNone;
        
        // also, i need a predefined rop able to host the sig<> of a full embot::eprot::diagnostics::InfoBasic w/out signature and time    
        ropstr_infobasic = new embot::eprot::rop::Stream(embot::eprot::rop::Stream::capacityfor(embot::eprot::rop::OPC::any, embot::eprot::diagnostics::InfoBasic::sizeofobject, plusMODE));         
        constexpr embot::eprot::rop::Descriptor sig_infobasic {
            embot::eprot::rop::OPC::sig, 
            embot::eprot::diagnostics::InfoBasic::id32, 
            embot::utils::Data{nullptr, embot::eprot::diagnostics::InfoBasic::sizeofobject},
            signature, time,
            plusMODE
        };      
        ropstr_infobasic->load(sig_infobasic);
        

        // also, i need a predefined rop able to host the sig<> of a full embot::eprot::diagnostics::Info w/out signature and time    
        ropstr_info = new embot::eprot::rop::Stream(embot::eprot::rop::Stream::capacityfor(embot::eprot::rop::OPC::any, embot::eprot::diagnostics::Info::sizeofobject, plusMODE));         
        constexpr embot::eprot::rop::Descriptor sig_info {
            embot::eprot::rop::OPC::sig, 
            embot::eprot::diagnostics::Info::id32, 
            embot::utils::Data{nullptr, embot::eprot::diagnostics::Info::sizeofobject},
            signature, time,
            plusMODE
        };      
        ropstr_info->load(sig_info);        
        
        initted = true;
        return true;
    }

    bool add(const embot::utils::Data &ropstream)
    {
        if(!initted)
        {
            return false;
        }

        if(!ropstream.isvalid())
        {
            return false;
        }

        uint16_t availspace = 0;
        return _ropframeformer->pushback(ropstream, availspace);
    }


    bool add(const embot::eprot::rop::Descriptor &ropdes)
    {
        if(!initted)
        {
            return false;
        }    

        if(!ropdes.isvalid())
        {
            return false;
        }        
          
        uint16_t availspace = 0;
        return _ropframeformer->pushback(ropdes, availspace);
        return false;
    }
    

    bool add(const embot::eprot::diagnostics::InfoBasic &infobasic)
    {
        if(!initted)
        {
            return false;
        }        
        // i need a pre-former rop (or ropstream) where to just add the infobasic stuff
        embot::utils::Data da{const_cast<embot::eprot::diagnostics::InfoBasic*>(&infobasic), embot::eprot::diagnostics::InfoBasic::sizeofobject};
        //embot::utils::Data da{&infobasic, embot::eprot::diagnostics::InfoBasic::size};
        static uint32_t sig = 0;
        ropstr_infobasic->update(da, sig++);
        
        uint8_t *strm = nullptr;
        size_t ss = 0;
        ropstr_infobasic->retrieve(&strm, ss);

        uint16_t availspace = 0;
        return _ropframeformer->pushback({strm, ss}, availspace);
    }

    bool add(const embot::eprot::diagnostics::Info &info)
    {
        if(!initted)
        {
            return false;
        } 
        
        // i need a pre-former rop (or ropstream) where to just add the infobasic stuff
        embot::utils::Data da{const_cast<embot::eprot::diagnostics::Info*>(&info), embot::eprot::diagnostics::Info::sizeofobject};
        static uint32_t sig = 0;
        ropstr_info->update(da, sig++);
        
        uint8_t *strm = nullptr;
        size_t ss = 0;
        ropstr_info->retrieve(&strm, ss);

        uint16_t availspace = 0;
        return _ropframeformer->pushback({strm, ss}, availspace);
    }
    
    bool prepare(size_t &sizeofropframe)
    {
        if(!initted)
        {
            return false;
        }

        // prepare the ropframe. but only if it is not empty. 
        // after preparation clear it

        sizeofropframe = 0;
#if 1
        if(0 == _ropframeformer->getNumberOfROPs())
        {
            return false;
        }   

        eOabstime_t timenow = eov_sys_LifeTimeGet(eov_sys_GetHandle());
        _ropframeformer->set(timenow, sequencenumber++);
        
        // copy into buffer
        embot::utils::Data fr {};
        _ropframeformer->get(fr); 
        std::memmove(bufferdata, fr.pointer, fr.capacity); 
        buffer.load(bufferdata, fr.capacity); // now buffer is valid ...        
        
        sizeofropframe = fr.capacity;
        // format
        _ropframeformer->format();
            
        return true;
        
#else
        if(0 == eo_ropframe_ROP_NumberOf(ropframe))
        {   // dont transmit: we dont have rops
            return false;
        }
        
        // if we have rops ...
        // 1. add a valid timestamp
        // 2. increment the sequence number
        // 3. copy the ropframe inside a buffer
        // 4. and finally clear the ropframe so that it will be able to load new rops
        // 1.
        eOabstime_t timenow = eov_sys_LifeTimeGet(eov_sys_GetHandle());
        eo_ropframe_age_Set(ropframe, timenow);
        
        // 2.
        eo_ropframe_seqnum_Set(ropframe, sequencenumber);
        sequencenumber++;
        
        // 3.
        uint8_t *fdata = NULL;
        uint16_t fsize = 0;
        uint16_t fcapacity = 0;
        
        eo_ropframe_Get(ropframe, &fdata, &fsize, &fcapacity);
        // fcapacity is surely bigger equal than buffercapacity (i allocated them myself), hence i dont check
        // copy the ropframe into the buffer
        std::memmove(bufferdata, fdata, fsize); 
        buffer.load(bufferdata, fsize); // now buffer is valid ...
        
        // 4.
        eo_ropframe_Clear(ropframe);  

        sizeofropframe = fsize;
        return true;
#endif        
    }

    bool retrieve(embot::utils::Data &datainropframe)
    {
        if(!initted)
        {
            return false;
        }

        if(!datainropframe.isvalid())
        {
            return false;
        }

        // in here i could lock buffer ..

        if(!buffer.isvalid())
        {
            return false;
        }

        if(datainropframe.capacity < buffer.capacity)
        {
            return false;
        }

        // i am ok: i can copy buffer into ropframe

        datainropframe.capacity = buffer.capacity;
        std::memmove(datainropframe.pointer, buffer.pointer, buffer.capacity); 

        // ok, i could unlock

        return true;
    }

};


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------



embot::app::DiagnosticsNode::DiagnosticsNode()
: pImpl(new Impl)
{ 
}

embot::app::DiagnosticsNode::~DiagnosticsNode()
{   
    delete pImpl;
}


bool embot::app::DiagnosticsNode::initted() const
{
    return pImpl->initted;
}


bool embot::app::DiagnosticsNode::init(const Config &config)
{       
    if(true == initted())
    {
        return false;
    }

    return pImpl->init(config);        
}


bool embot::app::DiagnosticsNode::add(const embot::utils::Data &ropstream)
{
    return pImpl->add(ropstream);
}

bool embot::app::DiagnosticsNode::add(const embot::eprot::rop::Descriptor &ropdes)
{
    return pImpl->add(ropdes);
}

bool embot::app::DiagnosticsNode::add(const embot::eprot::diagnostics::InfoBasic &infobasic)
{
    return pImpl->add(infobasic);    
}


bool embot::app::DiagnosticsNode::prepare(size_t &sizeofropframe)
{
    return pImpl->prepare(sizeofropframe);
}


bool embot::app::DiagnosticsNode::retrieve(embot::utils::Data &datainropframe)
{
    return pImpl->retrieve(datainropframe);
}

    

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

