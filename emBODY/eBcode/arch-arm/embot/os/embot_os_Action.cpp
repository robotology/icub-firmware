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

#include "embot_os_Action.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

namespace embot { namespace os { 
    
    bool EventToThread::isvalid() const
    {
        if((nullptr == task) || (0 == event)) 
        { 
            return false; 
        }
        else if(Thread::Type::eventTrigger != task->getType()) 
        { 
            return false; 
        }
        else 
        { 
            return true; 
        }
    }

    bool EventToThread::execute()
    {
        if(false == isvalid())
        {
            return false;
        }
        task->setEvent(event);
        return true;
    }
    
    bool MessageToThread::isvalid() const
    {
        if((nullptr == task) || (0 == message)) { return false; }
        else if(Thread::Type::messageTrigger != task->getType()) { return false; }
        else { return true; }
    } 
    
    bool MessageToThread::execute(core::relTime timeout)
    {
        if(false == isvalid())
        {
            return false;
        }
        task->setMessage(message, timeout);
        return true;
    } 
        

    bool CallbackToThread::isvalid() const 
    {
        if((false == callback.isvalid())) { return false; }
        //else if(Thread::Type::callbackTrigger != task->getType()) { return false; }
        else { return true; }
    } 
    
    
    bool CallbackToThread::execute(core::relTime timeout)
    {
        if(false == isvalid())
        {
            return false;
        }
        if((nullptr != task) && (embot::os::Thread::Type::callbackTrigger == task->getType()))
        {
            task->setCallback(callback, timeout);
        }
        else
        {
            callback.execute();
        }
        return true;
    }   
            
    void Action::load(const EventToThread &e)
    {
        type = Type::event2thread;
        evt = e;           
    }
        
    void Action::load(const MessageToThread &m)
    {
        type = Type::message2thread;
        msg = m;           
    }
    
    void Action::load(const CallbackToThread &c)
    {
        type = Type::callback2thread;
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
            
            case Type::event2thread:
            {
                ret = evt.isvalid();
            } break;

            case Type::message2thread:
            {
                ret = msg.isvalid();
            } break;
            
            case Type::callback2thread:
            {
                ret = cbk.isvalid();
            } break;
        }            
        return ret;            
    }
        
    bool Action::execute(core::relTime timeout)
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
            
            case Type::event2thread:
            {
                ret = evt.execute();
            } break;

            case Type::message2thread:
            {
                ret = msg.execute(timeout);
            } break;
            
            case Type::callback2thread:
            {
                ret = cbk.execute(timeout);
            } break;
        } 
        return ret;
    }

}} // namespace embot { namespace os {
 

    
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

