/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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
#ifndef _BSP_H_
#define _BSP_H_

#ifdef __cplusplus
extern "C" {
#endif
    
    
#include "stdint.h"
#include "stdbool.h"    
    
extern int8_t bsp_init(void);
    
extern uint32_t bsp_sys_get_clock(void);

extern void bsp_sys_reset(void);
    

extern bool bsp_sys_jump2address(uint32_t address);
    

enum { bsp_ledblue = 0, bsp_ledred = 1, bsp_led_maxnum=2, bsp_led_none = 255 };

extern int8_t bsp_led_init(uint8_t led, const void *dummyparam);

extern int8_t bsp_led_on(uint8_t led);

extern int8_t bsp_led_off(uint8_t led);

extern int8_t bsp_led_toggle(uint8_t led);

enum { bsp_btn0 = 0, bsp_ntn_none = 255 };

extern int8_t bsp_button_pressed(uint8_t btn);



extern bool bsp_flashstorage_init(void);
extern bool bsp_flashstorage_isinitted(void);
extern bool bsp_flashstorage_isaddressvalid(uint32_t adr);
extern uint32_t bsp_flashstorage_getbaseaddress(void);
extern uint32_t bsp_flashstorage_getsize(void);
extern bool bsp_flashstorage_fullerase(void);
extern bool bsp_flashstorage_erase(uint32_t addr, uint32_t size);
extern bool bsp_flashstorage_read(uint32_t addr, uint32_t size, void *data);
extern bool bsp_flashstorage_write(uint32_t addr, uint32_t size, const void *data);

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


