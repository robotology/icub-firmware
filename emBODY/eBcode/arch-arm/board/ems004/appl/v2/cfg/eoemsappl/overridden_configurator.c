
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


// simone.girardi on 8 Nov 2021: with the following macro enabled we can test the reading on aea3 encoder.
// with a minimum effort we can use it to test other similar encoders (e.g. aea1, aea2 ...)  as well.
//#define TEST_SPI_ENCODER
//#define TEST_AEA
//#define TEST_AEA3
//#define TEST_AKSIM2

#ifdef TEST_SPI_ENCODER

#include <hal_spiencoder.h>
#include <hal_trace.h>

void cbk(void *p)
{
    static volatile uint32_t v = 0;
    v++;
}

hal_spiencoder_cfg_t cfg = {0};
hal_spiencoder_diagnostic_t diag = {0};

// This function is called every 10 ms (see eom_emsconfigurator_Initialise)
extern void eom_emsconfigurator_hid_userdef_ProcessTimeout(EOMtheEMSconfigurator* p)
{
    static volatile uint32_t counter = 0;
    static hal_result_t res;
    static uint8_t initted = 0;
    
    if(0 == counter)
    {
        // init spiencoder
        memmove(&cfg, &hal_spiencoder_cfg_default, sizeof(hal_spiencoder_cfg_default));
        
        cfg.callback_on_rx       = cbk;
        cfg.priority             = hal_int_priority05;
        cfg.arg                  = NULL;
        
        #if defined(TEST_AEA3)
        cfg.type	                = hal_spiencoder_typeAEA3;
        #elif defined(TEST_AEA)
        cfg.type	                = hal_spiencoder_typeAEA;
        #elif defined(TEST_AKSIM2)
        cfg.type                    = hal_spiencoder_typeAksIM2;
        #endif
        cfg.sdata_precheck       = hal_false;  
        cfg.reg_addresses[0]	    = 0;
        cfg.reg_addresses[1]	    = 0; 
        
        hal_spiencoder_init(hal_spiencoder1, &cfg);
        
        diag.type = hal_spiencoder_diagnostic_type_none;
    }
    else if(0 == (counter % 10))
    {
        // retrieve the reading
        hal_spiencoder_position_t raw_value = {0};
        hal_result_t r = hal_spiencoder_get_value2(hal_spiencoder1, &raw_value, &diag);
        
        // print value
        char str[64] = {0};
        if(hal_res_OK == r)
        { 
            #if defined(TEST_AEA3)
                hal_spiencoder_position_t raw_value_shifted =  raw_value >> 1; // sensor return 16 bits --> only 14 are valid
                snprintf(str, sizeof(str), "%d, %d, %f", raw_value, raw_value_shifted, ((360.0 *  raw_value_shifted) / (1024.0*16)));  // (360*value)/(2^14)
            #elif defined(TEST_AEA)
                hal_spiencoder_position_t raw_value_shifted =  (raw_value >> 6) & 0x0FFF; // sensor return 16 bits --> only 12 are valid
                snprintf(str, sizeof(str), "%d, %d, %f", raw_value, raw_value_shifted, ((360.0 *  raw_value_shifted) / (1024.0*4)));  // (360*value)/(2^14)
            #elif defined(TEST_AKSIM2)
                snprintf(str, sizeof(str), "%d", raw_value); // it now prints only the position Value
            #endif
        }
        else
        {
            snprintf(str, sizeof(str), "spiencoder error during hal_spiencoder_get_value2");
        }
        
        hal_trace_puts(str);
                
        // start reading
        hal_spiencoder_read_start(hal_spiencoder1);
            
    }
    counter++;
}
#endif


#undef TEST_000

#if defined(TEST_000)

extern void eom_emsconfigurator_hid_userdef_ProcessTickEvent(EOMtheEMSconfigurator* p)
{
    eo_errman_Trace(eo_errman_GetHandle(), "tick of cfg", NULL);     
}


#include "hal_gyroscope.h"
#include "hal_sys.h"
#include "EOtheSharedHW.h"

const char s_object[] = "EoTheGyroReader";

extern void eom_emsconfigurator_hid_userdef_ProcessTimeout(EOMtheEMSconfigurator* p)
{
    return;
    const int freq = 2;
    
    static uint32_t times = 0;
    char s[100] = {0};
    hal_result_t res;

    
    if(0 == times)
    {
        // init
        hal_gyroscope_cfg_t config;
        config.range = hal_gyroscope_range_2000dps;
        const char * ranges[] = {"0250dps", "0500dps", "2000dps"};
        
        eo_sharedhw_Initialise(NULL);
        
        if(eores_OK == eo_sharedhw_Obtain(eo_sharedhw_GetHandle(), eosharedhw_resource_I2C3, eok_reltimeZERO))
        {   // if the semaphore is busy ... i dont want to wait 
            res = hal_gyroscope_init(hal_gyroscope1, &config);   

            eo_sharedhw_Release(eo_sharedhw_GetHandle(), eosharedhw_resource_I2C3);            
            
            snprintf(s, sizeof(s), "GYRO iter %d -> init of gyro w/ range %s: %s", times, ranges[config.range], (hal_res_OK == res) ? "OK" : "KO");    
            eo_errman_Trace(eo_errman_GetHandle(), s, s_object);   
        }
        else
        {
            // we cannot init .... i dont incremnet times ...
            snprintf(s, sizeof(s), "GYRO iter %d -> i2c3 is busy: i dont init for now", times); 
            eo_errman_Trace(eo_errman_GetHandle(), s, s_object); 
            return;            
        }       
    }
    else if(0 == (times % freq))
    {
        int16_t x;
        int16_t y;
        int16_t z;
        hal_gyroscope_angularrate_t ang;
        
//        const osal_reltime_t wait = osal_reltimeZERO;
        const eOreltime_t wait = eok_reltimeINFINITE;
                
        if(eores_OK == eo_sharedhw_Obtain(eo_sharedhw_GetHandle(), eosharedhw_resource_I2C3, wait))
        {   // if the semaphore is busy ... i dont want to wait 
            eo_errman_Trace(eo_errman_GetHandle(), "i2c3 is taken", s_object);
            res = hal_gyroscope_readraw(hal_gyroscope1, &x, &y, &z);   
            res = hal_gyroscope_read(hal_gyroscope1, &ang); 
            eo_errman_Trace(eo_errman_GetHandle(), "i2c3 is released", s_object);            
            eo_sharedhw_Release(eo_sharedhw_GetHandle(), eosharedhw_resource_I2C3); 
            
            snprintf(s, sizeof(s), "GYRO iter %d -> %s w/ raw = (%d %d %d) md/s = (%d %d %d)", times, (hal_res_OK == res) ? "OK" : "KO", x, y, z, ang.xar, ang.yar, ang.zar);
            eo_errman_Trace(eo_errman_GetHandle(), s, s_object); 
        }
        else
        {
            // we cannot init .... i dont incremnet times ...
            snprintf(s, sizeof(s), "GYRO iter %d -> i2c3 is busy: i dont read at this iteration", times);
            eo_errman_Trace(eo_errman_GetHandle(), s, s_object);           
        }                      
    }
    
    times++;
}

#endif // TEST_000

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




