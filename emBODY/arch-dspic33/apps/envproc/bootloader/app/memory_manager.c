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

// --------------------------------------------------------------------------------------------------------------------
// - doxy
// --------------------------------------------------------------------------------------------------------------------

/* @file       memory_menager.c
    @brief      this file implements memory manager module. It saves binary into a buffer and then write it into flash.
    @author     valentina.gaggero@iit.it
    @date       06/17/2011
**/

// acemor used teh datastruct available in eEcommon.h and cleaned a bit.

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <string.h>

#include <p33fj128mc802.h>
#include <libpic30.h>
#include <dsp.h>

#include "stdint.h"

#include "eEsharedInfo.h"

// acemor removed and placed it outside
// #include "hal_led.h"


extern void WriteLatchCM(uint16_t a, uint16_t b, uint16_t c);  // implemented in assembler in memory.s
extern void WriteMem(uint16_t a);                          // implemented in assembler in memory.s




/*
The following helper functions for erasing and writing to FLASH memory have been added to libpic30.a:
_erase_flash() - erase a page of FLASH memory
_write_flash16() - write a row of FLASH memory with 16-bit data
_write_flash24() - write a row of FLASH memory with 24-bit data
_write_flash_word16() - write a word of FLASH memory with 16-bit data
_write_flash_word24() - write a word of FLASH memory with 24-bit data
*/

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "memory_manager.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


//MACRO depend on dpic33f's flash

/* NOTE: for dspic33fJ128MC802 
    - smallest PM Erase size = 1 page = 512 instructions = 1536 bytes
    - smallest PM program size = 1 Row = 64 instructions = 92 bytes
 */


#define PM_ROWSIZE              64                      // number of instructions for row
#define PM_BYTE_PER_UNIT        4                       // one instruction = 3bytes + 1 phantom byte
#define PM_PAGE_ERASE_SIZE      512                     // number of instructions for page
#define PM_ROWS_PER_PAGE        PM_PAGE_ERASE_SIZE/PM_ROWSIZE //number of row in a flash page


// since in dspic33fJ128MC802 there isn't eeprom, it is simulated with flash
// so these values are equal to program memory values. 
#define EE_ROWSIZE				PM_ROWSIZE
#define EE_BYTE_PER_UNIT        PM_BYTE_PER_UNIT   
#define EE_PAGE_ERASE_SIZE      PM_PAGE_ERASE_SIZE

// these addesses can not be change. they depend on the microcontroller.
#define START_CM                0xF80000                // First address for Configuration registers (7 x 16 bits)
#define END_CM                  0xF80017                // Last address for Configuration registers
#define CM_BYTE_PER_UNIT        2                       // Configuration memory can be write in unit(word) of 2 bytes 
#define CM_ROWSIZE              (END_CM - START_CM)     // Row size for Configuratione memory: 7 word (1 word = 16 bit)  


#define CONFIG_REG_WRITE        0x4000

//this address can not be change. Depende on micro.
//in this memory is progma memory, but it contains system infomation: there are startup address, ivt and aivt
#define START_SYSPM                0x0                
#define END_SYSPM                  0x200              


// define about configuration bits

#define FBS                     0xF80000
#define FSS                     0xF80002
#define FGS 	                0xF80004
#define FOSCSEL                 0xF80006
#define FOSC                    0xF80008
#define FWDT                    0xF8000A
#define FPOR                    0xF8000C
#define FICD				    0xF8000E
#define FUID0				    0xE80010
#define FUID1				    0xE80012
#define FUID2				    0xE80014
#define FUID3				    0xE80016

#define FOSC_MASK               0x73    //0xC71F
#define FWDT_MASK               0xdf    //0x803F
#define FWDT_EN                 (1<<7)
#define FBORPOR_MASK            0xff
//#define FBS_MASK                0x310F
//#define FSS_MASK                0x330F
#define FGS_MASK                0x0007
#define FICD_MASK               0xe3    //0xC003


// shortcuts
#if defined(UPDTBL)
#define BOOTLDR_ADDR        (s_mm_data.partable->proc_application.addr)
#else
#define BOOTLDR_ADDR        (s_mm_data.partable->proc_loader.addr)
#endif

