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

/* @file       calculator.c
    @brief      This file implements interface to calculator entity. 
    @author     valentina.gaggero@iit.it
    @date       07/14/2011
*/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <string.h>
#include <dsp.h>
#include "6sg_config.h"
#include "adc.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "calculator.h"


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

typedef struct
{
    SIXsg_config_data_t     *SIXsg_config_ptr;
    int16_t                 current_tare[AN_CHANNEL_NUM];
}calculator_data_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static calculator_data_t calc_data = 
{
    .SIXsg_config_ptr = NULL,
    .current_tare = {1,1,1,1,1,1}   
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern hal_result_t calculator_init(SIXsg_config_data_t *cfg_ptr)
{
    if(NULL == cfg_ptr)
    {
        return(hal_res_NOK_nullpointer);
    }

    calc_data.SIXsg_config_ptr= cfg_ptr;
    memset( calc_data.current_tare, 0, sizeof(int16_t)*AN_CHANNEL_NUM );

    return(hal_res_OK);
}


extern hal_result_t calc_current_tare_get_value(uint8_t ch_num, int16_t *value)
{
    if( (ch_num >= AN_CHANNEL_NUM) || (NULL == value) )
    {
        return(hal_res_NOK_wrongparam);
    }

    *value = calc_data.current_tare[ch_num];

    return(hal_res_OK);

}

extern hal_result_t calc_current_tare_set_value(uint8_t ch_num, int16_t value)
{
    if(ch_num >= AN_CHANNEL_NUM)
    {
        return(hal_res_NOK_wrongparam);
    }

    calc_data.current_tare[ch_num] = value;

    return(hal_res_OK);

}


extern hal_result_t calculate_torque_and_force_data(int16_t *input, calc_data_output_t *output)
{
    //uint8_t i;
    int16_t raw_data[AN_CHANNEL_NUM];



    if( (NULL == output) || (NULL == input) )
    {
        return(hal_res_NOK_nullpointer);
    }

    VectorAdd (AN_CHANNEL_NUM, (fractional*)raw_data, (fractional*)input, calc_data.SIXsg_config_ptr->SIXsg_ee_data.calibration_tare);
    
    MatrixMultiply( AN_CHANNEL_NUM, // int numRows1,
                    AN_CHANNEL_NUM, // int numCols1Rows2,
                    1, // int numCols2,
                    (fractional*)&output[0],   // fractional* dstM,
                    (fractional*)&(calc_data.SIXsg_config_ptr->SIXsg_ee_data.tr_matrix[0][0]),    // fractional* srcM1,
                    (fractional*)&raw_data[0] ); // fractional* srcM2 



    VectorAdd (AN_CHANNEL_NUM, (fractional*)output->array, (fractional*)output->array, calc_data.current_tare);
    

//    for(i=0; i<AN_CHANNEL_NUM; i++)
//    {
//        output->array[i] += HEX_VALC;
//    }
    
    return(hal_res_OK);

}


extern void calc_current_tare_init(void)
{
    memset(calc_data.current_tare, 0, sizeof(int16_t)*AN_CHANNEL_NUM);
}


extern void calc_current_tare_reset(void)
{
    uint8_t i;
    int16_t input[AN_CHANNEL_NUM];
    int16_t output[AN_CHANNEL_NUM];

    adc_get_data(input);

    VectorAdd (AN_CHANNEL_NUM, (fractional*)input, (fractional*)&input[0], calc_data.SIXsg_config_ptr->SIXsg_ee_data.calibration_tare);
    MatrixMultiply(
                    AN_CHANNEL_NUM, // int numRows1,
                    AN_CHANNEL_NUM, // int numCols1Rows2,
                    1, // int numCols2,
                    &output[0],   // fractional* dstM,
                    &calc_data.SIXsg_config_ptr->SIXsg_ee_data.tr_matrix[0][0],    // fractional* srcM1,
                    (int16_t*)&input[0]); // fractional* srcM2 

    for (i=0; i<AN_CHANNEL_NUM; i++)
    {
        calc_data.current_tare[i] = -(output[i]);
    }
    //nota: è corretto non aggiungere 0x8000 perchè è usato per la tara
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



