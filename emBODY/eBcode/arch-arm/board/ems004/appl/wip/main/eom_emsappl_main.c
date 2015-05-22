/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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

/* @file       eom_emsappl_main.c
	@brief      This file keeps the main of an application on ems using the embobj
	@author     marco.accame@iit.it
    @date       05/21/2012
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------




#include "stdint.h"
#include "stdlib.h"
#include "string.h"



#include "EOMtheSystem.h"

#include "EOMtheEMSapplCfg.h"
#include "EOMtheEMSappl.h"



#include "EOtheLEDpulser.h"

#include "EoError.h"
#include "EOtheErrorManager.h"



#include "EOtheCANmapping.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------


 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_eom_emsappl_main_init(void);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


int main(void)
{
    EOMtheEMSapplCfg* emscfg = eom_emsapplcfg_Initialise();

    eom_sys_Initialise( &emscfg->wsyscfg.msyscfg, 
                        &emscfg->wsyscfg.mempoolcfg,                         
                        &emscfg->wsyscfg.errmancfg,                 
                        &emscfg->wsyscfg.tmrmancfg, 
                        &emscfg->wsyscfg.cbkmancfg 
                      );  
    
    eom_sys_Start(eom_sys_GetHandle(), s_eom_emsappl_main_init);

}



extern void eom_emstransceiver_callback_incaseoferror_in_sequencenumberReceived(eOipv4addr_t remipv4addr, uint64_t rec_seqnum, uint64_t expected_seqnum)
{   // the only reason of using the following two variables as static is: to reduce the use of stack.
    static int64_t delta = 0;
    static eOerrmanDescriptor_t errdes = 
    {
        .code           = EOERRORCODE(eoerror_category_System, eoerror_value_SYS_transceiver_rxseqnumber_error),
        .sourcedevice   = eo_errman_sourcedevice_localboard,
        .sourceaddress  = 0,
        .par16          = 0,
        .par64          = 0
    };
    
    if(1 == rec_seqnum)
    {
        // it is the first packet received from a remote transmitter freshly initted (i.e., robotInterface has just re-started bu this board was alive well before)
        // thus, we dont issue an error but an info: 
        errdes.code  = EOERRORCODE(eoerror_category_System, eoerror_value_SYS_transceiver_rxseqnumber_restarted);
        errdes.par64 = expected_seqnum;
        errdes.par16 = 1;       
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, NULL, &errdes);   
        
        // ok, now we must set back the error code as it is normal.
        errdes.code = EOERRORCODE(eoerror_category_System, eoerror_value_SYS_transceiver_rxseqnumber_error);
    }  
    else
    {    
        delta = rec_seqnum - expected_seqnum;
        if(delta > INT16_MAX)       delta = INT16_MAX;  //32767
        else if(delta < INT16_MIN)  delta = INT16_MIN;  //-32768;
        
        errdes.par16            = (int16_t)delta; 
        errdes.par64            = expected_seqnum; 
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);
    }
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


/** @fn         static void s_eom_emsappl_main_init(void)
    @brief      It initialises the emsappl 
    @details    bla bla bla.
 **/

static void s_eom_emsappl_main_init(void)
{      
    // init leds via the EOtheLEDpulser object
    eOledpulser_cfg_t ledpulsercfg =  
    {
        .led_enable_mask    = (1 << eo_ledpulser_led_zero | 1 << eo_ledpulser_led_one | 1 << eo_ledpulser_led_two | 1 << eo_ledpulser_led_three | 1 << eo_ledpulser_led_four | 1 << eo_ledpulser_led_five),
        .led_init           = (eOint8_fp_uint8_cvoidp_t) hal_led_init,
        .led_on             = (eOint8_fp_uint8_t) hal_led_on,
        .led_off            = (eOint8_fp_uint8_t) hal_led_off,
        .led_toggle         = (eOint8_fp_uint8_t) hal_led_toggle
    };

    eo_ledpulser_Initialise(&ledpulsercfg);
    
//    {
//        eOprotID32_t id32 = 0;
//        eOresult_t res = eores_OK;
//        eOcanmap_location_t loc = {0};
//        uint8_t numoflocs = 0;
//        uint8_t index = 0;
//        eObrd_cantype_t boardtype = eobrd_cantype_unknown;
//        extern EOconstvector* eo_vectorof_boardprops_eb1;
//        extern EOconstvector* eo_vectorof_descriptor_jomo_eb1;
//        extern EOconstvector* eo_vectorof_descriptor_skin_eb1;
//        EOtheCANmapping * canmap = eo_canmap_Initialise(NULL);
//        
//        eo_canmap_LoadBoards(canmap, eo_vectorof_boardprops_eb1);        
//        eo_canmap_ConfigEntity(canmap, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, eo_vectorof_descriptor_jomo_eb1); 
////        eo_canmap_eo_canmap_ConfigEntity(canmap, eoprot_endpoint_skin, eoprot_entity_sk_skin, eo_vectorof_descriptor_skin_eb2);
//        
//        // mc
//        id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, 2, eoprot_tag_none);       
//        res = eo_canmap_GetEntityLocation(canmap, id32, &loc, &numoflocs, &boardtype);      
//        res  = res;   

//        index = eo_canmap_GetEntityIndex(canmap, loc);      
//        index = index; 
//        
//        // sk
//        id32 = eoprot_ID_get(eoprot_endpoint_skin, eoprot_entity_sk_skin, 0, eoprot_tag_none);
//        res = eo_canmap_GetEntityLocation(canmap, id32, &loc, &numoflocs, &boardtype);      
//        res  = res;   

//        loc.port = 1;
//        loc.addr = 8;
//        index = eo_canmap_GetEntityIndex(canmap, loc);      
//        index = index; 
//        
//        EOconstarray *carray = eo_canmap_GetBoardLocations(canmap);
//        uint8_t size = eo_constarray_Size(carray);
//        uint8_t i = 0;
//        const eOcanmap_board_extended_t *extbrd = NULL;
//        for(i=0; i<size; i++)
//        {
//            eOcanmap_location_t *ll = (eOcanmap_location_t*) eo_constarray_At(carray, i);
//            extbrd = eo_canmap_GetBoard(canmap,  *ll);
//            uint8_t adr1 = extbrd->board.props.location.addr;
//            adr1 = adr1;
//            uint8_t adr2 = ll->addr;
//            adr2 = adr2;
//        }
//    }
    
    // init the ems application
    EOMtheEMSapplCfg* emscfg = eom_emsapplcfg_GetHandle();    
    eom_emsappl_Initialise(&emscfg->applcfg);
      
    // set the led reserved for link
    if(eores_OK == eom_ipnet_ResolveIP(eom_ipnet_GetHandle(), emscfg->applcfg.hostipv4addr, 5*EOK_reltime100ms))
    {
        // set led0 to ON
       eo_ledpulser_On(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero);
    }
    else
    {
        eo_ledpulser_Off(eo_ledpulser_GetHandle(), eo_ledpulser_led_zero);
    }
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