#define START_EE            (s_mm_data.partable->strg_priv_application.addr)
#define SIZE_EE             (s_mm_data.partable->strg_priv_application.size)
#define END_EE              (s_mm_data.partable->strg_priv_application.addr + s_mm_data.partable->strg_priv_application.size)

#define START_PM            (s_mm_data.partable->proc_application.addr)
#define SIZE_PM             (s_mm_data.partable->proc_application.size)
#define END_PM              (s_mm_data.partable->proc_application.addr + s_mm_data.partable->proc_application.size)


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

// this type contains all information necessary to memory manager in run time
typedef struct
{
    const eEsharinfoPartable_t*    partable;
    uint8_t                     update_ee;
} mem_datastruct_t;

// this type lets you see a buffer (vector/array) partitioned in bytes or in int16 or in int32 
typedef union 
{
    uint8_t asByte[PM_ROWSIZE*PM_BYTE_PER_UNIT];
    int16_t asWord16[PM_ROWSIZE*PM_BYTE_PER_UNIT/2];
    int32_t asWord32[PM_ROWSIZE*PM_BYTE_PER_UNIT/4];
} dataBuffer_t;


typedef struct
{
    mm_value32_t addr;
    mm_value32_t value;
} mem_devConf_datastruct_t;


typedef struct
{
    uint8_t     row_size;   // ROW is the smallest writable unit. it is expressed in number of instructions
    uint8_t     byte4unit; 
    uint32_t    start_addr;
    uint32_t    end_addr;
} mem_section_info_t;


typedef enum
{
    Program_MemT        = 0,    // Program memory type
    Eeprom_MemT         = 1,    // Eeprom memory type 
    DevConf_MemT        = 2,     // Device Configuration memory type
    System_MemT         = 3     // Device Configuration memory type
} memory_type_t;

enum { memory_type_max_num = 4 };




typedef struct
{
    dataBuffer_t    data;           // the data 
    uint16_t        index;          // counter for received bytes 
    uint8_t         byte_cntr;      // counter of bytes of each inscruction. (1 instruction = 3 bytes + 1 phantom byte)
} mem_buffer_datastruct_t;

// Address range to be programmed
typedef struct
{
    uint32_t        StartAddr;                  
    uint32_t        EndAddr;
    memory_type_t   mem_type;
} memory_writable_uinit_t;  // one ROW


typedef struct
{
    mem_devConf_datastruct_t    data[CM_ROWSIZE];
    uint16_t                    index;              // counter for received bytes  
    uint8_t                     byte_cntr;          // counter for received bytes 
} mem_devConf_buffer_datastruct_t;



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

//#warning -> acemor: siamo sicuri che sia possibile dichiarare un inline e poi definirlo dopo?
static inline void s_set_buff_index(uint32_t address);

static eEresult_t s_storeCM(uint8_t *data, uint8_t data_size, mm_hex32file_lineinfo_t *fLine_info_ptr);
static eEresult_t s_storePM(uint8_t *data, uint8_t data_size, mm_hex32file_lineinfo_t *fLine_info_ptr);
static eEresult_t s_storeEE(uint8_t *data, uint8_t data_size, mm_hex32file_lineinfo_t *fLine_info_ptr);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// ==> contains data to write in program or eeprom memory.
// NOTE: since eeprom is simulated, eeprom memory is a part of program memory, 
//       therefore we use the same buffer.
static mem_buffer_datastruct_t s_mm_buffer; 

// ==> contains data to write in device configuration bits
static mem_devConf_buffer_datastruct_t s_mm_devConf_buffer; 

// ==> contains start and end addresses of actual writable unit, that is FLASH ROW 
static memory_writable_uinit_t s_mm_actual_wUnit;



