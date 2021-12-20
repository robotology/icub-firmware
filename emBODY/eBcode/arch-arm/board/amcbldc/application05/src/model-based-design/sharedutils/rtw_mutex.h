/*
 * Copyright (C) 2021 iCub Tech Facility - Istituto Italiano di Tecnologia
 * Author:  Ugo Pattacini
 * email:   ugo.pattacini@iit.it
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

#ifndef RTW_MUTEX_H
#define RTW_MUTEX_H

#include "stm32hal.h"

#define rtw_mutex_init()
#define rtw_mutex_destroy()

inline bool IsException(void) 
{
    return(__get_IPSR() != 0U);
}

inline void rtw_mutex_lock(void) 
{    
    if(false == IsException())
    {
        NVIC_DisableIRQ(DMA1_Channel2_IRQn);  
            
    }  
}

inline void rtw_mutex_unlock(void) 
{
    if(false == IsException())
    {       
        NVIC_EnableIRQ(DMA1_Channel2_IRQn);
    }  
}

#endif // RTW_MUTEX_H

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
