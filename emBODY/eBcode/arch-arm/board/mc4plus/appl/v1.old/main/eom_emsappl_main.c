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

#include "OPCprotocolManager_Cfg.h" 
#include "EOtheEMSapplDiagnostics.h"
#include "emBODYrobot.h"

#include "EOtheLEDpulser.h"


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
static void s_eom_emsappl_main_init_testing(void);


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
  
    eom_sys_Start(eom_sys_GetHandle(), s_eom_emsappl_main_init_testing);
}

//#ifdef _TEST_SEQNUM_
//extern void eom_emstransceiver_callback_incaseoferror_in_sequencenumberReceived(eOipv4addr_t remipv4addr, uint64_t rec_seqnum, uint64_t expected_seqnum)
//{
//    char str[80];
//    snprintf(str, sizeof(str)-1, "SEQ_NUM: rec=%llu expeted=%llu ", rec_seqnum, expected_seqnum);
//    hal_trace_puts(str);
//  
//}
//#endif

extern void eom_emstransceiver_callback_incaseoferror_in_sequencenumberReceived(eOipv4addr_t remipv4addr, uint64_t rec_seqnum, uint64_t expected_seqnum)
{
    eo_theEMSdgn_UpdateApplCore(eo_theEMSdgn_GetHandle());    
    eo_theEMSdgn_Signalerror(eo_theEMSdgn_GetHandle(), eodgn_nvidbdoor_emsapplcommon  , 0);
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

/** @fn         static void s_eom_emsappl_main_init_testing(void)
    @brief      Redefine this function for testing purpose 
    @details    bla bla bla.
 **/

static void s_eom_emsappl_main_init_testing(void)
{      
      
    eOappEncReader_cfg_t cfg = { 0 }; // marco.accame on 12jan2015: the warning will be removed when a final version of the config is released.
    
    //memcpy(&cfg.streams, &p->config.encoderstreams, sizeof(cfg.streams));
    //cfg.connectedEncodersMask = p->config.connectedEncodersMask;
    cfg.SPI_callbackOnLastRead = NULL;
    cfg.SPI_callback_arg = NULL;
    
    //Test Initialization
    //joints
    cfg.joints_number = 3;
    eOappEncReader_joint_t j1,j2,j3,jnone;
    j1.joint_pos = eo_appEncReader_joint_position0;
    j1.primary_encoder = eo_appEncReader_enc_type_AMO;
    j1.SPI_encoder = hal_encoder1;
    j1.extra_encoder = eo_appEncReader_enc_type_NONE;
    j1.SPI_stream_associated = eo_appEncReader_stream0;
    j1.SPI_stream_position = 0;
    
    j2.joint_pos = eo_appEncReader_joint_position2;
    j2.primary_encoder = eo_appEncReader_enc_type_AEA;
    j2.SPI_encoder = hal_encoder2;
    j2.extra_encoder = eo_appEncReader_enc_type_NONE;
    j2.SPI_stream_associated = eo_appEncReader_stream1;
    j2.SPI_stream_position = 0;
    
    j3.joint_pos = eo_appEncReader_joint_position3;
    j3.primary_encoder = eo_appEncReader_enc_type_INC;
    j3.extra_encoder= eo_appEncReader_enc_type_NONE;
    jnone.primary_encoder = eo_appEncReader_enc_type_NONE;
    jnone.extra_encoder = eo_appEncReader_enc_type_NONE;
    cfg.joints[0] = j1;
    cfg.joints[1] = j2;
    cfg.joints[2] = j3;
    cfg.joints[3] = jnone;
    cfg.joints[4] = jnone;
    cfg.joints[5] = jnone;
    
    //SPI streams
    eOappEncReader_stream_t st1, st2;
    st1.numberof = 1;
    st1.type = hal_encoder_t2;
    st1.encoders[0] = hal_encoder1;
    st1.encoders[1] = hal_encoderNONE;
    st1.encoders[2] = hal_encoderNONE;
    st1.encoders[3] = hal_encoderNONE;
    st1.encoders[4] = hal_encoderNONE;
    st1.encoders[5] = hal_encoderNONE;
    
    st2.numberof = 1;
    st2.type = hal_encoder_t1;
    st2.encoders[0] = hal_encoder2;
    st2.encoders[1] = hal_encoderNONE;
    st2.encoders[2] = hal_encoderNONE;
    st2.encoders[3] = hal_encoderNONE;
    st2.encoders[4] = hal_encoderNONE;
    st2.encoders[5] = hal_encoderNONE;
    
    cfg.SPI_streams[0] = st1;
    cfg.SPI_streams[1] = st2;
    
   EOappEncReader* enc_ptr = eo_appEncReader_New(&cfg);
   
   eo_appEncReader_StartRead(enc_ptr);
   uint32_t pos1, pos2,pos3,dummy;
   char str[96];
   hal_encoder_errors_flags fl;
   for(;;)
   {
       //wait until both SPI are ready
       while (!eo_appEncReader_isReady(enc_ptr)) {};
       eo_appEncReader_GetJointValue(enc_ptr,0, &pos1, &dummy,&fl);
       snprintf (str, sizeof(str), "ENCODER POS1: %d", pos1);
       hal_trace_puts(str);
           
       eo_appEncReader_GetJointValue(enc_ptr,2, &pos2, &dummy,&fl);
       snprintf (str, sizeof(str), "ENCODER POS2: %d", pos2);
       hal_trace_puts(str);
           
       eo_appEncReader_GetJointValue(enc_ptr,3, &pos3, &dummy,&fl);
       snprintf (str, sizeof(str), "ENCODER POS4: %d", pos3);
       hal_trace_puts(str);
    
       //restart the encoders and wait
       eo_appEncReader_StartRead(enc_ptr);
       hal_trace_puts("\n");    
       hal_sys_delay(hal_RELTIME_1second);
   }
  
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



