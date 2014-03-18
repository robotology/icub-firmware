/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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
// - doxy
// --------------------------------------------------------------------------------------------------------------------

/* @file       hl_crc07.c
    @brief      This file contains implementation of hl_crc07 using LUTs, as described by Sven Reifegerste
                at http://zorc.breitbandkatze.de/crc.html
    @author     marco.accame@iit.it
    @date       12/02/2011
**/

// - modules to be built: contains the HL_USE_* macros ---------------------------------------------------------------

#include "hl_cfg_plus_modules.h"

#if     defined(HL_USE_UTIL_CRC07)

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "stdlib.h"     // to see NULL
#include "string.h"     // to see memset()


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "hl_crc07.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
#include "hl_crc07_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

// uncomment to save 256 bytes of rom
#define _USE_HL_CRC07TABLE_IN_ROM_
  

//#define LITTLE_ENDIAN
//#ifdef LITTLE_ENDIAN
//#define htonl(l)    __rev(l)
//#else
//#define htonl(l)    (l)
//#endif


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------

#ifdef _USE_HL_CRC07TABLE_IN_ROM_
// tbl taken from edl manual and NOT by running s_hl_crc07_generate_table(0x1021, tbl) on a cm3 micro with a little endian architecture.
const uint8_t hl_crc07_table_0x09[256] =
{
    0x00, 0x09, 0x12, 0x1b, 0x24, 0x2d, 0x36, 0x3f,
    0x48, 0x41, 0x5a, 0x53, 0x6c, 0x65, 0x7e, 0x77,
    0x19, 0x10, 0x0b, 0x02, 0x3d, 0x34, 0x2f, 0x26,
    0x51, 0x58, 0x43, 0x4a, 0x75, 0x7c, 0x67, 0x6e,
    0x32, 0x3b, 0x20, 0x29, 0x16, 0x1f, 0x04, 0x0d,
    0x7a, 0x73, 0x68, 0x61, 0x5e, 0x57, 0x4c, 0x45,
    0x2b, 0x22, 0x39, 0x30, 0x0f, 0x06, 0x1d, 0x14,
    0x63, 0x6a, 0x71, 0x78, 0x47, 0x4e, 0x55, 0x5c,
    0x64, 0x6d, 0x76, 0x7f, 0x40, 0x49, 0x52, 0x5b,
    0x2c, 0x25, 0x3e, 0x37, 0x08, 0x01, 0x1a, 0x13,
    0x7d, 0x74, 0x6f, 0x66, 0x59, 0x50, 0x4b, 0x42,
    0x35, 0x3c, 0x27, 0x2e, 0x11, 0x18, 0x03, 0x0a,
    0x56, 0x5f, 0x44, 0x4d, 0x72, 0x7b, 0x60, 0x69,
    0x1e, 0x17, 0x0c, 0x05, 0x3a, 0x33, 0x28, 0x21,
    0x4f, 0x46, 0x5d, 0x54, 0x6b, 0x62, 0x79, 0x70,
    0x07, 0x0e, 0x15, 0x1c, 0x23, 0x2a, 0x31, 0x38,
    0x41, 0x48, 0x53, 0x5a, 0x65, 0x6c, 0x77, 0x7e,
    0x09, 0x00, 0x1b, 0x12, 0x2d, 0x24, 0x3f, 0x36,
    0x58, 0x51, 0x4a, 0x43, 0x7c, 0x75, 0x6e, 0x67,
    0x10, 0x19, 0x02, 0x0b, 0x34, 0x3d, 0x26, 0x2f,
    0x73, 0x7a, 0x61, 0x68, 0x57, 0x5e, 0x45, 0x4c,
    0x3b, 0x32, 0x29, 0x20, 0x1f, 0x16, 0x0d, 0x04,
    0x6a, 0x63, 0x78, 0x71, 0x4e, 0x47, 0x5c, 0x55,
    0x22, 0x2b, 0x30, 0x39, 0x06, 0x0f, 0x14, 0x1d,
    0x25, 0x2c, 0x37, 0x3e, 0x01, 0x08, 0x13, 0x1a,
    0x6d, 0x64, 0x7f, 0x76, 0x49, 0x40, 0x5b, 0x52,
    0x3c, 0x35, 0x2e, 0x27, 0x18, 0x11, 0x0a, 0x03,
    0x74, 0x7d, 0x66, 0x6f, 0x50, 0x59, 0x42, 0x4b,
    0x17, 0x1e, 0x05, 0x0c, 0x33, 0x3a, 0x21, 0x28,
    0x5f, 0x56, 0x4d, 0x44, 0x7b, 0x72, 0x69, 0x60,
    0x0e, 0x07, 0x1c, 0x15, 0x2a, 0x23, 0x38, 0x31,
    0x46, 0x4f, 0x54, 0x5d, 0x62, 0x6b, 0x70, 0x79
};
#endif



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_hl_crc07_generate_table(const uint8_t polynomial, uint8_t *table);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern void hl_crc07_table_get(uint8_t polynomial, uint8_t *crctable)
{

#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK) 
    if(NULL == crctable)
    {
        return;
    }
#endif//!defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK) 
    
    s_hl_crc07_generate_table(polynomial, crctable);
}



