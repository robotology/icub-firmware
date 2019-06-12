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

#ifndef _EMBOT_SYS_ACTION_H_
#define _EMBOT_SYS_ACTION_H_

#include "embot_common.h"

#include "embot_sys.h"

#include "embot_sys_Task.h"


namespace embot { namespace sys {
    
    struct EventToTask
    {
        common::Event       event;
        Task*               task;  
        EventToTask(common::Event e, Task* t) : event(e), task(t) {}  
        EventToTask() : event(0), task(nullptr) {}
        bool isvalid() const;
        bool execute();
    };
    
    struct MessageToTask
    {
        common::Message     message;
        Task*               task;
        MessageToTask(common::Message m, Task* t) : message(m), task(t) {}   
        MessageToTask() : message(0), task(nullptr) {}   
        bool isvalid() const;
        bool execute(common::relTime timeout = common::timeWaitForever);
    };
    
    struct CallbackToTask
    {
        common::Callback    callback;
        Task*               task;       
        CallbackToTask(common::fpCaller c, void *a, Task *t) : callback(c, a), task(t) {}  
        CallbackToTask(common::Callback cbk, Task *t) : callback(cbk), task(t) {}                
        CallbackToTask() : callback(nullptr, nullptr), task(nullptr) {}   
        bool isvalid() const;
        bool execute(common::relTime timeout = common::timeWaitForever);
    };  
            

    struct Action
    {
        enum class Type { none = 0, event2task = 1, message2task = 2, callback2task = 3 };
               
        Type type;
        union
        {
            EventToTask     evt;
            MessageToTask   msg;
            CallbackToTask  cbk;
        };
               
        Action(){ clear(); } 
        Action(const CallbackToTask &c) { load(c); }
        Action(const MessageToTask &m) { load(m); }
        Action(const EventToTask &e) { load(e); }
        
        void clear() { type = Type::none; }
        
        void load(const EventToTask &e);
        
        void load(const MessageToTask &m);
        
        void load(const CallbackToTask &c);
               
        bool isvalid() const;
        
        bool execute(common::relTime timeout = common::timeWaitForever);
    };
    
}} // namespace embot { namespace sys {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
