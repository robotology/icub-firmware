/*
 * Copyright (C) 2019 iCub Facility - Istituto Italiano di Tecnologia
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

#include "embot_sys_Action.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

namespace embot { namespace sys { 
    
    bool EventToTask::isvalid() const
    {
        if((nullptr == task) || (0 == event)) 
        { 
            return false; 
        }
        else if(Task::Type::eventTrigger != task->getType()) 
        { 
            return false; 
        }
        else 
        { 
            return true; 
        }
    }

    bool EventToTask::execute()
    {
        if(false == isvalid())
        {
            return false;
        }
        task->setEvent(event);
        return true;
    }
    
    bool MessageToTask::isvalid() const
    {
        if((nullptr == task) || (0 == message)) { return false; }
        else if(Task::Type::messageTrigger != task->getType()) { return false; }
        else { return true; }
    } 
    
    bool MessageToTask::execute(common::relTime timeout)
    {
        if(false == isvalid())
        {
            return false;
        }
        task->setMessage(message, timeout);
        return true;
    } 
        

    bool CallbackToTask::isvalid() const 
    {
        if((false == callback.isvalid())) { return false; }
        //else if(Task::Type::callbackTrigger != task->getType()) { return false; }
        else { return true; }
    } 
    
    
    bool CallbackToTask::execute(common::relTime timeout)
    {
        if(false == isvalid())
        {
            return false;
        }
        if((nullptr != task) && (embot::sys::Task::Type::callbackTrigger == task->getType()))
        {
            task->setCallback(callback, timeout);
        }
        else
        {
            callback.execute();
        }
        return true;
    }   
            
    void Action::load(const EventToTask &e)
    {
        type = Type::event2task;
        evt = e;           
    }
        
    void Action::load(const MessageToTask &m)
    {
        type = Type::message2task;
        msg = m;           
    }
    
    void Action::load(const CallbackToTask &c)
    {
        type = Type::callback2task;
        cbk = c;           
    }
        

    bool Action::isvalid() const
    {
        bool ret = false;
        switch(type)
        {
            case Type::none:
            default:
            {
                ret = false;
            } break;
            
            case Type::event2task:
            {
                ret = evt.isvalid();
            } break;

            case Type::message2task:
            {
                ret = msg.isvalid();
            } break;
            
            case Type::callback2task:
            {
                ret = cbk.isvalid();
            } break;
        }            
        return ret;            
    }
        
    bool Action::execute(common::relTime timeout)
    {
        if(false == isvalid())
        {
            return false;
        }
        
        bool ret = false;
        switch(type)
        {
            case Type::none:
            default:
            {
            } break;
            
            case Type::event2task:
            {
                ret = evt.execute();
            } break;

            case Type::message2task:
            {
                ret = msg.execute(timeout);
            } break;
            
            case Type::callback2task:
            {
                ret = cbk.execute(timeout);
            } break;
        } 
        return ret;
    }

}} // namespace embot { namespace sys {
 

    
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