#undef USE_CRCTESTER_ALGORITHM



#if defined(USE_CRCTESTER_ALGORITHM)

extern uint8_t hl_crc07_compute(const uint8_t *hl_crc07table, const uint8_t polynom, uint8_t initvalue, uint8_t *data, uint32_t len)
{  
	// fast bit by bit algorithm without augmented zero bytes.
	// does not use lookup table, suited for polynom orders between 1...32.

	uint32_t i, j, c, bit;
	uint32_t crc = initvalue;

	for(i=0; i<len; i++)
    {
		c = (uint32_t)*data++;

		for(j=0x80; j; j>>=1) 
        {
			bit = crc & 0x40;
			crc<<= 1;
			if(c & j) 
            {
                bit ^= 0x40;
            }
			if(bit) 
            {
                crc ^= polynom;
            }
		}
	}	


	crc &= 0x7F;

	return(crc);
    
}
    
#else
    
// use algorithm found by edl ... see also: http://lxr.free-electrons.com/ident?i=crc7_syndrome_table or http://www.cs.fsu.edu/~baker/devices/lxr/http/source/linux/lib/crc7.c

static uint8_t crc7_byte(const uint8_t *table, uint8_t crc, uint8_t data)
{
    return(table[(crc << 1) ^ data]);
}

static uint8_t crc7(const uint8_t *table, uint8_t crc, const uint8_t *buffer, uint32_t len)
{
    while(len--)
    {
        crc = crc7_byte(table, crc, *buffer++);
    }
    
    return(crc);
}    
 
extern uint8_t hl_crc07_compute(const uint8_t *hl_crc07table, uint8_t initvalue, const uint8_t *data, uint32_t len)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK) 
    if((NULL == hl_crc07table) || (NULL == data) || (0 == len))
    {
        return(0);
    }
#endif//!defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)  
    
	return(crc7(hl_crc07table, initvalue, data, len));    
}

#endif    


 

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

//#warning WIP --> verify the function s_hl_crc07_generate_table(). run with 0x09 and verify vs table in manual of EMS4
// it does not generates the correct table ... thus we limit to the precomputed table
static void s_hl_crc07_generate_table(const uint8_t polynomial, uint8_t *table) 
{
#if 0
    
	uint32_t i, j;
	uint32_t bit, crc;

	for(i=0; i<256; i++) 
    {
		crc = i;
		crc >>= 1;
 

		for(j=0; j<8; j++) 
        {
			bit = crc & 0x40;
			crc <<= 1;
			if(bit)
            {
                crc ^= polynomial;
            }
		}			

		table[i]= crc & 0x7F;
	}
    
#else

    // from: http://www.pololu.com/docs/0J5/5.f
    // see also http://ghsi.de/CRC/index.php?Polynom=10001001&Message=E100CAFe
    int i, j;
    
    
    uint8_t pol = 0x80 + polynomial; // not 0x09 but 0x89
    
 
    // generate a table value for all 256 possible byte values
    for (i = 0; i < 256; i++)
    {
        table[i] = (i & 0x80) ? i ^ pol : i;
        for (j = 1; j < 8; j++)
        {
            table[i] <<= 1;
            if (table[i] & 0x80)
                table[i] ^= pol;
        }
    }    
    
#endif    
}

#endif//HL_USE_UTIL_CRC07

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------


