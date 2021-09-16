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

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoError.h"
#include "EOtheErrorManager.h"
#include "EOtheCANservice.h"
#include "EOtheCANdiscovery2.h"
#include "EOtheETHmonitor.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSconfigurator.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSconfigurator_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// marco.accame on 20 oct 2015: the object EOtheCANservice is configured with the argument passed to eo_canserv_Initialise()
// to make the CAN-RX handler to send the event emsconfigurator_evt_userdef00 to the task of EOMtheEMSconfigurator whenever 
// a CAN frame is received in CFG state. the task then executes this function, where it is correct to put the parser.

extern void eom_emsconfigurator_hid_userdef_ProcessUserdef00Event(EOMtheEMSconfigurator* p)
{   
    eo_canserv_ParseAll(eo_canserv_GetHandle());
}


// marco.accame on 15 sept 15:  the event emsconfigurator_evt_userdef01 is send by a timer inside the EOtheCANdiscovery2 
// if we are in CFG state so that we can _Tick() it.

extern void eom_emsconfigurator_hid_userdef_ProcessUserdef01Event(EOMtheEMSconfigurator* p)
{
    eo_candiscovery2_Tick(eo_candiscovery2_GetHandle());
}



extern void eom_emsconfigurator_hid_userdef_ProcessUserdef02Event(EOMtheEMSconfigurator* p)
{
    eo_ethmonitor_Tick(eo_ethmonitor_GetHandle());
}

#if defined(TEST_AEA3)

#include <hal_spiencoder.h>
#include <hal_trace.h>

#include "hal_motor.h"
#include "hal_quadencoder.h"


void cbk(void *p)
{
    static volatile uint32_t v = 0;
    v++;
}

hal_spiencoder_cfg_t cfgaea = {0};
hal_spiencoder_diagnostic_t diag = {0};
//static hal_spiencoder_position_t raw_buff_0 = 0;
//static hal_spiencoder_position_t raw_buff_1 = 0;
//static hal_spiencoder_position_t raw_buff_2 = 0;

static hal_spiencoder_position_t raw_value = 0;
//static hal_spiencoder_position_t raw_value_prev = 0;



extern void eom_emsconfigurator_hid_userdef_ProcessTimeout(EOMtheEMSconfigurator* p)
{

    static volatile uint32_t counter = 0;
    // static hal_spiencoder_position_t raw_value = 0;
    
    static hal_result_t res;
    
    if(0 == counter)
    {
        // init aea
        memmove(&cfgaea, &hal_spiencoder_cfg_default, sizeof(hal_spiencoder_cfg_default));

        cfgaea.callback_on_rx       = cbk;
        cfgaea.priority     	    = hal_int_priority05;
        cfgaea.arg                  = NULL;
        cfgaea.type				    = hal_spiencoder_typeAEA3;
        cfgaea.sdata_precheck       = hal_false;  
        cfgaea.reg_addresses[0]	    = 0;
        cfgaea.reg_addresses[1]	    = 0; 
        
        hal_spiencoder_init(hal_spiencoder1, &cfgaea);
        
        // start reading
        //hal_spiencoder_read_start(hal_spiencoder1);
        
        //hal_spiencoder_read_start(hal_spiencoder2);   // TODO: read both AEA2 and AEA3 in different connectors
        
        diag.type = hal_spiencoder_diagnostic_type_none;
        
        // -----------------------------------------------------------------------------
        // init DC encoder motor
        //hal_quadencoder_init(hal_quadencoder3);
        /*
        res = hal_motor_init(hal_motorALL, NULL);
        char str_res[64] = {0};
        
        if(hal_res_OK != res)
        {
            snprintf(str_res, sizeof(str_res), "MOTORS DEVICE INIT FAILED");
            return;
        }
        else
        {
            snprintf(str_res, sizeof(str_res), "MOTORS DEVICE INIT SUCCESS");
        }
        hal_trace_puts(str_res);

        // start DC motor
        */

    }
    else if(0 == (counter % 1))
    {
        
        // retrieve the reading
        
        hal_result_t r = hal_spiencoder_get_value2(hal_spiencoder1, &raw_value, &diag);
        
        //hal_result_t r = hal_spiencoder_get_value3(hal_spiencoder1, &raw_value, &raw_buff_0, &raw_buff_1, &raw_buff_2, &diag);
        
        // print value
        char str[64] = {0};
        if(hal_res_OK == r)
        { 
            hal_spiencoder_position_t raw_value_shifted =  raw_value >> 9; // sensor return 16 bits --> only 14 are valid
            snprintf(str, sizeof(str), "%d, %d, %f", raw_value, raw_value_shifted, ((360.0 *  raw_value_shifted) / (1024.0*16)));  // (360*value)/(2^14)
            
            
            //hal_spiencoder_position_t r = raw_value > raw_value_prev ? raw_value - raw_value_prev : raw_value_prev - raw_value;
            
            //if(r > 20000)
            //{
            //    snprintf(str, sizeof(str), "[ERROR]: (%d - %d) = %d > 20000", raw_value, raw_value_prev, r);
            //}
            
            //raw_value_prev = raw_value;
            
            // TODO: remove
            //uint8_t s0 = raw_buff_0;
            //uint8_t s1 = raw_buff_1;
            //uint8_t s2 = raw_buff_2;
            
            //uint16_t x0 = ((s0 & 0x7F) << 7) + s1; // 14 bit of resolution
            //x0 <<= 1;
            //x0 += s2;
            
            //snprintf(str, sizeof(str), "%x, %x, %x %d", s0, s1, s2, x0);

            hal_trace_puts(str);
        }
        else
        {
            snprintf(str, sizeof(str), "aea error");
        }
        
        //eo_errman_Trace(eo_errman_GetHandle(), str, "test aea"); 
        
        // start reading
        hal_spiencoder_read_start(hal_spiencoder1);
        
        //hal_motor_pwmset(hal_motorALL ,10);
    }
	
    // 
    counter++;
}
#endif

// marco.accame on 20 oct 2015: this function is triggered if function eom_emssocket_Transmit() inside the task 
// of EOMtheEMSconfigurator it there is a failure to transmit a UDP packet.

extern void eom_emsconfigurator_hid_userdef_onemstransceivererror(EOMtheEMStransceiver* p)
{
    eOerrmanDescriptor_t errdes = {0};
    errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_configurator_udptxfailure);
    errdes.par16            = 0;
    errdes.par64            = 0;
    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
    errdes.sourceaddress    = 0;    
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, "EOMtheEMSconfigurator", &errdes); 
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





