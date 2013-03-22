/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "EoCommon.h"

#include "EOtheMemoryPool.h"
#include "EOtheErrormanager.h"


#include "OPCprotocolManager.h"

//#include "appl-core.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSbackdoortransceiver.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSbackdoortransceiver_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const eOemsbackdoortransceiver_cfg_t eom_emsbackdoortransceiver_DefaultCfg = 
{
    EO_INIT(.hostipv4port)              3333,
    EO_INIT(.replypktcapacity)          64,
    EO_INIT(.backdoorprotocol)          eobackdoor_protocol_userdef_opc
};



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static eObool_t s_parse_and_form(uint8_t *data, uint8_t size, eOipv4addr_t remaddr, eOipv4port_t remport);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOMtheEMSbackdoortransceiver";

 
static EOMtheEMSbackdoortransceiver s_emsbackdoortransceiver_singleton = 
{
    EO_INIT(.replypkt)                  NULL,
    EO_INIT(.transmit)                  eobool_false,
    EO_INIT(.cfg)                       {0},
    EO_INIT(.opcprotman)                NULL
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOMtheEMSbackdoortransceiver * eom_emsbackdoortransceiver_Initialise(const eOemsbackdoortransceiver_cfg_t *cfg)
{
    if(NULL != s_emsbackdoortransceiver_singleton.replypkt)
    {
        return(&s_emsbackdoortransceiver_singleton);
    }
    
    if(NULL == cfg)
    {
        cfg = &eom_emsbackdoortransceiver_DefaultCfg;
    }
    
    memcpy(&s_emsbackdoortransceiver_singleton.cfg, cfg, sizeof(eOemsbackdoortransceiver_cfg_t));
    
    s_emsbackdoortransceiver_singleton.replypkt = eo_packet_New(s_emsbackdoortransceiver_singleton.cfg.replypktcapacity);
    s_emsbackdoortransceiver_singleton.transmit = eobool_false;
    
    s_emsbackdoortransceiver_singleton.opcprotman = opcprotman_New(eom_emsbackdoortransceiver_hid_userdef_get_OPCprotocolManager_cfg());
    
    return(&s_emsbackdoortransceiver_singleton);
}


extern EOMtheEMSbackdoortransceiver* eom_emsbackdoortransceiver_GetHandle(void) 
{
    if(NULL != s_emsbackdoortransceiver_singleton.replypkt)
    {
        return(&s_emsbackdoortransceiver_singleton);
    }
    else
    {
        return(NULL);
    }
}



extern eOresult_t eom_emsbackdoortransceiver_Parse(EOMtheEMSbackdoortransceiver* p, EOpacket* rxpkt)
{
    uint8_t *data = NULL;
    uint16_t size = 0;
    eOipv4addr_t remaddr;
    eOipv4port_t remport;
    
    if((NULL == p) || (NULL == rxpkt))
    {
        return(eores_NOK_nullpointer);
    }
        
    eo_packet_Payload_Get(rxpkt, &data, &size);
    eo_packet_Addressing_Get(rxpkt, &remaddr, &remport);
    
 
    // ok can parse the packet
    
    p->transmit = s_parse_and_form(data, size, remaddr, remport);
        
     
    return(eores_OK);
}


extern eOresult_t eom_emsbackdoortransceiver_GetReply(EOMtheEMSbackdoortransceiver* p, EOpacket** replypkt)
{
    eOresult_t res = eores_NOK_generic;
    
    if((NULL == p) || (NULL == replypkt))
    {
        return(eores_NOK_nullpointer);
    }
    
    *replypkt = p->replypkt;
    
    res = (eobool_true == p->transmit) ? (eores_OK) : (eores_NOK_generic);
    
    p->transmit = eobool_false;
    
    return(res);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

__weak extern opcprotman_cfg_t* eom_emsbackdoortransceiver_hid_userdef_get_OPCprotocolManager_cfg(void)
{
    return(NULL);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


// returns eobool_true if a tx is required
static eObool_t s_parse_and_form(uint8_t *data, uint8_t size, eOipv4addr_t remaddr, eOipv4port_t remport)
{
//    eOresult_t res = eores_NOK_generic;
    eObool_t transmit = eobool_false;


    uint8_t *txdata;
    uint16_t txsize = 0;
    eo_packet_Payload_Get(s_emsbackdoortransceiver_singleton.replypkt, &txdata, &txsize);  

    
    if((eobackdoor_protocol_userdef_opc == s_emsbackdoortransceiver_singleton.cfg.backdoorprotocol) &&
       (opcprotman_OK == opcprotman_Has_Signature(s_emsbackdoortransceiver_singleton.opcprotman, (opcprotman_message_t*)data)) )
    {
         // call the opc parser
        uint16_t opctxsize = 0;
        opcprotman_res_t rr = opcprotman_Parse(s_emsbackdoortransceiver_singleton.opcprotman, (opcprotman_message_t*)data, (opcprotman_message_t*)txdata, &opctxsize);    

        if(opcprotman_OK_withreply == rr)
        {               
            transmit = eobool_true;            
            eo_packet_Size_Set(s_emsbackdoortransceiver_singleton.replypkt, opctxsize); 
            eo_packet_Addressing_Set(s_emsbackdoortransceiver_singleton.replypkt, remaddr, s_emsbackdoortransceiver_singleton.cfg.hostipv4port);
        }            
  
    }


    return(transmit);
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