// Note: following fields set with 0, will be configured in init function
static mem_section_info_t s_mm_sectionMem_info[memory_type_max_num]=
{
    //PROGRAM MEMORY SECTION
    {
        PM_ROWSIZE,         //uint8_t row_size
        PM_BYTE_PER_UNIT,   //uint8_t byte4unit
        0,                  //uint32_t start_addr
        0,                  //uint32_t end_addr
    },
    //EEPROM MEMORY SECTION
    {
        EE_ROWSIZE,         //uint8_t row_size
        EE_BYTE_PER_UNIT,   //uint8_t byte4unit
        0,                  //uint32_t start_addr
        0,                  //uint32_t end_addr
    },
    //DEVICE MEMORY SECTION
    {
        CM_ROWSIZE,         //uint8_t row_size
        CM_BYTE_PER_UNIT,   //uint8_t byte4unit
        START_CM,           //uint32_t start_addr
        END_CM              //uint32_t end_addr
    },

    //SYSTEM MEMORY SECTION
    {
        PM_ROWSIZE,         //uint8_t row_size
        PM_BYTE_PER_UNIT,   //uint8_t byte4unit
        START_SYSPM,       //uint32_t start_addr
        END_SYSPM-1        //uint32_t end_addr     TODO: verifica il meno uno
    }
};

// this variable contains all information necessary to memory manager
static mem_datastruct_t s_mm_data;

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern void mm_init(const eEsharinfoPartable_t* pt)
{
    s_mm_data.partable = pt;
    s_mm_data.update_ee = 0;

    s_mm_sectionMem_info[Program_MemT].start_addr = START_PM;
    s_mm_sectionMem_info[Program_MemT].end_addr = END_PM;

    s_mm_sectionMem_info[Eeprom_MemT].start_addr = START_EE;
    s_mm_sectionMem_info[Eeprom_MemT].end_addr = END_EE;
 
}

extern void mm_update_eeprom(uint8_t updateit)
{
    s_mm_data.update_ee = updateit;
}

extern void mm_erase(void)
{
    mm_erase_programMem();
    mm_erase_systemMem();
    
    if(s_mm_data.update_ee)
    {
        mm_erase_eeprom();
    }
}


extern eEresult_t mm_start_write(mm_hex32file_lineinfo_t *fLine_info_ptr)
{
    uint16_t RowNumber;


    if(fLine_info_ptr->address.val32 >= START_PM && fLine_info_ptr->address.val32 < END_PM)
    {
        s_mm_actual_wUnit.mem_type = Program_MemT;       //For address in program memory        
    }
    else if(fLine_info_ptr->address.val32 >= START_EE && fLine_info_ptr->address.val32 < END_EE)
    {
        s_mm_actual_wUnit.mem_type = Eeprom_MemT;       //For address in EEPROM
    }    
    else if(fLine_info_ptr->address.val32 >= START_CM && fLine_info_ptr->address.val32 < END_CM)
    {
        s_mm_actual_wUnit.mem_type = DevConf_MemT;      //For address in configuration memory
        //init buffer
        memset(&(s_mm_devConf_buffer.data), 0, sizeof(mem_devConf_datastruct_t) * CM_ROWSIZE);
        s_mm_devConf_buffer.index = 0;
    }
    else if(fLine_info_ptr->address.val32 >= START_SYSPM && fLine_info_ptr->address.val32 < END_SYSPM)
    {
        s_mm_actual_wUnit.mem_type = System_MemT;
     }    
    else
    {
        return(ee_res_NOK_generic);
    }


    //if I'm here address belong to Progrm_mem or eeprom_mem

    //Calculate new row address range, the correct index for the buffer and update actual_row parameter

    /*the memory is addressed by PC units, ROWSIZE are in instruction number. 
      1 instruction = 2 PC units.
      Is for this, we moltiply row_size for 2.*/
	/* floor:function provide by  mplab C30*/
    RowNumber = floor((fLine_info_ptr->address.val32 - (s_mm_sectionMem_info[s_mm_actual_wUnit.mem_type].start_addr) )/(2*(s_mm_sectionMem_info[s_mm_actual_wUnit.mem_type].row_size)))+1;
    
    s_mm_actual_wUnit.StartAddr = (RowNumber-1) * 2 *(s_mm_sectionMem_info[s_mm_actual_wUnit.mem_type].row_size) + 
                             (s_mm_sectionMem_info[s_mm_actual_wUnit.mem_type].start_addr);

    s_mm_actual_wUnit.EndAddr = s_mm_actual_wUnit.StartAddr +(s_mm_sectionMem_info[s_mm_actual_wUnit.mem_type].row_size-1)*2;

    if(s_mm_actual_wUnit.mem_type != DevConf_MemT)
    {
        s_mm_buffer.index = (fLine_info_ptr->address.val32 - s_mm_actual_wUnit.StartAddr)*2;
        //-  init buffer: set bits 1 is equal to blank memory.
        //-  num of byte of buffer is PM_ROWSIZE*PM_BYTE_PER_UNIT both program and eeprom memory because, 
        //   eeprom is simulated with program memory
        memset(&s_mm_buffer.data.asByte, 0xFF, PM_ROWSIZE*PM_BYTE_PER_UNIT);
    }

    return(ee_res_OK);
}




