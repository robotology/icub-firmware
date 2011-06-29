/* 
 * Copyright (C) 2007 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Claudio Lorini, Marco Maggiali, Marco Randazzo, Valentina Gaggero
 * email:   claudio.lorini@iit.it, marco.maggiali@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
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
/*	@file 	hal_errors.c
	@brief 	This file implements functionality for error management.
	@author claudio.lorini@iit.it, marco.maggiali@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
	@date	-
*/



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include <reset.h>


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "hal_can_interface.h"
#include "hal_errors.h"

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
// empty-section


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
void hal_error_manage(uint8_t x)
// Insert error in queue
{
// timestamp
// enqueue
// flag error
}


extern void hal_error_canMsg_set( uint8_t *Txdata, int8_t *datalen, uint8_t can_cmd)
{
	*datalen = 4; 
	Txdata[0] = can_cmd;
	Txdata[1] = 'B';
	Txdata[2] = 'U';
	Txdata[3] = 'G';
}

extern hal_error_reset_t hal_error_resetCause_get(void)
{
	if(isBOR!=0)
	{
    	return(INFO_TRSET_isBOR);
	}
	if(isPOR!=0)
	{
		return(INFO_TRSET_isPOR);
	}
	if(isLVD!=0)
	{
		return(INFO_TRSET_isLVD);
	}
	if(isMCLR!=0)
	{
		return(INFO_TRSET_isMCLR);
	}
	if(isWDTTO!=0)
	{
		return(INFO_TRSET_isWDTTO);
	}
	if(isWDTWU!=0)
	{
		return(INFO_TRSET_isWDTWU);
	}
	if(isWU!=0)
	{
		return(INFO_TRSET_isWU);
	}

}

extern void hal_error_resetRegister_reset(void)
{

	RCON=0;
	RCONbits.POR=0;
	RCONbits.BOR=0;
	RCONbits.IDLE=0;
	RCONbits.SLEEP=0;
	RCONbits.WDTO=0;
	RCONbits.SWDTEN=0;
	RCONbits.SWR=0;
	RCONbits.EXTR=0;
	RCONbits.LVDL =0;
	RCONbits.LVDEN=0;
	RCONbits.BGST=0;
	RCONbits.IOPUWR=0;
	RCONbits.TRAPR=0;

}



extern void hal_error_resetCause_send(uint8_t BoardAddress)
{
	unsigned int SID = (CAN_MSG_CLASS_POLLING) | ((BoardAddress)<<4) | (0) ;
	unsigned char message[6] = {0,0,0,0,0,0};

	if(RCONbits.POR!=0)
	{
		message[0]=1;
	}
	if(RCONbits.BOR!=0)
	{   
		message[1]=1;
	}
	if(RCONbits.IDLE!=0)
	{   
		message[2]=1;
 	}
	if(RCONbits.LVDL!=0)	
	{
		message[3]=1;
	}
	if(RCONbits.LVDEN!=0)
	{
		message[4]=1;
	}
	if(RCONbits.WDTO!=0)
	{
		message[5]=1;
	}
	if(RCONbits.EXTR!=0)
	{
		message[6]=1;
	}
	
	while( !(hal_can_txHwBuff_isEmpty(hal_can_portCAN1, 0)) );
	hal_can_put_immediately(hal_can_portCAN1, SID, message, 6, 1 );
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



