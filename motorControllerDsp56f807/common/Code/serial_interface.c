#include "dsp56f807.h"
#include "options.h"
#include "pid.h"
#include "asc.h"
#include "can1.h"
#include "trajectory.h"
#include "serial_interface.h"
#include "pwm_interface.h"
#include "flash_interface.h"
#include "can_interface.h"


#ifndef VERSION
#	error "No valid version specified"
#endif



/***********************************************************
 global variables
************************************************************/

extern  char    _additional_info[32];
extern byte	_board_ID;
byte   serial_channels = 0;

/***************************************************************************/
/**
 * This method inits the serial interface
 * @param channels_num is the max number of channels on the board (2,4) 
 ***************************************************************************/
void serial_interface_init (byte channels_num)
{
	AS1_Init ();
	serial_channels = channels_num;
}
	
/***************************************************************************/
/**
 * prints the version number of the firmware to the serial port.
/***************************************************************************/
void print_version(void)
{

}

/***************************************************************************/
/**
 * This method reads and executes the serial messages.
/***************************************************************************/
void serial_interface (void)
{

}