extern eEresult_t mm_check_new_fileLineInfo(mm_hex32file_lineinfo_t *fLine_info_ptr)
{
    eEresult_t res = ee_res_OK;

    if(!((fLine_info_ptr->address.val32 >= s_mm_actual_wUnit.StartAddr) && (fLine_info_ptr->address.val32 <= s_mm_actual_wUnit.EndAddr) ) )
    {
        mm_write_buffer(); //flush_buffer
    
        res = mm_start_write(fLine_info_ptr);
    }
    else
    {
        s_set_buff_index(fLine_info_ptr->address.val32 - s_mm_actual_wUnit.StartAddr);
    }
    return(res);
}



extern eEresult_t mm_write_buffer(void)
{
    if(DevConf_MemT == s_mm_actual_wUnit.mem_type)
    {
        return(ee_res_NOK_generic); //this function must be invoked on PM and EE memory only.
                                   //appropriete funcion exists to flash DEVConf memory.
    }

    if(0 != s_mm_buffer.index)
    {
        _write_flash24(s_mm_actual_wUnit.StartAddr, s_mm_buffer.data.asWord32 );
        
        s_mm_buffer.index = 0;
        
        //hal_led_toggle(led_red);
    }
    return(ee_res_OK);
}


extern eEresult_t mm_store_data(uint8_t *data, uint8_t data_size, mm_hex32file_lineinfo_t *fLine_info_ptr)
{
    eEresult_t res = ee_res_OK;

    if(s_mm_actual_wUnit.mem_type == Program_MemT || s_mm_actual_wUnit.mem_type == System_MemT)
    {
		res= s_storePM(data, data_size, fLine_info_ptr);
	}
    else if(s_mm_actual_wUnit.mem_type == Eeprom_MemT)
    {
		res = s_storeEE(data, data_size, fLine_info_ptr);
	}
    else if(s_mm_actual_wUnit.mem_type == DevConf_MemT)
    {
        res = s_storeCM(data, data_size, fLine_info_ptr);
	}

    return(res);
}



