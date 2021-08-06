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

<<<<<<< HEAD
// simone.girardi on 8 Nov 2021: with the following macro enabled we can test the reading on aea3 encoder.
// with a minimum effort we can use it to test other similar encoders (e.g. aea1, aea2 ...)  as well.
=======
>>>>>>> f10575e65 (mc4plus: moved the tests for aea3 into a dedicated project)
#if defined(TEST_AEA3)

#include <hal_spiencoder.h>
#include <hal_trace.h>

<<<<<<< HEAD
=======
hal_spiencoder_cfg_t cfgaea = {0};

>>>>>>> f10575e65 (mc4plus: moved the tests for aea3 into a dedicated project)
void cbk(void *p)
{
    static volatile uint32_t v = 0;
    v++;
}

<<<<<<< HEAD
hal_spiencoder_cfg_t cfgaea = {0};
hal_spiencoder_diagnostic_t diag = {0};

hal_spiencoder_position_t raw_value = 0;

// This function is called every 10 ms (see eom_emsconfigurator_Initialise)
extern void eom_emsconfigurator_hid_userdef_ProcessTimeout(EOMtheEMSconfigurator* p)
{
    static volatile uint32_t counter = 0;
    static hal_result_t res;
    
=======
hal_spiencoder_diagnostic_t diag = {0};
static hal_spiencoder_position_t raw_buff_0 = 0;
static hal_spiencoder_position_t raw_buff_1 = 0;
static hal_spiencoder_position_t raw_buff_2 = 0;
static hal_spiencoder_position_t raw_value = 0;

unsigned char reverse_byte(unsigned char x)
{
    static const unsigned char table[] = {
        0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
        0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
        0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
        0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
        0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
        0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
        0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
        0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
        0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
        0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
        0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
        0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
        0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
        0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
        0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
        0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
        0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
        0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
        0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
        0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
        0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
        0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
        0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
        0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
        0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
        0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
        0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
        0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
        0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
        0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
        0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
        0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
    };
    return table[x];
}


extern void eom_emsconfigurator_hid_userdef_ProcessTimeout(EOMtheEMSconfigurator* p)
{

    static volatile uint32_t counter = 0;
	
    // static hal_spiencoder_position_t raw_value = 0;
    
    // if you want to use the Logic Analyzer, move these 2 variables to the global space variables.
    //static hal_spiencoder_position_t raw_buff_val_1 = 0;
    //static hal_spiencoder_position_t raw_buff_val_2 = 0;

>>>>>>> f10575e65 (mc4plus: moved the tests for aea3 into a dedicated project)
    if(0 == counter)
    {
        // init aea
        memmove(&cfgaea, &hal_spiencoder_cfg_default, sizeof(hal_spiencoder_cfg_default));
<<<<<<< HEAD
        
        cfgaea.callback_on_rx       = cbk;
        cfgaea.priority             = hal_int_priority05;
        cfgaea.arg                  = NULL;
        cfgaea.type	                = hal_spiencoder_typeAEA3;
=======

        cfgaea.callback_on_rx       = cbk;
        cfgaea.priority     	    = hal_int_priority05;
        cfgaea.arg                  = NULL;
        cfgaea.type				    = hal_spiencoder_typeAEA3;
>>>>>>> f10575e65 (mc4plus: moved the tests for aea3 into a dedicated project)
        cfgaea.sdata_precheck       = hal_false;  
        cfgaea.reg_addresses[0]	    = 0;
        cfgaea.reg_addresses[1]	    = 0; 
        
        hal_spiencoder_init(hal_spiencoder1, &cfgaea);
<<<<<<< HEAD
        
        diag.type = hal_spiencoder_diagnostic_type_none;
=======
        // start reading
        hal_spiencoder_read_start(hal_spiencoder1);
        
        //hal_spiencoder_read_start(hal_spiencoder2);   // TODO: read both AEA2 and AEA3
        
        diag.type = hal_spiencoder_diagnostic_type_none;

>>>>>>> f10575e65 (mc4plus: moved the tests for aea3 into a dedicated project)
    }
    else if(0 == (counter % 1))
    {
        // retrieve the reading
<<<<<<< HEAD
        hal_result_t r = hal_spiencoder_get_value2(hal_spiencoder1, &raw_value, &diag);
        
=======
        
        hal_result_t r = hal_spiencoder_get_value2(hal_spiencoder1, &raw_value, &diag);
        
        //hal_result_t r = hal_spiencoder_get_value3(hal_spiencoder1, &raw_value, &raw_buff_0, &raw_buff_1, &raw_buff_2, &diag);
        
>>>>>>> f10575e65 (mc4plus: moved the tests for aea3 into a dedicated project)
        // print value
        char str[64] = {0};
        if(hal_res_OK == r)
        { 
<<<<<<< HEAD
            hal_spiencoder_position_t raw_value_shifted =  raw_value >> 1; // sensor return 16 bits --> only 14 are valid
            snprintf(str, sizeof(str), "%d, %d, %f", raw_value, raw_value_shifted, ((360.0 *  raw_value_shifted) / (1024.0*16)));  // (360*value)/(2^14)
        }
        else
        {
            snprintf(str, sizeof(str), "aea error during hal_spiencoder_get_value2");
        }

        hal_trace_puts(str);
                
        // start reading
        hal_spiencoder_read_start(hal_spiencoder1);
    }
=======
            snprintf(str, sizeof(str), "%d, %f", raw_value, ((360.0 *  raw_value) / (1024.0*16)));
            
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
    }
	
    // 
>>>>>>> f10575e65 (mc4plus: moved the tests for aea3 into a dedicated project)
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





