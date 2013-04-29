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

#include "OPCprotocolManager.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "OPCprotocolManager_hid.h" 


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

static opcprotman_var_map_t* s_opcprotman_find(OPCprotocolManager* p, opcprotman_header_t* head);
static opcprotman_res_t s_opcprotman_process_operation(opcprotman_message_t* msg, opcprotman_var_map_t* map, opcprotman_message_t* reply, uint16_t* replysize);
static opcprotman_var_map_t* s_opcprotman_find_var(OPCprotocolManager* p, uint16_t var);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "OPCprotocolManager";



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern OPCprotocolManager * opcprotman_New(const opcprotman_cfg_t *cfg)
{
    opcprotman_res_t res;
    OPCprotocolManager* p = NULL;
    
    if(NULL == cfg)
    {
        return(NULL);
    }
    
    if(NULL == (p = (OPCprotocolManager*) calloc(sizeof(OPCprotocolManager), 1)))
    {
        return(NULL);
    }
    
    p->cfg = cfg;
    
    
    res = opcprotman_personalize_database();  
    if(opcprotman_OK != res)
    {
        return(NULL);
    }
    
    return(p);

}

extern opcprotman_res_t opcprotman_personalize_var(OPCprotocolManager* p, uint16_t var, uint8_t* var_ram, opcprotman_fp_onrec_t fn)
{
    opcprotman_var_map_t* map_ptr;
    
    if((NULL == p) || (NULL == var_ram))
    {
        return(opcprotman_NOK_generic);
    }
    
    map_ptr = opcprotman_find(p, var);
    
    if(NULL == map_ptr)
    {
        return(opcprotman_NOK_generic);
    }
    
    map_ptr->ptr = var_ram;
    map_ptr->onrec = fn;
    
    return(opcprotman_OK);

}

extern opcprotman_res_t opcprotman_Has_Signature(OPCprotocolManager* p, opcprotman_message_t* msg)
{   
    if((NULL == p) || (NULL == msg))
    {
        return(opcprotman_NOK_generic);
    }

    return((opcprotman_signature == msg->head.sgn) ? (opcprotman_OK) : (opcprotman_NOK_generic));
}



extern opcprotman_res_t opcprotman_Has_Same_Database(OPCprotocolManager* p, opcprotman_message_t* msg)
{
    if((NULL == p) || (NULL == msg))
    {
        return(opcprotman_NOK_generic);
    }

    return((p->cfg->databaseversion == msg->head.sgn) ? (opcprotman_OK) : (opcprotman_NOK_generic));
}



extern opcprotman_res_t opcprotman_Is_Managed(OPCprotocolManager* p, opcprotman_message_t* msg)
{
    if((NULL == p) || (NULL == msg))
    {
        return(opcprotman_NOK_generic);
    }
    
    // verify signature
    if(opcprotman_signature != msg->head.sgn)
    {
        return(opcprotman_NOK_generic);
    }
    
    // verify database and belonging
    opcprotman_var_map_t* map = s_opcprotman_find(p, &msg->head);    
    return((NULL != map) ? (opcprotman_OK) : (opcprotman_NOK_generic));   
}


extern opcprotman_res_t opcprotman_Parse(OPCprotocolManager* p, opcprotman_message_t* msg, opcprotman_message_t* reply, uint16_t* replysize)
{
    opcprotman_res_t res = opcprotman_NOK_generic;
       
    if((NULL == p) || (NULL == msg) || (NULL == replysize))
    {
        return(opcprotman_NOK_generic);
    }
    
    if(opcprotman_signature != msg->head.sgn)
    {
        *replysize = 0;
        return(opcprotman_NOK_generic);
    }
    
    
    opcprotman_var_map_t* map = s_opcprotman_find(p, &msg->head);
    
    if(NULL == map)
    {
        return(opcprotman_NOK_generic);
    }
    
    res = s_opcprotman_process_operation(msg, map, reply, replysize);
               
    return(res);
}

extern opcprotman_res_t opcprotman_Form(OPCprotocolManager* p, opcprotman_opc_t opc, opcprotman_var_t var, const void* setdata, opcprotman_message_t* msg, uint16_t* size)
{  
    opcprotman_res_t res = opcprotman_NOK_generic;
    
    
    if((NULL == p) || (NULL == msg) || (NULL == size))
    {
        return(opcprotman_NOK_generic);
    }
    
    *size = 0;
    
    if((opc == opcprotman_opc_say) || (opc == opcprotman_opc_none))
    {
        return(opcprotman_NOK_generic);
    }

    msg->head.sgn   = opcprotman_signature;
    msg->head.opc   = opc;
    msg->head.dbv   = p->cfg->databaseversion;
    msg->head.var   = var;
    msg->head.len   = 0; // but it shall be updated
    
    opcprotman_var_map_t* map = s_opcprotman_find(p, &msg->head);
    
    if((NULL == map) || (0 == map->size) || (NULL == map->ptr))
    {
        memset(&msg->head, 0, sizeof(opcprotman_header_t));
        return(opcprotman_NOK_generic);
    }
    
    msg->head.len = map->size;

    switch(opc)
    {
        case opcprotman_opc_ask:
        {
            *size = sizeof(opcprotman_header_t);
            res = opcprotman_OK;            
        } break;
        
        case opcprotman_opc_sig:
        {   // copy into message the data which is inside the variable
            memcpy(msg->data, map->ptr, map->size);
            *size = map->size + sizeof(opcprotman_header_t);
            res = opcprotman_OK;
        } break;
 
        case opcprotman_opc_set:
        {   
            // copy into message the data which is inside the variable
            if(NULL != setdata)
            {
                memcpy(msg->data, setdata, map->size);  
                *size = map->size + sizeof(opcprotman_header_t);
                res = opcprotman_OK;
            }
            else     
            {   
                res = opcprotman_NOK_generic;
            }            
            
        } break;
        
        default:
        {
            res = opcprotman_NOK_generic;        
        } break;
 
    }
    
    if(opcprotman_NOK_generic == res)
    {
        memset(&msg->head, 0, sizeof(opcprotman_header_t));
    }

    
       
    return(res);     
}


