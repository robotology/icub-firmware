
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eo_embotIF_diagnode.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "DiagnosticsNode.h"


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

namespace DiagNode {

    struct Impl
    {
        bool initted {false};
        embot::app::DiagnosticsNode *node {nullptr};
        embot::app::DiagnosticsNode::Config config {};

        Impl() = default;

    };

    Impl impl {};  

} // namespace DiagNode {



extern void embotIF_diagnode_init(void)
{
    if(!DiagNode::impl.initted)
    {
        DiagNode::impl.node = new embot::app::DiagnosticsNode;
        DiagNode::impl.node->init(DiagNode::impl.config);
        DiagNode::impl.initted =  true;
    }
}    

extern void embotIF_diagnode_add(uint8_t *ropstream, uint16_t size)
{
    if(DiagNode::impl.initted)
    {
        DiagNode::impl.node->add({ropstream, size});
    }
}


extern void embotIF_diagnode_add1(eOmn_info_basic_t *infobasic)
{
    if(DiagNode::impl.initted)
    {
        embot::eprot::diagnostics::InfoBasic ib {infobasic->timestamp, infobasic->properties.code, infobasic->properties.flags, infobasic->properties.par16, infobasic->properties.par64};
        DiagNode::impl.node->add(ib);
    }
}


extern void embotIF_diagnode_add2(eOmn_info_basic_t *infobasic)
{
    if(DiagNode::impl.initted)
    {
        embot::eprot::diagnostics::InfoBasic ib {infobasic->timestamp, infobasic->properties.code, infobasic->properties.flags, infobasic->properties.par16, infobasic->properties.par64};
        embot::eprot::rop::Descriptor descriptor;
        descriptor.fill(embot::eprot::rop::OPC::sig, embot::eprot::diagnostics::InfoBasic::id32, 
                        {&ib, embot::eprot::diagnostics::InfoBasic::sizeofobject}, 0x666, 666, 
                        embot::eprot::rop::PLUS::signaturetime);
                                
        DiagNode::impl.node->add(descriptor);
    }
}

extern eObool_t embotIF_diagnode_prepare(uint16_t *sizeofropframe)
{
    bool r {false};
    if(DiagNode::impl.initted)
    {
        size_t ss = 0;
        r = DiagNode::impl.node->prepare(ss);
        if(nullptr != sizeofropframe)
        {
            *sizeofropframe = ss;
        }
    }
    
    return r ? eobool_true : eobool_false;
}

extern uint16_t embotIF_diagnode_retrieve(uint8_t *ropframe, uint16_t capacityofropframe)
{
    if(DiagNode::impl.initted)
    {
        embot::utils::Data data(ropframe, capacityofropframe);
        DiagNode::impl.node->retrieve(data);
        return data.capacity;
    }  

    return 0;  
}




// --------------------------------------------------------------------------------------------------------------------
// - test unit
// --------------------------------------------------------------------------------------------------------------------
// enable it w/ macro ENABLE_embotIF_TEST_diagnode_parse defined

#include "DiagnosticsHost.h"
#include "EoProtocol.h"
#include "EoProtocolMN.h"
#include "EoError.h"
#include "hal_trace.h"

namespace DiagHost {
    
    char textout[128] = {0};
    
    struct Impl
    {
        bool initted {false};
        embot::app::DiagnosticsHost *host {nullptr};
        embot::app::DiagnosticsHost::Config config { false, 128, ropdecode};
        

        Impl() = default;
            
        static bool ropdecode(const embot::eprot::IPv4 &ipv4, const embot::eprot::rop::Descriptor &rop)
        {
            // i accept only sig<>
            if(embot::eprot::rop::OPC::sig != rop.opcode)
            {
                return false;
            }
            
            constexpr embot::eprot::ID32 id32statusbasic = EOPROT_ID_GET(eoprot_endpoint_management, eoprot_entity_mn_info, 0, eoprot_tag_mn_info_status_basic);
            //eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_info, 0, eoprot_tag_mn_info_status_basic);
            const embot::eprot::ID32 id32statusfull = EOPROT_ID_GET(eoprot_endpoint_management, eoprot_entity_mn_info, 0, eoprot_tag_mn_info_status);
            //eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_info, 0, eoprot_tag_mn_info_status);
            
            switch(rop.id32)
            {
                
                case embot::eprot::diagnostics::InfoBasic::id32:
                {
                    eOmn_info_basic_t *info = reinterpret_cast<eOmn_info_basic_t*>(rop.value.getU08ptr()); 
                    uint64_t tt = info->timestamp;
                    uint32_t sec = tt/(1000*1000);
                    uint32_t tmp = tt%(1000*1000);
                    uint32_t msec = tmp / 1000;
                    uint32_t usec = tmp % 1000;                    
                    const char *text = eoerror_code2string(info->properties.code); 
                    char buf[16] = {0};
                    if(rop.hassignature() || rop.hastime())
                    {
                        snprintf(textout, sizeof(textout), "from %s [sig = 0x%x, tim = %lld] -> @ s%d m%d u%d -> %s", ipv4.tostring(buf, sizeof(buf)), rop.signature, rop.time, sec, msec, usec, text);
//                        if(rop.validtime() && rop.va)
//                        {
//                            snprintf(textout, sizeof(textout), "from %s [sig = 0x%x. tim = %lld] -> @ s%d m%d u%d -> %s", ipv4.tostring(buf, sizeof(buf)), rop.signature, rop.time, sec, msec, usec, text);
//                        }
//                        else
//                        {
//                            snprintf(textout, sizeof(textout), "from %s [sig = 0x%x] -> @ s%d m%d u%d -> %s", ipv4.tostring(buf, sizeof(buf)), rop.signature, sec, msec, usec, text);
//                        }
                    }
                    else
                    {
                        snprintf(textout, sizeof(textout), "from %s -> @ s%d m%d u%d: %s", ipv4.tostring(buf, sizeof(buf)), sec, msec, usec, text);
                    }
                    hal_trace_puts(textout);
                } break;
                
                default:
                {
                    hal_trace_puts("unknown");
                } break;
            }
               
            return true;
        }
    };

    Impl impl {};  

} // namespace DiagHOST {


extern void embotIF_TEST_diagnode_init(void)
{
    if(!DiagHost::impl.initted)
    {
        DiagHost::impl.host = new embot::app::DiagnosticsHost;
        DiagHost::impl.host->init(DiagHost::impl.config);
        DiagHost::impl.initted =  true;
    }    
}

extern void embotIF_TEST_diagnode_parse(uint8_t *ropframe, uint16_t sizeofropframe)
{
    if(DiagHost::impl.initted)
    {
        embot::eprot::IPv4 ipv4 {"10.0.1.2"};
        embot::utils::Data data(ropframe, sizeofropframe);
        DiagHost::impl.host->accept(ipv4, data);
    }  

}




    

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