//Reserved configuration bits must be programmed as 1
//Unimplemented bits must be programmed with 0
extern void mm_write_deviceConfig(void)
{
    int16_t i;


    for(i=0; i < s_mm_devConf_buffer.index; i++)
    {

        switch(s_mm_devConf_buffer.data[i].addr.val32)
        {
            case FOSC:
            {
                /* Set the bootloader default configuration (the value in HEX file isn't used)
                   When change FOSC configuration register it's necessary a reset
                */
                /*
                    IOL1WAY_OFF ==> disable Single configuration for remappable I/O
                    FCKSM_CSECMD ==> enable clock switching and disable clock monitor
                    POSCMD_EC ==> source scillator : external clock
                    OSCIOFNC_ON ==> OSC2 Pin is configured as Digital I/O
                */
                s_mm_devConf_buffer.data[i].value.val32 = IOL1WAY_OFF & FCKSM_CSECMD & POSCMD_EC & OSCIOFNC_ON; //equal to bootloader configuration bits
            }
            break;
    
            case FOSCSEL:
            {
    		    s_mm_devConf_buffer.data[i].value.val32 = IESO_OFF & FNOSC_FRC;//FNOSC_PRIPLL; //equal to bootloader configuration bits
            }
            break;
    
            case FWDT:
            {
        		/* wdt can be disabled by SW */
                s_mm_devConf_buffer.data[i].value.val32 = FWDTEN_OFF; //(FWDT_MASK & (~FWDT_EN));
            }
            break;
            
            case FPOR:
            {
                s_mm_devConf_buffer.data[i].value.val32 &= FBORPOR_MASK;
            }
            break;
          
            case FGS:
            {
                //GSS<1:0> = 0b11 User program memory is not code protected
                //GWRP = 0b1 user program memory is not write protected
                s_mm_devConf_buffer.data[i].value.val32 = 0x0007;
            }
            break;
    
            case FBS:
            {
                //Is a reserved register (reserved bits must be 1, unimplemented bits must be 0)
                s_mm_devConf_buffer.data[i].value.val32 = 0xFF; //&= 0xcf ;
            }
            break;
    
            case FSS:
            {
                //Is a reserved register (reserved bits must be 1, unimplemented bits must be 0)
         		  s_mm_devConf_buffer.data[i].value.val32 = 0xFF; //&= 0xcf;
            }
            break;
    
            case FICD:
            {
                s_mm_devConf_buffer.data[i].value.val32 &= FICD_MASK;
            }
            break;
    
    	    /* FUID* are not touched */
        }// end switch

    	/* A.M. - According with DSP33 flash program A.N. - Configuration 
         * registers must be written only with internal RC osc 
         */
    
    	__builtin_write_OSCCONH(0);/* switch to internal clk */
    	__builtin_write_OSCCONL(1);
    	while(OSCCONbits.COSC != 0); /* wait for osc to be changed */
        
    	//------------------  IMPORTANT --------------------------------
        //For Configuration memory use WriteLatchCM (write only low word)
        //
        
    	WriteLatchCM( s_mm_devConf_buffer.data[i].addr.word.HW, s_mm_devConf_buffer.data[i].addr.word.LW, 
                      s_mm_devConf_buffer.data[i].value.word.LW);
        WriteMem(CONFIG_REG_WRITE);
    	
    	__builtin_write_OSCCONH(3);/* switch back to external+pll clk */
    	__builtin_write_OSCCONL(1);
    }//end for
}


extern void mm_erase_eeprom(void)
{
    //#warning -> acemor: nice, but to erase the emulated eeprom memory it is better using hal_flash_erase(START_EE, END_EE-START_EE)
    _prog_addressT EE_addr;

    EE_addr = START_EE;

    while( EE_addr+EE_PAGE_ERASE_SIZE <= END_EE )
    {
        _erase_flash(EE_addr);
        EE_addr += EE_PAGE_ERASE_SIZE;
    }
}

extern void mm_erase_programMem(void)
{
    //#warning -> acemor: nice, but to erase the program memory it is better using hal_flash_erase(START_PM, END_PM-START_PM)
    _prog_addressT FLASH_addr;
    
    FLASH_addr = START_PM;	
   
    // for dsp33 erase can be done for a PAGE, not for a single row 
    // Erases the free program memory 
    while(FLASH_addr < END_PM)
    {
        _erase_flash(FLASH_addr);
        FLASH_addr += PM_PAGE_ERASE_SIZE;
    }
}


extern void mm_erase_systemMem(void)
{
    //#warning -> acemor: nice, but to erase the program memory it is better using hal_flash_erase(START_PM, END_PM-START_PM)
    _prog_addressT FLASH_addr;
    
    FLASH_addr = s_mm_sectionMem_info[System_MemT].start_addr;	
   
    // for dsp33 erase can be done for a PAGE, not for a single row 
    // Erases the free program memory 
    while(FLASH_addr < s_mm_sectionMem_info[System_MemT].end_addr)
    {
        _erase_flash(FLASH_addr);
        FLASH_addr += PM_PAGE_ERASE_SIZE;
    }
}
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static inline void s_set_buff_index(uint32_t address)
{
    if( s_mm_actual_wUnit.mem_type == DevConf_MemT)
    {
        s_mm_buffer.index = address;
    }
    else // memtype is program or eeprom
    {
        s_mm_buffer.index = address*2;
    }
}



