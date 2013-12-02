/* 
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero
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
#ifndef _MEMORY_MANAGER_H_
#define _MEMORY_MANAGER_H_

// - doxy begin -------------------------------------------------------------------------------------------------------
/**
    @file   memory_manager.h
    @brief  this file provides memory module's interface. 
            memory menager saves binary into a buffer and then write it into flash.
    @author valentina.gaggero@iit.it
    @date   06/17/2011
**/

// - external dependencies --------------------------------------------------------------------------------------------

#include "eEcommon.h"
#include "eEsharedInfo.h"


// - public #define  --------------------------------------------------------------------------------------------------
//empty section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef union mm_value32_t
    @brief      this type lets you use 32 bits in different ways: as one uint32_t, as two uint16_t, or as four uint8_t
 **/
typedef union
{
	uint32_t val32;

	struct
	{
		uint16_t LW;
		uint16_t HW;
	} __attribute__((packed)) word;

	uint8_t val[4];
} mm_value32_t;


/** @typedef    typedef struct mm_hex32file_lineinfo_t
    @brief      contains information that is present in a line of .hex file (HEX32 format)
 **/
typedef struct
{
    mm_value32_t   address;         /**< the address */
    uint8_t     num_bytes;          /**< the number of bytes that must be stores at address */
} mm_hex32file_lineinfo_t;
    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern void mm_init(const eEsharinfoPartable_t *pt);
    @brief      initialises memory manager module with data containing in bootLoader configuration.
    @param      pt          pointer to the partition table.
 **/
extern void mm_init(const eEsharinfoPartable_t *pt);



/** @fn         extern eEresult_t  mm_start_write(mm_hex32file_lineinfo_t *fLine_info_ptr);
    @brief      it is invoked on receiveing of first CMD_ADDRESS; it initialises all data structure needed to write in flash 
                but it does not write yet in flash as we dont have any data yet ...
    @param      fLine_info_ptr         pointer to ...
    @return     ee_res_NOK_generic if the address specified in fLine_info_ptr doesn't belog to a known flash section
                ee_res_OK otherwise
 **/
extern eEresult_t  mm_start_write(mm_hex32file_lineinfo_t *fLine_info_ptr);



/** @fn         extern eEresult_t mem_check_new_fileLineInfo(mm_hex32file_lineinfo_t *fLine_info_ptr);
    @brief      it is invoked every time CMD_ADDRESS is received except for the first time
                it verifies if binary data will arrive belong to the actual flash page.
                if so, increments buffer index whre data is saved provisionally;
                if not, writes data in buffer in the correct flash page and invokes mem_start_write  
    @param      fLine_info_ptr         pointer to .hex file data
    @return     ee_res_OK or ee_res_NOK_generic
 **/
extern eEresult_t mm_check_new_fileLineInfo(mm_hex32file_lineinfo_t *fLine_info_ptr);



/** @fn         extern eEresult_t mem_write_buffer(void);
    @brief      writes everything that has accumulated in the buffer
    @return     ee_res_OK or ee_res_NOK_generic
 **/
extern eEresult_t mm_write_buffer(void);


/** @fn         extern void mm_write_deviceConfig(void);
    @brief      writes device configuration bits in its flash section. (see datasheet for more information)
 **/
extern void mm_write_deviceConfig(void);


/** @fn         extern void mm_erase_programMem(void);
    @brief      erase program memory but only in application section defined in bootLoader configuration
 **/
extern void mm_erase_programMem(void);


/** @fn         extern void mm_erase_eeprom(void);
    @brief      erase emulated eeprom. only the section dedictaed to application
 **/
extern void mm_erase_eeprom(void);


/** @fn         extern eEresult_t mm_store_data(uint8_t *data, uint8_t data_size, mm_hex32file_lineinfo_t *fLine_info_ptr, uint8_t updateEE);
    @brief      save in buffer data arrived in CMD_DATA command 
    @param      data                  pointer to data to save  
                data_size             size of data
                fLine_info_ptr        pointer to .hex file data
    @return     ee_res_OK or ee_res_NOK_generic
 **/
extern eEresult_t mm_store_data(uint8_t *data, uint8_t data_size, mm_hex32file_lineinfo_t *fLine_info_ptr);


/** @fn         extern void mm_update_eeprom(uint8_t updateit);
    @brief      sets if eeprom must be updated with new data 
    @param      updateit            if = 1 eeprom will be update, if = 0 otherwise.  
 **/
extern void mm_update_eeprom(uint8_t updateit);


/** @fn         extern void mm_erase(void);
    @brief      erases program memory and eeprom memory if mem_update_eeprom(1) was invoked before 
 **/
extern void mm_erase(void);


/** @fn         extern void mm_erase_systemMem(void);
    @brief      erases program memory from addr 0x0 to 0x400. Between 0x0 and 0x200 there are startup address,
				ivt (interrupt vector table) and aivt (alternate interrupt vector table)
 **/
extern void mm_erase_systemMem(void);
// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


