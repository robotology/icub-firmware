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

namespace embot { namespace sys {
    
        
    struct Action
    {
        enum class Type { none = 0, event2task = 1, message2task = 2, executecallback = 3 };
        
        struct EventToTask
        {
            common::Event   event;
            Task*           task;            
        };
        
        struct MessageToTask
        {
            common::Message message;
            Task*           task;            
        };
        
        struct ExecuteCallback
        {
            common::fpCallback    callback;
            void*           arg;
            Task*           task;            
        };  
        
        Type            type;
        union
        {
            EventToTask     evt;
            MessageToTask   msg;
            ExecuteCallback cbk;
        };
        
        
        Action(){ clear(); } 
        
        void clear() { type = Type::none; }
        
        void set(EventToTask &e)
        {
            type = Type::event2task;
            evt = e;           
        }
        
        void set(MessageToTask &m)
        {
            type = Type::message2task;
            msg = m;           
        }
        
        void set(ExecuteCallback &c)
        {
            type = Type::executecallback;
            cbk = c;           
        }
    };
    
}} // namespace embot { namespace sys {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
