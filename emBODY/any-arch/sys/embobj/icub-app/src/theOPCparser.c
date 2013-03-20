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



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "theOPCparser.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "theOPCparser_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static opcparser_var_map_t* s_opcparser_find(opcparser_header_t* head);
static opcparser_res_t s_opcparser_process_operation(opcparser_message_t* msg, opcparser_var_map_t* map, opcparser_message_t* reply, uint16_t* replysize);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "theOPCparser";

 
static theOPCparser s_opcparser_singleton = 
{
    .cfg        = NULL
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern theOPCparser * opcparser_Initialise(const opcparser_cfg_t *cfg)
{
    if(NULL != s_opcparser_singleton.cfg)
    {
        return(&s_opcparser_singleton);
    }
    
    if(NULL == cfg)
    {
        return(NULL);
    }
    
    s_opcparser_singleton.cfg = cfg;
      
    return(&s_opcparser_singleton);
}


extern theOPCparser* opcparser_GetHandle(void) 
{
    if(NULL != s_opcparser_singleton.cfg)
    {
        return(&s_opcparser_singleton);
    }
    else
    {
        return(NULL);
    }
}


extern opcparser_res_t opcparser_Has_Signature(theOPCparser* p, opcparser_message_t* msg)
{   
    if((NULL == p) || (NULL == msg))
    {
        return(opcparser_NOK_generic);
    }

    return((opcparser_signature == msg->head.sgn) ? (opcparser_OK) : (opcparser_NOK_generic));
    
}

extern opcparser_res_t opcparser_Parse(theOPCparser* p, opcparser_message_t* msg, opcparser_message_t* reply, uint16_t* replysize)
{
    opcparser_res_t res = opcparser_NOK_generic;
       
    if((NULL == p) || (NULL == msg) || (NULL == replysize))
    {
        return(opcparser_NOK_generic);
    }
    
    if(opcparser_signature != msg->head.sgn)
    {
        *replysize = 0;
        return(opcparser_NOK_generic);
    }
    
    
    opcparser_var_map_t* map = s_opcparser_find(&msg->head);
    
    if(NULL == map)
    {
        return(opcparser_NOK_generic);
    }
    
    res = s_opcparser_process_operation(msg, map, reply, replysize);
    
           
    return(res);
}

extern opcparser_res_t opcparser_Form(theOPCparser* p, opcparser_opc_t opc, opcparser_var_t var, opcparser_message_t* msg, uint16_t* size)
{       
    if((NULL == p) || (NULL == msg) || (NULL == size))
    {
        return(opcparser_NOK_generic);
    }
    
    if((opc != opcparser_opc_ask) && (opc != opcparser_opc_sig))
    {
        return(opcparser_NOK_generic);
    }

    msg->head.sgn   = opcparser_signature;
    msg->head.opc   = opc;
    msg->head.dbv   = s_opcparser_singleton.cfg->databaseversion;
    msg->head.var   = var;
    msg->head.len   = 0; // but it shall be updated
    
    opcparser_var_map_t* map = s_opcparser_find(&msg->head);
    
    if(NULL == map)
    {
        memset(&msg->head, 0, sizeof(opcparser_header_t));
        return(opcparser_NOK_generic);
    }
    
    msg->head.len = map->size;
    if(0 != map->size)
    {
        memcpy(msg->data, map->ptr, map->size);
    }
    
    return(opcparser_OK);
     
}





// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static opcparser_var_map_t* s_opcparser_find(opcparser_header_t* head)
{
    if(s_opcparser_singleton.cfg->databaseversion != head->dbv)
    {
        return(NULL);
    }
    
    uint16_t i;
    opcparser_var_map_t* map = NULL;
    
    for(i=0; i<s_opcparser_singleton.cfg->numberofvariables; i++)
    {
        if(head->var == s_opcparser_singleton.cfg->arrayofvariablemap[i].var)
        {
            map = &s_opcparser_singleton.cfg->arrayofvariablemap[i];
            break;
        }
    }
    
    return(map);
}


static opcparser_res_t s_opcparser_process_operation(opcparser_message_t* msg, opcparser_var_map_t* map, opcparser_message_t* reply, uint16_t* replysize)
{
    opcparser_res_t res = opcparser_OK;
    
    *replysize = 0;
       
    opcparser_opc_t opc = (opcparser_opc_t)msg->head.opc;
    
    switch(opc)
    {
        case opcparser_opc_ask:
        {
            reply->head.sgn     = opcparser_signature;
            reply->head.opc     = opcparser_opc_say;
            reply->head.dbv     = msg->head.dbv;
            reply->head.var     = map->var;
            reply->head.len     = map->size;
            if(0 != map->size)
            {
                memcpy(reply->data, map->ptr, map->size);
            }
            *replysize = sizeof(opcparser_header_t) + map->size;
            // ok... now i make it multiple of 4
            *replysize += 3;
            *replysize /= 4;
            *replysize *= 4;
            
            res = opcparser_OK_withreply;
        
        } break;

        case opcparser_opc_set:
        {
            reply->head.sgn     = opcparser_signature;
            reply->head.opc     = opcparser_opc_none;
            reply->head.dbv     = 0;
            reply->head.var     = 0;
            reply->head.len     = 0;
//             if((NULL != map->ptr) && (0 != map->size))
//             {
//                 memcpy(map->ptr, msg->data, map->size);
//             }           
            if(NULL != map->onset)
            {
                map->onset(map, msg->data);
            }
            
            res = opcparser_OK;
        
        } break;
        
        case opcparser_opc_sig:
        {
            reply->head.sgn     = opcparser_signature;
            reply->head.opc     = opcparser_opc_none;
            reply->head.dbv     = 0;
            reply->head.var     = 0;
            reply->head.len     = 0;  
            
            if(NULL != map->onsig)
            {
                map->onsig(map, msg->data);
            }
            
            res = opcparser_OK;    
            
        } break;

        case opcparser_opc_say:
        default:
        {
            reply->head.sgn     = opcparser_signature;
            reply->head.opc     = opcparser_opc_none;
            reply->head.dbv     = 0;
            reply->head.var     = 0;
            reply->head.len     = 0;  
            
            res = opcparser_NOK_generic;    
            
        } break;
     
    }
    
    return(res);
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





