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


/* @file       pid.c
	@brief      This file implements the PID.
	@author     marco.maggiali@iit.it, 
    @date       26/03/2013
**/

#include "pid.h"

/************** TORQUE PID-CONTROLLERS INIT VALUES **************/

// With MB459 phase current (A)= (PID_X_REFERENCE * 0.64)/(32767 * Rshunt)

/* default values for Torque control loop */
#define PID_TORQUE_REFERENCE   (int16_t)4500   //(N.b: that's the reference init  
                                       //value in both torque and speed control)
#define PID_TORQUE_KP_DEFAULT  (int16_t)8000       
#define PID_TORQUE_KI_DEFAULT  (int16_t)1000
#define PID_TORQUE_KD_DEFAULT  (int16_t)3000

/* default values for Flux control loop */
#define PID_FLUX_REFERENCE   (int16_t)0
#define PID_FLUX_KP_DEFAULT  (int16_t)7500 
#define PID_FLUX_KI_DEFAULT  (int16_t)1000
#define PID_FLUX_KD_DEFAULT  (int16_t)3000

// Torque PID  parameter dividers
#define TF_KPDIV ((uint16_t)(8192))
#define TF_KIDIV ((uint16_t)(4096))
#define TF_KDDIV ((uint16_t)(8192))

/*********************** SPEED LOOP SAMPLING TIME *****************************/
//Not to be modified

#define PID_SPEED_REFERENCE	  0
						     
#define PID_SPEED_SAMPLING_500us      0     // min 500usec
#define PID_SPEED_SAMPLING_1ms        1
#define PID_SPEED_SAMPLING_2ms        3     // (3+1)*500usec = 2msec
#define PID_SPEED_SAMPLING_5ms        9		// (9+1)*500usec = 5msec		
#define PID_SPEED_SAMPLING_10ms       19	// (19+1)*500usec = 10msec
#define PID_SPEED_SAMPLING_20ms       39	// (39+1)*500usec = 20msec
#define PID_SPEED_SAMPLING_127ms      255   // max (255-1)*500us = 127 ms

//User should make his choice here below
#define PID_SPEED_SAMPLING_TIME   (uint8_t)(PID_SPEED_SAMPLING_2ms)

/******************** SPEED PID-CONTROLLER INIT VALUES************************/

/* default values for Speed control loop */
#define PID_SPEED_REFERENCE_RPM   (int16_t)1500
#define PID_SPEED_KP_DEFAULT      (int16_t)4000
#define PID_SPEED_KI_DEFAULT      (int16_t)700
#define PID_SPEED_KD_DEFAULT      (int16_t)800

/* Speed PID parameter dividers          */
#define SP_KPDIV ((uint16_t)(16))
#define SP_KIDIV ((uint16_t)(256))
#define SP_KDDIV ((uint16_t)(16))

#define NOMINAL_TORQUE 2000

static int16_t hTorque_Reference=0;
static int16_t hSpeed_Reference=0;