static eEresult_t s_storeCM(uint8_t *data, uint8_t data_size, mm_hex32file_lineinfo_t *fLine_info_ptr)
{
    int16_t i = 1;

    
    s_mm_devConf_buffer.byte_cntr = 0;
	s_mm_devConf_buffer.data[s_mm_devConf_buffer.index].addr = fLine_info_ptr->address;  //store the address for config. register

    do
    {
        
        if(s_mm_devConf_buffer.byte_cntr < CM_BYTE_PER_UNIT)
        {
            if(s_mm_devConf_buffer.index >= CM_ROWSIZE)
            {
                return(ee_res_NOK_generic);
            }
        
            s_mm_devConf_buffer.data[s_mm_devConf_buffer.index].value.val[s_mm_devConf_buffer.byte_cntr] = data[i];
            s_mm_devConf_buffer.byte_cntr++;
        }
        //For each data read from CAN, decrease data length for HEX packet
        fLine_info_ptr->num_bytes--;
      	i++;	

    }
    while(i < data_size);

    s_mm_devConf_buffer.index++;
    
    return(ee_res_OK);
	
}






static eEresult_t s_storePM(uint8_t *data, uint8_t data_size, mm_hex32file_lineinfo_t *fLine_info_ptr)
{
    int16_t i;
    eEresult_t res;


    i=1;
    do
    {
        s_mm_buffer.data.asByte[s_mm_buffer.index++] = data[i];

        if(s_mm_actual_wUnit.StartAddr == 0)
        {
            s_mm_buffer.data.asWord32[0] = 0x00040000 + (BOOTLDR_ADDR & 0xffff);
            s_mm_buffer.data.asWord32[1] = BOOTLDR_ADDR>>16;
        }
		

        i++;
        s_mm_buffer.byte_cntr++;
        //For each data read from CAN, decrease data length for HEX packet
        fLine_info_ptr->num_bytes--;
          
        //After 4 bytes increments address and reset the byte counter
        if(s_mm_buffer.byte_cntr == PM_BYTE_PER_UNIT)
        {
            fLine_info_ptr->address.val32 += 2;
            s_mm_buffer.byte_cntr = 0;
        }


        if(s_mm_buffer.index >= PM_ROWSIZE*PM_BYTE_PER_UNIT) 
        {
            //Write buffer on memory
            mm_write_buffer(); //I don't check error because here the function can be return only ok.
            
            res = mm_start_write(fLine_info_ptr);
            if(res != ee_res_OK)
            {
                return(res);
            }
        }
    }
    while(i < data_size);
		
    return(ee_res_OK);
}


static eEresult_t s_storeEE(uint8_t *data, uint8_t data_size, mm_hex32file_lineinfo_t *fLine_info_ptr)
{
    int16_t i;
    eEresult_t res;

    if(!s_mm_data.update_ee)
    {
        fLine_info_ptr->num_bytes -= (data_size-1);
        return(ee_res_OK);
    }


    i=1;
    do
    {

        if(s_mm_buffer.byte_cntr < EE_BYTE_PER_UNIT)
        {
            s_mm_buffer.data.asByte[s_mm_buffer.index++] = data[i];
        }
     
        i++;
        s_mm_buffer.byte_cntr++;
        //For each data read from CAN, decrease data length for HEX packet
        fLine_info_ptr->num_bytes--;
      
        //After 4 bytes increments address and reset the byte counter
     	if(s_mm_buffer.byte_cntr == EE_BYTE_PER_UNIT)
        {
            fLine_info_ptr->address.val32 += 2;
            s_mm_buffer.byte_cntr = 0;
        }

        if(s_mm_buffer.index >= EE_ROWSIZE*EE_BYTE_PER_UNIT)
        {
            //Write s_mm_buffer on memory
            mm_write_buffer();

            res = mm_start_write(fLine_info_ptr);
            if(res !=  ee_res_OK)
            {
                return(res);
            }
        }
    }
    while(i < data_size);
		
    return(ee_res_OK);

}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



