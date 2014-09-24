/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Marco Maggiali
 * email:   marco.maggiali@iit.it
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


/* @file       as5048.h
	@brief      This file implements the reading of the as5048 sensor (Absolute Magnetic sensor)
	@author     marco.maggiali@iit.it, 
    @date       26/03/2013
**/

/*******************************************************************************
* Function Name  : as5048_init
* Description    : init of the as5048 sensor	 
*
* Input          : sensorID 0 and 1 
* Output         : None
* Return         : None
*******************************************************************************/
void as5048_init(uint8_t sensorID);

/*******************************************************************************
* Function Name  : as5048_read
* Description    : read the as5048 sensor	 
*
* Input          : sensorID 0 and 1 
* Output         : None
* Return         : the 14 bit value of the encoder 
*******************************************************************************/
uint16_t * as5048_read(uint8_t sensorID); 