extern opcprotman_var_map_t* opcprotman_find(OPCprotocolManager* p, uint16_t var)
{
    if(NULL == p)
    {
        return(NULL);
    }
    return(s_opcprotman_find_var(p, var));
}

// #ifndef __USE_MY _OPCPROTMAN_PERSONALIZE_DATABASE__
// extern opcprotman_res_t opcprotman_personalize_database(void)
// {
//     return(opcprotman_OK);
// }
// #endif
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static opcprotman_var_map_t* s_opcprotman_find(OPCprotocolManager* p, opcprotman_header_t* head)
{
    if(p->cfg->databaseversion != head->dbv)
    {
        return(NULL);
    }

    return(s_opcprotman_find_var(p, head->var));
    
//     uint16_t i;
//     opcprotman_var_map_t* map = NULL;
//     
//     for(i=0; i<p->cfg->numberofvariables; i++)
//     {
//         if(head->var == p->cfg->arrayofvariablemap[i].var)
//         {
//             map = &p->cfg->arrayofvariablemap[i];
//             break;
//         }
//     }
//     
//     return(map);
}


static opcprotman_var_map_t* s_opcprotman_find_var(OPCprotocolManager* p, uint16_t var)
{
    uint16_t i;
    opcprotman_var_map_t* map = NULL;
    
    for(i=0; i<p->cfg->numberofvariables; i++)
    {
        if(var == p->cfg->arrayofvariablemap[i].var)
        {
            map = &p->cfg->arrayofvariablemap[i];
            break;
        }
    }
    
    return(map);
}


static opcprotman_res_t s_opcprotman_process_operation(opcprotman_message_t* msg, opcprotman_var_map_t* map, opcprotman_message_t* reply, uint16_t* replysize)
{
    opcprotman_res_t res = opcprotman_OK;
    
    *replysize = 0;
       
    opcprotman_opc_t opc = (opcprotman_opc_t)msg->head.opc;
    
    switch(opc)
    {
        case opcprotman_opc_ask:
        {
            reply->head.sgn     = opcprotman_signature;
            reply->head.opc     = opcprotman_opc_say;
            reply->head.dbv     = msg->head.dbv;
            reply->head.var     = map->var;
            reply->head.len     = map->size;
            if(0 != map->size)
            {
                memcpy(reply->data, map->ptr, map->size);
            }
            *replysize = sizeof(opcprotman_header_t) + map->size;
            // ok... now i make it multiple of 4
            *replysize += 3;
            *replysize /= 4;
            *replysize *= 4;
            
            res = opcprotman_OK_withreply;
        
        } break;

        case opcprotman_opc_set:
        {
            reply->head.sgn     = opcprotman_signature;
            reply->head.opc     = opcprotman_opc_none;
            reply->head.dbv     = 0;
            reply->head.var     = 0;
            reply->head.len     = 0;
//             if((NULL != map->ptr) && (0 != map->size))
//             {
//                 memcpy(map->ptr, msg->data, map->size);
//             }           
            if(NULL != map->onrec)
            {
                map->onrec(opcprotman_opc_set, map, msg->data);
            }
            
            res = opcprotman_OK;
        
        } break;
        
        case opcprotman_opc_sig:
        {
            reply->head.sgn     = opcprotman_signature;
            reply->head.opc     = opcprotman_opc_none;
            reply->head.dbv     = 0;
            reply->head.var     = 0;
            reply->head.len     = 0;  
            
            if(NULL != map->onrec)
            {
                map->onrec(opcprotman_opc_sig, map, msg->data);
            }
            
            res = opcprotman_OK;    
            
        } break;

        case opcprotman_opc_say:
        {
            reply->head.sgn     = opcprotman_signature;
            reply->head.opc     = opcprotman_opc_none;
            reply->head.dbv     = 0;
            reply->head.var     = 0;
            reply->head.len     = 0;  
            
            if(NULL != map->onrec)
            {
                map->onrec(opcprotman_opc_say, map, msg->data);
            }
            
            res = opcprotman_OK;     
            
        } break;
        
        default:
        {
            reply->head.sgn     = opcprotman_signature;
            reply->head.opc     = opcprotman_opc_none;
            reply->head.dbv     = 0;
            reply->head.var     = 0;
            reply->head.len     = 0;  
            
            res = opcprotman_NOK_generic;    
            
        } break;
     
    }
    
    return(res);
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





