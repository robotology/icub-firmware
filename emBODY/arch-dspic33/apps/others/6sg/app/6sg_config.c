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
// empty section


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <string.h>
#include "hal_eeprom.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "6sg_config.h"



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

hal_arch_dspic_eeprom_emfl_DECLARE_VAR_IN_EE(ee_6sg_cfg, SIXsg_ee_config_data_t, ".edata") = 
{
    .additional_info = {'C', 'i', 'a', 'o'},
    .serial_number = {1,2,3,4,5,6,7},  
    .an_channel_offset = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }, //{0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5 },  //ofset to apply to each channel.
    .tr_matrix =
    {
        {0x7FFF,0,0,0,0,0},  // TT_TF_TMatrix
        {0,0x7FFF,0,0,0,0},
        {0,0,0x7FFF,0,0,0},
        {0,0,0,0x7FFF,0,0},
        {0,0,0,0,0x7FFF,0},
        {0,0,0,0,0,0x7FFF},
    },
    .an_channel_gain = { {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0} },
    .calibration_tare = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0 },
    .full_scales = {1,2,3,4,5,6},
    .sw_matrix_gain = 0XC,
    .can_msg_datarate = 1,
    .iir_tr_coef =
    {
        0x00A6,
        0xFF85,
        0x73D2,
        0x00A6,
        0xCB5C,
        0x0800,
        0xF200,
        0x76A6,
        0x0800,
        0xC759,
        0x143E,
        0xDA86,
        0x7A8C,
        0x143E,
        0xC26A,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000,
        0x0000 
    },
    .checksum = 0xBB,
};



//this variable contains default values for data saved in eeprom.
//these values are used if eeprom has been erased.
//static SIXsg_config_data_t SIXsg_cfg =
//{
//    .SIXsg_ee_data=
//    {
//        .an_channel_offset = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }, //{0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5 },  //ofset to apply to each channel.
//        .tr_matrix =
//        {
//            {0x7FFF,0,0,0,0,0},  // TT_TF_TMatrix
//            {0,0x7FFF,0,0,0,0},
//            {0,0,0x7FFF,0,0,0},
//            {0,0,0,0x7FFF,0,0},
//            {0,0,0,0,0x7FFF,0},
//            {0,0,0,0,0,0x7FFF},
//        },
//        .an_channel_gain = { {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0} },
//        .calibration_tare = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0 },
//        .full_scales = {1,2,3,4,5,6},
//        .sw_matrix_gain = 0XC,
//        .can_msg_datarate = 1,
//        .iir_tr_coef =
//        {
//            0x00A6,
//            0xFF85,
//            0x73D2,
//            0x00A6,
//            0xCB5C,
//            0x0800,
//            0xF200,
//            0x76A6,
//            0x0800,
//            0xC759,
//            0x143E,
//            0xDA86,
//            0x7A8C,
//            0x143E,
//            0xC26A,
//            0x0000,
//            0x0000,
//            0x0000,
//            0x0000,
//            0x0000,
//            0x0000,
//            0x0000,
//            0x0000,
//            0x0000,
//            0x0000 
//      },
//        .checksum = 0xBB,
//    },
//    .gen_ee_data=
//    {
//        .hw_bits = 123,
//        .board_address = 0xD,
//        .additional_info = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31},
//        .serial_number = {0xA, 0xB, 0x1, 0x2, 0xA, 0xB, 0x1},
//    },
//    //. behaviour_cfg = {0} here don't care of this fileds they are configued in compile time
//
//};
//


//this variable contains varsion information about 6SG application and its can protocol.
static srcCode_version_info_t srcCode_info = 
{
    .exe_file = 
    {
        .version = 1,
        .release = 0,
        .build = 22,
    },
    .can_protocol =
    {
        
        .version = 0,
        .release = 1,
        .build = 0 //unused!!!
    } 
}; 


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern hal_result_t SIXsg_config_save_to_eeprom(SIXsg_config_data_t *cfg_ptr)
{
    uint32_t ee_6sg_addr;

    if(NULL == cfg_ptr)
    {
        return(hal_res_NOK_nullpointer);
    }

    ee_sh_data_write(&cfg_ptr->gen_ee_data);

    hal_arch_dspic_eeprom_emfl_GET_ADDR(ee_6sg_addr, ee_6sg_cfg);
    hal_eeprom_write(hal_eeprom_emulatedflash, ee_6sg_addr, sizeof(SIXsg_config_data_t), (void*)&cfg_ptr->SIXsg_ee_data);

    return(hal_res_OK);
}


extern srcCode_version_info_t* srcCode_version_get_handler(void)
{
    return(&srcCode_info);
}


extern hal_result_t SIXsg_boardAddress_save_to_eeprom(SIXsg_config_data_t *cfg_ptr)
{
     if(NULL == cfg_ptr)
    {
        return(hal_res_NOK_nullpointer);
    }

    return( ee_sh_data_write(&cfg_ptr->gen_ee_data) );
}


extern hal_result_t SIXsg_config_read_from_eeprom(SIXsg_config_data_t *cfg_ptr)
{
    ee_shared_data_t sh_data_aux;
    uint32_t ee_6sg_addr;

    if(NULL == cfg_ptr)
    {
        return(hal_res_NOK_nullpointer);
    }

    ee_sh_data_read(&sh_data_aux);
    if((0 == sh_data_aux.board_address) || (0xFF == sh_data_aux.erased) ) //se la eeprom non è stata cancellata
    {
        cfg_ptr->gen_ee_data.board_address =  0xD;
        cfg_ptr->gen_ee_data.erased = 0xFF;
    }
    else
    {
        memcpy( &cfg_ptr->gen_ee_data, &sh_data_aux, sizeof(sh_data_aux) );
    }

    hal_arch_dspic_eeprom_emfl_GET_ADDR(ee_6sg_addr, ee_6sg_cfg);
    hal_eeprom_read(hal_eeprom_emulatedflash, ee_6sg_addr, sizeof(SIXsg_ee_config_data_t), (void*)&cfg_ptr->SIXsg_ee_data);

    return(hal_res_OK);
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



