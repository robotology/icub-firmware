
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




