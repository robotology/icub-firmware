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

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_OS_ACTION_H_
#define _EMBOT_OS_ACTION_H_

#include "embot_core.h"
#include "embot_os_common.h"


namespace embot { namespace os {
    
    struct EventToThread
    {
        os::Event event {0};
        Thread* task {nullptr};  
        
        EventToThread() = default;
        EventToThread(os::Event e, Thread* t) : event(e), task(t) {}
        bool isvalid() const;
        bool execute();
    };
    
    struct MessageToThread
    {
        os::Message message {nullptr};
        Thread* task {nullptr};
        
        MessageToThread() = default;  
        MessageToThread(os::Message m, Thread* t) : message(m), task(t) {}    
        bool isvalid() const;
        bool execute(core::relTime timeout = core::reltimeWaitForever);
    };

    
    struct ValueToThread
    {
        os::Value value {0};
        Thread* task {nullptr};
        
        ValueToThread() = default;  
        ValueToThread(os::Value v, Thread* t) : value(v), task(t) {}    
        bool isvalid() const;
        bool execute(core::relTime timeout = core::reltimeWaitForever);
    };    
    
    struct CallbackToThread
    {
        core::Callback callback {nullptr, nullptr};
        Thread* task {nullptr};  

        CallbackToThread() = default;        
        CallbackToThread(core::fpCaller c, void *a, Thread *t) : callback(c, a), task(t) {}  
        CallbackToThread(core::Callback cbk, Thread *t) : callback(cbk), task(t) {}                   
        bool isvalid() const;
        bool execute(core::relTime timeout = core::reltimeWaitForever);
    };  
            

    struct Action
    {
        enum class Type { none = 0, event2thread = 1, message2thread = 2, callback2thread = 3, value2thread = 4 };
               
        Type type {Type::none};
        union
        {
            EventToThread     evt;
            MessageToThread   msg;
            ValueToThread     val;
            CallbackToThread  cbk;
        };
               
        Action() { clear(); } 
        Action(const CallbackToThread &c) { load(c); }
        Action(const MessageToThread &m) { load(m); }
        Action(const ValueToThread &v) { load(v); }
        Action(const EventToThread &e) { load(e); }        
        void clear() { type = Type::none; }        
        void load(const EventToThread &e);        
        void load(const MessageToThread &m); 
        void load(const ValueToThread &v);        
        void load(const CallbackToThread &c);               
        bool isvalid() const;        
        bool execute(core::relTime timeout = core::reltimeWaitForever);
    };
    
}} // namespace embot { namespace os {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