/*******************************************************************************
* Function Name  : PID_Init
* Description    : Initialize PID coefficients for torque, flux and speed loop: 
                   Kp_Gain: proportional coeffcient 
                   Ki_Gain: integral coeffcient 
                   Kd_Gain: differential coeffcient 
* Input          : Pointer 1 to Torque PI structure,                     
                   Pointer 2 to Speed PI structure  
* Output         : None
* Return         : None
*******************************************************************************/
void PID_Init (PID_Struct_t *PID_Torque, PID_Struct_t *PID_Speed)
{
  hTorque_Reference = PID_TORQUE_REFERENCE;

  PID_Torque->hKp_Gain    = PID_TORQUE_KP_DEFAULT;
  PID_Torque->hKp_Divisor = TF_KPDIV;  

  PID_Torque->hKi_Gain = PID_TORQUE_KI_DEFAULT;
  PID_Torque->hKi_Divisor = TF_KIDIV;
  
  PID_Torque->hKd_Gain = PID_TORQUE_KD_DEFAULT;
  PID_Torque->hKd_Divisor = TF_KDDIV;
  PID_Torque->wPreviousError = 0;
  
  PID_Torque->hLower_Limit_Output=-3360;   //Lower Limit for Output limitation
  PID_Torque->hUpper_Limit_Output= 3360;   //Upper Limit for Output limitation
  PID_Torque->wLower_Limit_Integral = -3000 * TF_KIDIV;
  PID_Torque->wUpper_Limit_Integral = 3000 * TF_KIDIV;
  PID_Torque->wIntegral = 0;
 
  /**************************************************/
  /************END PID Torque Regulator members*******/
  /**************************************************/


  /**************************************************/
  /************PID Speed Regulator members*************/
  /**************************************************/


  PID_Speed->wIntegral = 0;  // reset integral value 

  hSpeed_Reference = PID_SPEED_REFERENCE;

  PID_Speed->hKp_Gain    = PID_SPEED_KP_DEFAULT;
  PID_Speed->hKp_Divisor = SP_KPDIV;  

  PID_Speed->hKi_Gain = PID_SPEED_KI_DEFAULT;
  PID_Speed->hKi_Divisor = SP_KIDIV;
  
  PID_Speed->hKd_Gain = PID_SPEED_KD_DEFAULT;
  PID_Speed->hKd_Divisor = SP_KDDIV;
  PID_Speed->wPreviousError = 0;
  
  PID_Speed->hLower_Limit_Output= -NOMINAL_TORQUE;   //Lower Limit for Output limitation
  PID_Speed->hUpper_Limit_Output= NOMINAL_TORQUE;   //Upper Limit for Output limitation
  PID_Speed->wLower_Limit_Integral = -NOMINAL_TORQUE * SP_KIDIV;
  PID_Speed->wUpper_Limit_Integral = NOMINAL_TORQUE * SP_KIDIV;
  PID_Speed->wIntegral = 0;
  /**************************************************/
  /**********END PID Speed Regulator members*********/
  /**************************************************/
}

/*******************************************************************************
* Function Name  : PID_Regulator
* Description    : Compute the PI(D) output for a PI(D) regulation.
* Input          : Pointer to the PID settings (*PID_Flux)
                   Speed in int16_t format
* Output         : int16_t
* Return         : None
*******************************************************************************/
int16_t PID_Regulator(int16_t hReference, int16_t hPresentFeedback, PID_Struct_t *PID_Struct)
{
  int32_t wError, wProportional_Term,wIntegral_Term, houtput_32;
  int64_t dwAux;

#ifdef DIFFERENTIAL_TERM_ENABLED    
  int32_t wDifferential_Term;
#endif    
  // error computation
  wError= (int32_t)(hReference - hPresentFeedback);
 
  // Proportional term computation
  wProportional_Term = PID_Struct->hKp_Gain * wError;

  // Integral term computation
  if (PID_Struct->hKi_Gain == 0)
  {
    PID_Struct->wIntegral = 0;
  }
  else
  { 
    wIntegral_Term = PID_Struct->hKi_Gain * wError;
    dwAux = PID_Struct->wIntegral + (int64_t)(wIntegral_Term);
    
    if (dwAux > PID_Struct->wUpper_Limit_Integral)
    {
      PID_Struct->wIntegral = PID_Struct->wUpper_Limit_Integral;
    }
    else if (dwAux < PID_Struct->wLower_Limit_Integral)
          { 
            PID_Struct->wIntegral = PID_Struct->wLower_Limit_Integral;
          }
          else
          {
           PID_Struct->wIntegral = (int32_t)(dwAux);
          }
  }
  // Differential term computation
#ifdef DIFFERENTIAL_TERM_ENABLED
  {
  int32_t wtemp;
  
  wtemp = wError - PID_Struct->wPreviousError;
  wDifferential_Term = PID_Struct->hKd_Gain * wtemp;
  PID_Struct->wPreviousError = wError;    // store value 
  }
  houtput_32 = (wProportional_Term/PID_Struct->hKp_Divisor+ 
                PID_Struct->wIntegral/PID_Struct->hKi_Divisor + 
                wDifferential_Term/PID_Struct->hKd_Divisor); 

#else  
  houtput_32 = (wProportional_Term/PID_Struct->hKp_Divisor+ 
                PID_Struct->wIntegral/PID_Struct->hKi_Divisor);
#endif
  
    if (houtput_32 >= PID_Struct->hUpper_Limit_Output)
      {
      return(PID_Struct->hUpper_Limit_Output);		  			 	
      }
    else if (houtput_32 < PID_Struct->hLower_Limit_Output)
      {
      return(PID_Struct->hLower_Limit_Output);
      }
    else 
      {
        return((int16_t)(houtput_32)); 		
      }
}		   
