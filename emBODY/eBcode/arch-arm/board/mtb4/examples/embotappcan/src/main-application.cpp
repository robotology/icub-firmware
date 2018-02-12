/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
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
// - description of application
// --------------------------------------------------------------------------------------------------------------------
//
// this is a very simple application which waits for one can frame to arrive. 
// if the can frame arrives, it calls a callback which does an action.
// it may also send back a can frame.
// if the can frame does not arrive by the time of 500 ms, the application toggles a led to show that it is alive
// NOTE: we dont init the systick, hence we cannot use HAL_Delay(). 
// 


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stm32hal.h" 
    
#include "embot_common.h"
#include "embot_binary.h"
    
#include "embot_hw_bsp.h"
#include "embot_hw_sys.h"  
#include "embot_hw_can.h" 
#include "embot_hw_led.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "can_parser.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_canOnRXframe(void *p);
static void s_canOnTXqueueEmpty(void *p);

// returns true if we have a can frame to process. false if the timeout has expired
static bool s_waitForCANframe(embot::common::relTime timeout, embot::common::relTime reactivity);

static bool s_retrieveCANframes(void);

static bool s_transmitCANframes(embot::common::relTime timeout, embot::common::relTime reactivity);


static void s_tick1msecinit(void);
static uint32_t s_tick1msecget(void);



// --------------------------------------------------------------------------------------------------------------------
// - some constant variables. DONT USE MACROS !!!!!
// --------------------------------------------------------------------------------------------------------------------

static const std::uint8_t canQueueTXcapacity = 4;
static const std::uint8_t canQueueRXcapacity = 4;

static const embot::hw::led::LED heartbeatLED = embot::hw::led::LED::one;
static const embot::hw::led::LED canActivityLED = embot::hw::led::LED::two;

static const embot::common::relTime reactivityForCAN = 100*embot::common::time1microsec; 
static const embot::common::relTime timeoutForCANrx = 500*embot::common::time1millisec; // = 500000 ... but dont use numbers
static const embot::common::relTime timeoutforCANtx = 10*embot::common::time1millisec; 

// --------------------------------------------------------------------------------------------------------------------
// - some variables. 
// --------------------------------------------------------------------------------------------------------------------

static volatile bool aCANframeIsHere = false;
static volatile bool allCANframesHaveGONE = false;

static volatile uint64_t s_1mstickcount = 0;

// --------------------------------------------------------------------------------------------------------------------
// - the main
// --------------------------------------------------------------------------------------------------------------------



int main(void)
{ 
    // we init the BSP: we dont use any timing system. 
    // we cannot use embot::hw::bsp::now() or embot::hw::sys::now() but only embot::hw::sys::delay()
    const embot::hw::bsp::stm32halConfig stm32c(s_tick1msecinit, s_tick1msecget);
    embot::hw::bsp::Config cc(stm32c, nullptr); 
    embot::hw::bsp::init(cc);  
    
    
    // we init some LEDs    
    embot::hw::led::init(heartbeatLED);
    embot::hw::led::init(canActivityLED);
    
    
    // we init the CAN
    // description: the can driver allows to buffer up to 4 frames in reception. when a frame arrives the ISR calls 
    // a function which will manage can frame reception. if one wants to transmit, the tx buffer hosts up to 4 frames. 
    // when all frames are transmitted, a callback is called to alert that we can do other activities.
    
    embot::hw::can::Config canconfig;   
    canconfig.txcapacity = canQueueTXcapacity;
    canconfig.rxcapacity = canQueueRXcapacity;
    canconfig.onrxframe = embot::common::Callback(s_canOnRXframe, nullptr); 
    canconfig.ontxframe = embot::common::Callback(nullptr, nullptr);
    canconfig.txqueueempty = embot::common::Callback(s_canOnTXqueueEmpty, nullptr);
    
    embot::hw::can::init(embot::hw::can::Port::one, canconfig);    
    
        
    for(;;)
    {   
        embot::hw::led::toggle(heartbeatLED);    
        
        if(true == s_waitForCANframe(timeoutForCANrx, reactivityForCAN))
        {   // we have at least one can frame.            
            bool txback = s_retrieveCANframes();
            if(true == txback)
            {
                s_transmitCANframes(timeoutforCANtx, reactivityForCAN);
            }
            embot::hw::led::toggle(canActivityLED);   
        }            
    }
  
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions
// --------------------------------------------------------------------------------------------------------------------


    
void SysTick_Handler(void)
{
    s_1mstickcount++;
}

static void s_tick1msecinit(void)
{
    HAL_SYSTICK_Config(SystemCoreClock/1000);
}
    
static uint32_t s_tick1msecget(void)
{
    return (uint32_t)s_1mstickcount;
}



static void s_canOnRXframe(void *p)
{
    aCANframeIsHere =  true;
}


static void s_canOnTXqueueEmpty(void *p)
{
    allCANframesHaveGONE = true;
}

static bool s_waitForCANframe(embot::common::relTime timeout, embot::common::relTime reactivity)
{
    // hei: without the rtos we dont have embot::sys::timeNow() ... 
    // and the embot::hw namespace does not have it unless we program the systick
    // but we dont want the systick with the rtos ... hence ... never mind
    
    // check if we have a frame
    bool ret = false;
    
    if(embot::hw::can::inputqueuesize(embot::hw::can::Port::one) > 0)
    {
        ret = true;
        return ret;
    }
    
    // else we wait for small pieces of reactivity until we reach timeout or until we have a frame.        
    for(embot::common::relTime t=0; t<timeout; t += reactivity)
    {   
        if(true == aCANframeIsHere)
        {
            aCANframeIsHere = false;
            ret = true;;
            break;
        }  
            
        embot::hw::sys::delay(reactivity);
    }
 
    return(ret);
}


static bool s_retrieveCANframes(void)
{  
    // parse all the can frames ...
    std::uint8_t nframes = embot::hw::can::inputqueuesize(embot::hw::can::Port::one);    
    bool ret = false;
            
    for(int i=0; i<nframes; i++)
    {
        embot::hw::can::Frame frame;
        std::uint8_t remaining = 0;
        embot::hw::can::get(embot::hw::can::Port::one, frame, remaining);
        
        bool txback = can_parser_decode(frame.id, frame.size, frame.data);
        
        if(true == txback)
        {
            embot::hw::can::Frame reply;
            if(true == can_parser_getreply(&reply.id, &reply.size, reply.data))
            {
                ret = true;
                embot::hw::can::put(embot::hw::can::Port::one, reply);
            }                        
        }
        
    }
        
    return ret;
}


static bool s_transmitCANframes(embot::common::relTime timeout, embot::common::relTime reactivity)
{
    bool ret = false;
    
    // if we must, we transmit back
    if(embot::hw::can::outputqueuesize(embot::hw::can::Port::one) > 0)
    { 
        ret = true;
        embot::hw::can::transmit(embot::hw::can::Port::one);
        
        if(0 == timeout)
        {
            return ret;
        }
        
        // else wait until completion     
        for(embot::common::relTime t=0; t<timeout; t += reactivity)
        {   
            if(true == allCANframesHaveGONE)
            {
                allCANframesHaveGONE = false;
                break;
            }                  
            embot::hw::sys::delay(reactivity);
        }
        
    } 


    return ret;            
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




