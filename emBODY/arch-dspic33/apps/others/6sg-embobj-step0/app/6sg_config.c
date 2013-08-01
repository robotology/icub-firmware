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
#include "eEsharedStorage.h"
#include "eEmemorymap_dspic33.h"

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

//const uint8_t asfidanken_board_address = 4;


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
static uint8_t s_SIXsg_config_ee_data_is_valid(SIXsg_ee_config_data_t *ee_data);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

#warning --> asfidanken says: i dati della 6sg vanno salvati in eeprom e riletti. TOBEDONE
#if 0
hal_arch_dspic_eeprom_emfl_DECLARE_VAR_IN_EE(ee_6sg_cfg, SIXsg_ee_config_data_t, ".edata") = 
{
    .additional_info = {'6', 's', 'g'},
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
#endif


static const SIXsg_ee_config_data_t ee_cfg_data_default =
{
    .additional_info = {'6', 's', 'g'},
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
// -- application info about the application

static const eEmoduleInfo_t s_ap_moduleinfo_application = 
{
    .info           =
    {
        .entity     =
        {
            .type       = ee_entity_process,
            .signature  = ee_procApplication,
            .version    = 
            { 
                .major = 1, 
                .minor = 1
            },  
            .builddate  = 
            {
                .year  = 2013,
                .month = 1,
                .day   = 23,
                .hour  = 18,
                .min   = 0
            }
        },
        .rom        = 
        {   
            .addr   = EE_MEMMAP_APPLICATION_ROM_ADDR,
            .size   = EE_MEMMAP_APPLICATION_ROM_SIZE
        },
        .ram        = 
        {   
            .addr   = EE_MEMMAP_APPLICATION_RAM_ADDR,
            .size   = EE_MEMMAP_APPLICATION_ROM_SIZE
        },
        .storage    = 
        {
            .type   = ee_strg_none,
            .size   = 0,
            .addr   = 0
        },
        .communication  = ee_commtype_can1,
        .name           = "appl-dspic33"
    },
    .protocols  =
    {
        .udpprotversion  = { .major = 0, .minor = 0},
        .can1protversion = { .major = 1, .minor = 0},
        .can2protversion = { .major = 0, .minor = 0},
        .gtwprotversion  = { .major = 0, .minor = 0}
    },
    .extra      = {0}
};


static SIXsg_config_data_t s_the_appcfg =
{
    .gen_ee_data = {0},
    .SIXsg_ee_data = {0},  /**< contains specific information for 6SG application.*/
    .behaviour_cfg = 
    {
        .send_ack_each_cmd = 1, //if = 1 send ack for each command (polling msg)
        .save_in_ee_immediately = 1, //if = 1 save current values of ee_data in eeprom each time a cmd change a value of data in eeprom.      
        .filt_data_mode = filtMode_iir, // filtMode_iir, //TODO: da spostare per adc
        .tx_outMsg_mode = tx_outMsg_off  //tx_outMsg_torqueData_on
    }
};

static uint8_t s_the_appcfg_is_inited = 0;

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern hal_result_t SIXsg_config_init(config_behaviour_t *cfg_behav)
{
    const eEsharinfoPartable_t *ptab;
    hal_result_t res;
    SIXsg_ee_config_data_t ee_cfg_data;
    uint32_t s_canaddr = 0x0A;

    //1) if i have a cfg behaviour i copy it in appl cfg
    if(NULL != cfg_behav)
    {
        memcpy(&s_the_appcfg.behaviour_cfg, cfg_behav, sizeof(config_behaviour_t));
    }
    
    //2) init shared mem
    if(ee_res_OK != ee_sharedstorage_init(ee_procApplication, &s_ap_moduleinfo_application, NULL, 0))
    {
        return(hal_res_NOK_generic);  
    }

    //3) get data from shared mem
    if(ee_res_OK != ee_sharedstorage_deviceinfo_canaddress_get(&s_canaddr))
    {
        return(hal_res_NOK_generic);   
    }
    //3.1)init general data
    s_the_appcfg.gen_ee_data.board_address = s_canaddr;
    s_the_appcfg.gen_ee_data.verinfo.exe_file.major = s_ap_moduleinfo_application.info.entity.version.major;
    s_the_appcfg.gen_ee_data.verinfo.exe_file.minor = s_ap_moduleinfo_application.info.entity.version.minor;
    s_the_appcfg.gen_ee_data.verinfo.can_protocol.major = s_ap_moduleinfo_application.protocols.can1protversion.major;
    s_the_appcfg.gen_ee_data.verinfo.can_protocol.minor = s_ap_moduleinfo_application.protocols.can1protversion.minor;


    //4) get application data.if there no appl data then save default data.
	ee_sharedstorage_partiontable_get(&ptab);

    res = hal_eeprom_read(hal_eeprom_emulatedflash, ptab->strg_priv_application.addr, 
                              sizeof(SIXsg_ee_config_data_t), (void*)&ee_cfg_data);
    if(hal_res_OK != res)
    {
        return(hal_res_NOK_generic);      
    }

    //4.1) check if data is valid
    if(!s_SIXsg_config_ee_data_is_valid(&ee_cfg_data))
    { 
        res = hal_eeprom_write(hal_eeprom_emulatedflash, ptab->strg_priv_application.addr, 
                              sizeof(SIXsg_ee_config_data_t), (void*)&ee_cfg_data_default);
        if(hal_res_OK != res)
        {
            return(res);    
        }
        memcpy(&s_the_appcfg.SIXsg_ee_data, &ee_cfg_data_default, sizeof(SIXsg_ee_config_data_t));
    }
    else
    {
        memcpy(&s_the_appcfg.SIXsg_ee_data, &ee_cfg_data, sizeof(SIXsg_ee_config_data_t));
    }

    s_the_appcfg_is_inited = 1;
    return(hal_res_OK);
}


extern SIXsg_config_data_t * SIXsg_config_get(void)
{
    if(s_the_appcfg_is_inited)
    {
        return(&s_the_appcfg);
    }

    return(NULL);
}

extern hal_result_t SIXsg_config_save_to_eeprom(SIXsg_config_data_t *cfg_ptr)
{
    uint32_t ee_6sg_addr;
    const eEsharinfoPartable_t *ptab;
    hal_result_t res;

    if(NULL == cfg_ptr)
    {
        return(hal_res_NOK_nullpointer);
    }

    ee_sharedstorage_partiontable_get(&ptab);

    //1) save appl data
    res = hal_eeprom_write(hal_eeprom_emulatedflash, ptab->strg_priv_application.addr, 
                             sizeof(SIXsg_ee_config_data_t), (void*)&(cfg_ptr->SIXsg_ee_data));
    if(hal_res_OK != res)
    {
        return(res);
    }

    //2) save addr
    if( ee_res_OK != (ee_sharedstorage_deviceinfo_canaddress_set(cfg_ptr->gen_ee_data.board_address)))
    {
        return(hal_res_NOK_generic);
    }

    return(hal_res_OK);
}



extern hal_result_t SIXsg_boardAddress_save_to_eeprom(SIXsg_config_data_t *cfg_ptr)
{
    if(NULL == cfg_ptr)
    {
        return(hal_res_NOK_nullpointer);
    }

    if( ee_res_OK != (ee_sharedstorage_deviceinfo_canaddress_set(cfg_ptr->gen_ee_data.board_address)))
    {
        return(hal_res_NOK_generic);
    }

    return(hal_res_OK);
}


//extern hal_result_t SIXsg_config_read_from_eeprom(SIXsg_config_data_t *cfg_ptr)
//{
//    //ee_shared_data_t sh_data_aux;
//    uint32_t ee_6sg_addr;
//
//    if(NULL == cfg_ptr)
//    {
//        return(hal_res_NOK_nullpointer);
//    }
//
//    #warning --> asfidanken: read from eeprom shared data. in particular: retrieve address. if not valid set it to 0xD
//    
//    void *asfidanken = &cfg_ptr->asfidanken_shared_data;
////    ee_sh_data_read(&sh_data_aux);
////    if((0 == sh_data_aux.board_address) || (0xFF == sh_data_aux.erased) ) //se la eeprom non è stata cancellata
////    {
////        cfg_ptr->gen_ee_data.board_address =  0xD;
////        cfg_ptr->gen_ee_data.erased = 0xFF;
////    }
////    else
////    {
////        memcpy( &cfg_ptr->gen_ee_data, &sh_data_aux, sizeof(sh_data_aux) );
////    }
//
//    #warning --> asfidanken: read from eeprom the application data
////    hal_arch_dspic_eeprom_emfl_GET_ADDR(ee_6sg_addr, ee_6sg_cfg);
////    hal_eeprom_read(hal_eeprom_emulatedflash, ee_6sg_addr, sizeof(SIXsg_ee_config_data_t), (void*)&cfg_ptr->SIXsg_ee_data);
//
//    return(hal_res_OK);
//}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static uint8_t s_SIXsg_config_ee_data_is_valid(SIXsg_ee_config_data_t *ee_data)
{
    //al posto di questo stupido check usa: ee_common_check_isvalid
    if(0xBB != ee_data->checksum)
    {
        return(0);
    }
    return(1);
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



