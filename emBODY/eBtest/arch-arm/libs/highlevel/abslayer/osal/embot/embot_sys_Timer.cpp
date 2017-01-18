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
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_sys_Timer.h"




// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "EOtimer.h"
#include "EOVtheCallbackManager.h"

#include "embot_sys_Task.h"


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

    
struct embot::sys::Timer::Impl
{
    EOtimer *tmr;
    Impl() 
    {
        tmr = eo_timer_New();
    }
    ~Impl()
    {
        eo_timer_Delete(tmr);
    }
};


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------



embot::sys::Timer::Timer()
: pImpl(new Impl)
{   

}

embot::sys::Timer::~Timer()
{   
    delete pImpl;
}


bool embot::sys::Timer::start(common::relTime countdown, Type type, Action &onexpiry)
{
    EOaction_strg strg = {0};
    EOaction* action = (EOaction*)&strg;
    EOVtaskDerived *totask = NULL;
    
    switch(onexpiry.type)
    {
        case Action::Type::event2task:
        {            
            if(NULL != onexpiry.evt.task)
            {
                totask = (EOVtaskDerived*) onexpiry.evt.task->getEOMtask();
            }
            eo_action_SetEvent(action, onexpiry.evt.event, totask);
        } break;
        case Action::Type::message2task:
        {
            if(NULL != onexpiry.msg.task)
            {
                totask = (EOVtaskDerived*) onexpiry.msg.task->getEOMtask();
            }
            eo_action_SetMessage(action, onexpiry.msg.message, totask);
        } break;
        
        case Action::Type::executecallback:
        {
            totask = eov_callbackman_GetTask(eov_callbackman_GetHandle());
            eo_action_SetCallback(action, onexpiry.cbk.callback, onexpiry.cbk.arg, totask);
        } break;  
        
        default:
        {
            eo_action_Clear(action);
        } break;
    } 
    
    eOresult_t r = eo_timer_Start(pImpl->tmr, eok_abstimeNOW, countdown, static_cast<eOtimerMode_t>(type), action);    
    return (eores_OK == r) ? true : false;
}


bool embot::sys::Timer::stop()
{
    eOresult_t r = eo_timer_Stop(pImpl->tmr);    
    return (eores_OK == r) ? true : false;
}


embot::sys::Timer::Status embot::sys::Timer::getStatus()
{
    eOtimerStatus_t st = eo_timer_GetStatus(pImpl->tmr);    
    return static_cast<Timer::Status>(st);   
}


embot::sys::Timer::Type embot::sys::Timer::getType()
{
    eOtimerMode_t mode = eo_timer_GetMode(pImpl->tmr);    
    return static_cast<Timer::Type>(mode);   
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

