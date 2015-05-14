
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



#include "stdlib.h"
#include "string.h"
#include "EoCommon.h"

#include "EOMtheEMSappl.h"

#include "osal_system.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSrunner.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSrunner_hid.h" 


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



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static void s_xxx_delay(eOreltime_t delay);

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

static volatile uint32_t xxxcount = 0;

extern void eom_emsrunner_hid_userdef_taskRX_activity_beforedatagramreception(EOMtheEMSrunner *p)
{
    xxxcount ++;
}

extern void eom_emsrunner_hid_userdef_taskRX_activity_afterdatagramreception(EOMtheEMSrunner *p)
{
    static uint32_t totalreceived = 0;
    
    totalreceived += p->numofrxpackets;
    
    if(3 == totalreceived)
    {
        totalreceived = 0;
        eom_emsrunner_StopAndGoTo(eom_emsrunner_GetHandle(), eo_sm_emsappl_EVgo2cfg);
    } 
    
    static uint32_t totalcalled = 0;
    
    totalcalled++;
    
    if(2 == totalreceived)
    {
        eom_emsrunner_SetMode(p, eo_emsrunner_mode_softrealtime);
    }
    
    
    if(0 == (xxxcount % 10))
    {
        //s_xxx_delay(600);
        //s_xxx_delay(200);
    }
    else
    {
        s_xxx_delay(300);
    }
    


    
}



extern void eom_emsrunner_hid_userdef_taskDO_activity(EOMtheEMSrunner *p)
{
#if 0
    volatile eObool_t quit = eobool_false;
    //volatile eObool_t* touched = &p->safetyGAPtouched[eo_emsrunner_taskid_runDO];
    
    for(;;)
    {
        s_xxx_delay(20);
        //if(eobool_true == *touched)
        if(eobool_true == eom_emsrunner_SafeDurationExpired(eom_emsrunner_GetHandle(), eo_emsrunner_taskid_runDO))
        {
            return;
        }        
    }
#else
//     static uint32_t xcount = 0;
//     
//     xcount++;
//     if(0 == (xcount % 10))
//     {
//         s_xxx_delay(500);
//     }
//     else
//     {
//         s_xxx_delay(100);
//     }
    
    s_xxx_delay(100+150);
#endif    
    //     
//     //for(i=0; i<1000*2; i++)
//     for(i=0; i<2000; i++) // 200 microsec
//     {
//         aaa = aaa*1.02f;
//         aaa = aaa/1.0345f;
//         if(eobool_true == *touched)
//         {
//             return;
//         }
//     }    
}


extern void eom_emsrunner_hid_userdef_taskTX_activity_beforedatagramtransmission(EOMtheEMSrunner *p)
{
     

    if(0 == (xxxcount % 20))
    {
        //s_xxx_delay(3500);
        s_xxx_delay(150);
//         s_xxx_delay(50);
//         s_xxx_delay(50);
//         s_xxx_delay(50);
//         s_xxx_delay(50);
//         s_xxx_delay(50);
//         s_xxx_delay(50);
//         s_xxx_delay(50);
//         s_xxx_delay(50);
//         s_xxx_delay(50);
//         s_xxx_delay(50);
    }
    else
    {
    //    s_xxx_delay(50);
    }
}


extern void eom_emsrunner_hid_userdef_taskTX_activity_afterdatagramtransmission(EOMtheEMSrunner *p)
{
     

    if(0 == (xxxcount % 20))
    {
        //s_xxx_delay(3500);
        //s_xxx_delay(400);
         s_xxx_delay(50);
//         s_xxx_delay(50);
//         s_xxx_delay(50);
//         s_xxx_delay(50);
//         s_xxx_delay(50);
//         s_xxx_delay(50);
//         s_xxx_delay(50);
//         s_xxx_delay(50);
//         s_xxx_delay(50);
//         s_xxx_delay(50);
    }
    else
    {
        s_xxx_delay(50);
    }
}





// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_xxx_delay(eOreltime_t delay)
{

#if 1
    uint64_t startat = osal_system_nanotime_get();
    uint64_t stopat = startat + (uint64_t)delay*1000;
    volatile uint64_t now = startat;
    volatile uint64_t problemat = 0;
    volatile uint64_t tmp = 0;
   

    for(;;)
    {
        
        if(now >= stopat)
        {
            break;
        } 
//         if((stopat-now) > delay)
//         {
//             if(0 == problemat)
//             {
//                 problemat = now;
//             }
//         }
        tmp = osal_system_nanotime_get();
        now = tmp;///1000ll;        
    } 
#else

    uint64_t startat = osal_system_abstime_get();
    uint64_t stopat = startat + (uint64_t)delay;
    volatile uint64_t now = startat;
    volatile uint64_t problemat = 0;
    volatile uint64_t tmp = 0;
   

    for(;;)
    {
        
        if(now >= stopat)
        {
            break;
        } 
//         if((stopat-now) > delay)
//         {
//             if(0 == problemat)
//             {
//                 problemat = now;
//             }
//         }
        tmp = osal_system_abstime_get();
        now = tmp;        
    } 

#endif    
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




