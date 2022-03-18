
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "servicetester.h"


#if defined(enableTHESERVICETESTER)

#include "EOMtheEMSconfigurator.h"
#include "embot_app_eth_theServiceTester.h"

#define TESTtheFTservice

#endif


#if defined(TESTtheFTservice)
#include "embot_app_eth_theFTservice.h"
#endif

extern void servicetester_init(void)
{
#if !defined(enableTHESERVICETESTER)
// do nothing
#elif defined(TESTtheFTservice)
    const embot::app::eth::theServiceTester::Config &cfg = embot::app::eth::theFTservice::getInstance().servicetesterconfig();
    embot::app::eth::theServiceTester::getInstance().initialise(cfg);    
#endif    
}

extern void servicetester_runtick(void)
{   
#if !defined(enableTHESERVICETESTER)
// do nothing
#else
    embot::app::eth::theServiceTester::getInstance().tick(embot::app::eth::theServiceTester::Caller::threadRUN);
#endif    
}


extern void servicetester_cfgtick(void)
{    
#if !defined(enableTHESERVICETESTER)
// do nothing
#else
    embot::app::eth::theServiceTester::getInstance().tick(embot::app::eth::theServiceTester::Caller::threadCFG);
#endif    
}


#if !defined(enableTHESERVICETESTER)
// nothing
#else 
extern "C"
{
    extern void eom_emsconfigurator_hid_userdef_ProcessUserdef03Event(EOMtheEMSconfigurator* p)
    {
        servicetester_cfgtick();
    }
}
#endif


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

