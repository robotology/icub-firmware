/* 
 * Copyright (C) 2007 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Claudio Lorini, Marco Randazzo, Valentina Gaggero
 * email:   claudio.lorini@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
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
/*	@file 	strain_IIR_filter.h
	@brief 	This file implements functions for IIR filter.
	@author claudio.lorini@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
	@date	
*/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include <p30f4013.h>
#include <libpic30.h>



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "strain_IIR_filter.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
//#include "entity_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------

// eeprom filter data init
/*static*/ struct s_eeIIRTransposedCoefs _EEDATA(1) eeIIRTransposedCoefs =
{
  2,      // n. BiQuads
  {
  // filter coefs LPF 50Hz 
  0x00E3,
  0x00DD,
  0x68D6,
  0x00E3,
  0xD487,
  0x0343,
  0xFDD5,
  0x728F,
  0x0343,
  0xC914
  }
};


IIRTransposedStruct iirt[6];
s_eeIIRTransposedCoefs IirTransposedCoefs __attribute__((__space__(xmemory))) = { 0 };
fractional IirTransposedState1[6][IIR_LPF_N_MAX_BQ * 2] __attribute__((__space__(ymemory)));
fractional IirTransposedState2[6][IIR_LPF_N_MAX_BQ * 2] __attribute__((__space__(ymemory)));




// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

void strain_IIRFilter_init()
{
  int16_t i;
  
  for( i=0; i<6 ; i++) 
  { 
    iirt[i].numSectionsLess1 = IirTransposedCoefs.IIR_N_BQ - 1;
    iirt[i].coeffsBase = &IirTransposedCoefs.IirTransposedCoefs[0];
    iirt[i].coeffsPage = COEFFS_IN_DATA;
    iirt[i].delayBase1 = &IirTransposedState1[i][0];
    iirt[i].delayBase2 = &IirTransposedState2[i][0];
	iirt[i].finalShift = 0; 
  
    // Zero the filter state
  //  IIRTransposedInit(&iirt[i]);
  }
}

void strain_IIRFilter_saveInEE()
{
	// 
	// Save IIR Filter Coefs to EE
	//
	
	_prog_addressT EE_addr;
	int16_t i=0, *DMAdx;

	// initialize Data EEPROM address 
	_init_prog_address(EE_addr, eeIIRTransposedCoefs);
	
	// Erase Data EEPROM at ee_data  
	for(i=0 ; i < sizeof(IirTransposedCoefs) ; i++)
	{
	  _erase_eedata(EE_addr++, _EE_WORD);
	  _wait_eedata();
	}
	
	// Write eeIIRTransposedCoefs to Data EEPROM
	_init_prog_address(EE_addr, eeIIRTransposedCoefs);
	// save NBiQuads+coeffs
	DMAdx = (int*) &(IirTransposedCoefs);
	for(i=0 ; i < sizeof(IirTransposedCoefs) ; i++)
	{
	  _write_eedata_word(EE_addr, *DMAdx );
	  EE_addr+=2;
	  DMAdx++;
	  _wait_eedata();
	}
}

void strain_IIRFilter_readFromEE()
{
  	_prog_addressT EE_addr;
    _init_prog_address(EE_addr, eeIIRTransposedCoefs);
    _memcpy_p2d16(&IirTransposedCoefs, EE_addr, sizeof(IirTransposedCoefs));
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

