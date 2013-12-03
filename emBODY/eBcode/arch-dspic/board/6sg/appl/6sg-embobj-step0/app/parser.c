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

/*  @file       parser.c
    @brief      This file implements parser for messages belonging to can protocol.
                If message has invalid parameters or has unknown commands, the parser
                does not anything and return ok. (i.e does not care these problems.)
    @author     valentina.gaggero@iit.it
    @date       07/18/2011
*/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <string.h>
#include "hal_can.h"
#include "hal_sys.h"
#include "calculator.h"
#include "sender_outMsg.h"
#include "amplifier.h"
#include "adc.h"
#include "EOStheSystem.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "parser.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
//#include "entity_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define MAX_CMD_NUM                     0x1F

#define GET_CMD_CLASS(MSG_ID)           ((MSG_ID)&0b11100000000)
#define GET_CMD_DEST(MSG_ID)            ((MSG_ID)&0b00000001111)
#define GET_CMD_SRC(MSG_ID)             (((MSG_ID)&0b00011110000) >> 4)



//shortcut to access configuration data
#define CFG_6SG_BEHAV                   parser_data.SIXsg_config_ptr->behaviour_cfg
#define CFG_GEN_EEDATA                  parser_data.SIXsg_config_ptr->gen_ee_data
#define CFG_6SG_EEDATA                  parser_data.SIXsg_config_ptr->SIXsg_ee_data

#define EVT_MSG_RECEIVED            (0x1<<3)    // evt pos is 4 
// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
    uint8_t sendAmsg = 1;
    uint8_t sendBmsg = 1;


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    uint8_t ee_data_is_saved;
    uint8_t can_proto_compatibility;
    SIXsg_config_data_t *SIXsg_config_ptr;
} parser_datastruct_t;


typedef struct
{
    uint8_t size;
} header_tbl_t;

typedef struct
{
    header_tbl_t hd;
    hal_result_t(*func[MAX_CMD_NUM])(uint8_t*, uint8_t*, uint8_t*);
} parser_cmd_func_table_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_result_t s_cmd_set_board_addr(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out);

static hal_result_t s_cmd_get_matrix_rc(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out);
static hal_result_t s_cmd_set_matrix_rc(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out);

static hal_result_t s_cmd_get_ch_dac(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out);
static hal_result_t s_cmd_set_ch_dac(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out);

static hal_result_t s_cmd_get_ch_adc(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out);

static hal_result_t s_cmd_get_matrix_gain(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out);
static hal_result_t s_cmd_set_matrix_gain(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out);

static hal_result_t s_cmd_get_full_scales(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out);
static hal_result_t s_cmd_set_full_scales(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out);

static hal_result_t s_cmd_get_serial_number(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out);
static hal_result_t s_cmd_set_serial_number(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out);

static hal_result_t s_cmd_get_eeprom_status(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out);

static hal_result_t s_cmd_get_calib_tare(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out);
static hal_result_t s_cmd_set_calib_tare(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out);

static hal_result_t s_cmd_get_curr_tare(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out);
static hal_result_t s_cmd_set_curr_tare(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out);

static hal_result_t s_cmd_set_tx_mode(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out);

static hal_result_t s_cmd_set_can_data_rate(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out) ;

static hal_result_t s_cmd_save2eeprom(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out);

static hal_result_t s_cmd_get_fw_version(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out);

static hal_result_t s_cmd_get_ch_gain(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out);

static hal_result_t s_cmd_set_ch_gain(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out);

static hal_result_t s_cmd_unused(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out);




static hal_result_t s_parse_can_loaderMsg(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out);

static void s_error_manage(hal_result_t error);

static void s_frame_fill(hal_can_frame_t *msg_out, uint32_t receivedId, uint8_t *data, uint8_t len);

static void s_tx_mode_check(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static parser_datastruct_t      parser_data;


static const parser_cmd_func_table_t cmd_func_tbl =
{
    .hd = {.size = MAX_CMD_NUM},
    .func = {
//***********************************************************************************************
//      func name                       command name                         command_id = table_index    
//***********************************************************************************************
        NULL,                           // CAN_CMD_NONE                      0x0
        s_cmd_unused,                   // CAN_CMD_SET_IIR                   0x1
        NULL,
        s_cmd_set_matrix_rc,            // CAN_CMD_SET_MATRIX_RC             0x3
        s_cmd_set_ch_dac,               // CAN_CMD_SET_CH_DAC                0x4
        s_cmd_unused,                   // CAN_CMD_SELECT_ACTIVE_CH          0x5
        s_cmd_unused,                   // CAN_CMD_CALIBRATE_OFFSET          0x6
        s_cmd_set_tx_mode,              // CAN_CMD_SET_TXMODE                0x7 
        s_cmd_set_can_data_rate,        // CAN_CMD_SET_CANDATARATE           0x8
        s_cmd_save2eeprom,              // CAN_CMD_SAVE2EE                   0x9
        s_cmd_get_matrix_rc,            // CAN_CMD_GET_MATRIX_RC             0xA 
        s_cmd_get_ch_dac,               // CAN_CMD_GET_CH_DAC                0xB
        s_cmd_get_ch_adc,               // CAN_CMD_GET_CH_ADC                0xC
        s_cmd_unused,                   // CAN_CMD_FILTER_EN                 0xD  
        s_cmd_unused,                   // CAN_CMD_MUX_EN                     0xE
        s_cmd_unused,                   // CAN_CMD_MUX_NUM                    0xF
        s_cmd_unused,                   // CAN_CMD_SET_RESOLUTION            0x10
        s_cmd_set_matrix_gain,          // CAN_CMD_SET_MATRIX_G              0x11 
        s_cmd_get_matrix_gain,          // CAN_CMD_GET_MATRIX_G              0x12
        s_cmd_set_calib_tare,           // CAN_CMD_SET_CALIB_TARE            0x13
        s_cmd_get_calib_tare,           // CAN_CMD_GET_CALIB_TARE            0x14
        s_cmd_set_curr_tare,            // CAN_CMD_SET_CURR_TARE             0x15
        s_cmd_get_curr_tare,            // CAN_CMD_GET_CURR_TARE            0x16
        s_cmd_set_full_scales,          // CAN_CMD_SET_FULL_SCALES           0x17
        s_cmd_get_full_scales,          // CAN_CMD_GET_FULL_SCALES           0x18 
        s_cmd_set_serial_number,        // CAN_CMD_SET_SERIAL_NO             0x19
        s_cmd_get_serial_number,        // CAN_CMD_GET_SERIAL_NO             0x1A
        s_cmd_get_eeprom_status,        // CAN_CMD_GET_EEPROM_STATUS         0x1B
        s_cmd_get_fw_version,           // CAN_CMD_GET_FW_VERSION              0X1C
        s_cmd_get_ch_gain,              // CAN_CMD_GET_CH_GAIN                 0X1D
        s_cmd_set_ch_gain               // CAN_CMD_SET_CH_GAIN                 0X1E
    }

};

// ==> the following commands are unused actually: <==
// CAN_CMD_SET_RESOLUTION
// CAN_CMD_SELECT_ACTIVE_CH
// CAN_CMD_MUX_EN
// CAN_CMD_FILTER_EN
// CAN_CMD_MUX_NUM
// CAN_CMD_SET_IIR


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern hal_result_t parser_init(SIXsg_config_data_t *cfg_ptr)
{
    parser_data.ee_data_is_saved = 1; //todo: metti enum
    if(NULL == cfg_ptr)
    {
        return(hal_res_NOK_nullpointer);
    }
    parser_data.SIXsg_config_ptr = cfg_ptr;
    
    return(hal_res_OK);
}


extern hal_result_t parse_message(void)
{
    hal_result_t res;
    hal_can_frame_t msg_in, msg_out;
    uint8_t len_data_out = 0;
    uint8_t data_out[8];
    uint8_t cmd;
    static int ena_ack = 255;
    static int len = hal_can_frame_payload_maxlen;
    static uint8_t remaining_msg = 0;
    
    res = hal_can_get(hal_can_port1, &msg_in, &remaining_msg); 
    if(hal_res_OK != res)
    {
        return(res);
    }

len =len;
ena_ack = CFG_6SG_BEHAV.send_ack_each_cmd;
ena_ack = 255;
ena_ack = parser_data.SIXsg_config_ptr->behaviour_cfg.send_ack_each_cmd;
    switch(GET_CMD_CLASS(msg_in.id))
    {
        case CAN_MSG_CLASS_POLLING:
        {
            cmd = msg_in.data[0]; //the first byte of payload of msg belonging to CAN_MSG_CLASS_PERIODIC contains the command
            /*
                Note: CAN_CMD_SET_BOARD_ADX value (= 0x32) is outside range of other commands, 
                so it must be managed directly, without function table.
            */
            if(CAN_CMD_SET_BOARD_ADX == cmd)
            {
                res = s_cmd_set_board_addr(msg_in.data, data_out, &len_data_out);
            }
            else if((cmd >= cmd_func_tbl.hd.size) || (NULL == cmd_func_tbl.func[cmd]) )
            {
                res = hal_res_NOK_unsupported;
                break;
            }
            else
            {
                res = cmd_func_tbl.func[cmd](msg_in.data, data_out, &len_data_out);
            }
        } break;
        
        case CAN_MSG_CLASS_LOADER:
        {
            res =  s_parse_can_loaderMsg(msg_in.data, data_out, &len_data_out);
        } break;
        
        default:
        {
            res = hal_res_NOK_wrongparam;
        } break;
    }
    
    if(hal_res_OK != res)
    {
        s_error_manage(res);
        return(hal_res_OK); //actually doesn't care of unkown message and message with invaid params
    }

    // if I'm here it means the message is valid,

    /*
        if message_out's length is bettween 0 and 8, means incomming message needs a rensponse
        if message_out's length is bigger then 8 (255) means the function ha already sent response,
            so here I must not do anything.
        if message_out's length is equal to 0 and the application was been configured to send ack each message
            I must send ack (empty message)
    */
    if( ((len_data_out > 0) && (len_data_out <=  len)) || ( (ena_ack) && (len_data_out == 0)) )
    {
        //s_frame_fill(&msg_out,  /*src*/CFG_GEN_EEDATA.board_address, /*dest*/(GET_CMD_SRC(msg_in.id)), data_out, len_data_out);msg_in.id
        s_frame_fill(&msg_out, msg_in.id, data_out, len_data_out);
        
        res = hal_can_put(hal_can_port1, &msg_out, hal_can_send_normprio_now);
        while(hal_res_NOK_busy == res)
        {
            res = hal_can_put(hal_can_port1, &msg_out, hal_can_send_normprio_now);
        }
    }
    
    
    if(remaining_msg >0)
    {
        eos_foop_SetEvent(eos_foop_GetHandle(), EVT_MSG_RECEIVED);
    }
    return (res);
}






extern hal_result_t parse_test(void)
{
    hal_can_frame_t msg_in, msg_out;
    uint8_t len_data_out;
    uint8_t data_out[8];
    uint8_t cmd;
    uint8_t remaining_msg = 0;
    hal_result_t res = hal_res_NOK_generic;
    hal_result_t txres = hal_res_NOK_generic;
    uint8_t txattempts = 0;
    
    res = hal_can_get(hal_can_port1, &msg_in, &remaining_msg); 
    if(hal_res_OK != res)
    {
        return(res);
    }


    if((msg_in.data[0] == 0x04) && (msg_in.size != 8))
    {
        res = s_cmd_set_ch_dac(&msg_in.data[0], &data_out, &len_data_out);
        return(res);
    }
    
    if((msg_in.data[0] == 0x0B) && (msg_in.size != 8))
    {
        res = s_cmd_get_ch_dac(&msg_in.data[0], &data_out, &len_data_out);
        s_frame_fill(&msg_out, msg_in.id, data_out, len_data_out);
    }
    else
    {

        msg_out.id = msg_in.id;  
        msg_out.id_type = msg_in.id_type;
        msg_out.frame_type = msg_in.frame_type;
        msg_out.size = msg_in.size;
        
        memcpy(&msg_out.data[0], &msg_in.data[0], msg_in.size);
        msg_out.data[0] = 0xA0;
        txattempts = 0;
    }
    
//    cmd = msg_in.data[0];
//    if((cmd%100) == 0)
//    {
//        ampl_set_gain_and_offset();
//    }
    
    do
    {
        txattempts++;
        txres = hal_can_put(hal_can_port1, &msg_out, hal_can_send_normprio_now);
        msg_out.data[0]++;
    } 
    while((txattempts < 10) && (hal_res_OK != txres));


    
//    if(hal_res_OK != res)
//    {
//        s_error_manage(res);
//        return(hal_res_OK); //actually doesn't care of unkown message and message with invaid params
//    }

    
    
    if(remaining_msg >0)
    {
        eos_foop_SetEvent(eos_foop_GetHandle(), EVT_MSG_RECEIVED);
    }
    return (res);
}






// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_result_t s_cmd_set_board_addr(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out) //CAN_CMD_SET_BOARD_ADX
{
    
    if ( !(( data_in[1] > 0 ) && ( data_in[1] <= 15 )) )
    {
        return(hal_res_NOK_wrongparam);
    }
    
    CFG_GEN_EEDATA.board_address = data_in[1];
    
    hal_can_receptionfilter_set(hal_can_port1, 0, 0x70F, 2, (CAN_MSG_CLASS_POLLING|CFG_GEN_EEDATA.board_address ), hal_can_frameID_std);
    hal_can_receptionfilter_set(hal_can_port1, 0, 0x70F, 0, (CAN_MSG_CLASS_LOADER|CFG_GEN_EEDATA.board_address ), hal_can_frameID_std);
    //each time can addres is changed it must be saved in eeprom.
    SIXsg_boardAddress_save_to_eeprom(parser_data.SIXsg_config_ptr);
    
    *len_data_out = 0;

    return(hal_res_OK);

}
        
    
    
static hal_result_t s_cmd_get_matrix_rc(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out)// CAN_CMD_GET_MATRIX_RC: //get MATRIX
{

    if( (data_in[1] >= AN_CHANNEL_NUM) || (data_in[2] >= AN_CHANNEL_NUM) )
    {
        return(hal_res_NOK_wrongparam);
    }

    data_out[0] = CAN_CMD_GET_MATRIX_RC; 
    data_out[1] = data_in[1]; 
    data_out[2] = data_in[2]; 
    data_out[3] = CFG_6SG_EEDATA.tr_matrix[data_in[1]][data_in[2]] >> 8; 
    data_out[4] = CFG_6SG_EEDATA.tr_matrix[data_in[1]][data_in[2]] & 0xFF;  
    *len_data_out = 5;

    return(hal_res_OK);
}


static hal_result_t s_cmd_set_matrix_rc(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out) // CAN_CMD_SET_MATRIX_RC: //set i,j value of transform. matrix:
{ 

    int16_t sender_status;

    if( (data_in[1] >= AN_CHANNEL_NUM) || (data_in[2] >= AN_CHANNEL_NUM) )
    {
        return(hal_res_NOK_wrongparam);
    }
    
    parser_data.ee_data_is_saved = 0;
    
    sender_status = sender_is_running();
    sender_stop();
    
    CFG_6SG_EEDATA.tr_matrix[data_in[1]][data_in[2]] = data_in[3]<<8 | data_in[4];

    if(sender_status)
    {
        sender_start();
    }   
    
    *len_data_out = 0;

    return(hal_res_OK);
}        


static hal_result_t s_cmd_get_ch_dac(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out) //CAN_CMD_GET_CH_DAC:   //get gain of DAC converter 
{
    if(data_in[1] >= AN_CHANNEL_NUM)
    {
        return(hal_res_NOK_wrongparam);
    }
    data_out[0] = CAN_CMD_GET_CH_DAC; 
    data_out[1] = data_in[1];  
    data_out[2] = CFG_6SG_EEDATA.an_channel_offset[data_in[1]] >> 8; 
    data_out[3] = CFG_6SG_EEDATA.an_channel_offset[data_in[1]] & 0xFF; 

    *len_data_out = 4;

    return(hal_res_OK);
}


static hal_result_t s_cmd_set_ch_dac(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out) // CAN_CMD_SET_CH_DAC:  //  set DAC value 0x205 len 4  data 4 ch msb lsb
{           

    if(data_in[1] >= AN_CHANNEL_NUM)
    {
        return(hal_res_NOK_wrongparam);
    }

    parser_data.ee_data_is_saved = 0;
    
    CFG_6SG_EEDATA.an_channel_offset[data_in[1]] = (data_in[2]<<8) | data_in[3];
    /* TODO: pensare bene a come farlo...attualmente l'amplificatore
     riconfigura tutti e sei i canali, ma poi dovremmo permettere che ne faccia uno alla volta!! */
    ampl_set_offset();
    
    *len_data_out = 0;

    return(hal_res_OK);
}            
    
            
    
static hal_result_t s_cmd_get_ch_adc(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out) //CAN_CMD_GET_CH_ADC: get ADC channel
{

    hal_result_t res;
    int16_t value; 
    int16_t ch_values[AN_CHANNEL_NUM];
    calc_data_output_t tf_values;

    if(data_in[1] >= AN_CHANNEL_NUM)
    {
        return(hal_res_NOK_wrongparam);
    }
    

    //***************************************************************************
    //************************* IMPORTANTE **************************************
    //==> FERMA L'ACQUISIZIONE DEI DATI se no si ha race condition!!!
        //hal_timer_interrupt_disa(hal_timerT1);// stop timer to avoid rece condition on strain_cfg.ee_data.EE_AN_ChannelValue
        //hal_timer_stop(hal_timerT1);

    //in realtà qui non serve se uso i due buffer. Ovviamente non è l'ultimissimo valore letto
    
    if(data_in[2] == 0) //raw value
    {
        res = adc_get_ch_value(data_in[1],0, &value);
    }
    else //calibrated data
    {   
        res = adc_get_data(ch_values);
        calculate_torque_and_force_data(ch_values, &tf_values);
        value = tf_values.array[data_in[1]];
    }
    //***************************************************************************
    //************************* IMPORTANTE **************************************
    //==> FAI RIPARTIRE L'ACQUISIZIONE DEI DATI se no si ha race condition!!!    
    //hal_timer_start(hal_timerT1);                
    //hal_timer_interrupt_ena(hal_timerT1);


    if(hal_res_OK != res)
    {
        return(hal_res_NOK_wrongparam);
    }
    value = value + 0x8000;
    
    data_out[0] = CAN_CMD_GET_CH_ADC; 
    data_out[1] = data_in[1];  //channel number
    data_out[2] = data_in[2]; //mode
    data_out[3] = value >> 8; 
    data_out[4] = value & 0xFF;
    *len_data_out = 5;
    return(hal_res_OK);
 
}
        

static hal_result_t s_cmd_get_matrix_gain(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out) //CAN_CMD_GET_MATRIX_G: //get matrix gain
{    
    data_out[0] = CAN_CMD_GET_MATRIX_G; 
    data_out[1] = CFG_6SG_EEDATA.sw_matrix_gain; 

    *len_data_out = 2;
    return(hal_res_OK);     
}
        
    
static hal_result_t s_cmd_set_matrix_gain(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out) //CAN_CMD_SET_MATRIX_G: //set matrix gain
{
    parser_data.ee_data_is_saved = 0;

    //matrix gain is not used in calculations. So it isn't necessary to stop sender
    CFG_6SG_EEDATA.sw_matrix_gain = data_in[1];


    *len_data_out = 0;
    return(hal_res_OK); 

}  
            

static hal_result_t s_cmd_get_full_scales(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out) //CAN_CMD_GET_FULL_SCALES: 
{
    if(data_in[1] >= AN_CHANNEL_NUM)
    {
        return(hal_res_NOK_wrongparam); 
    }

    data_out[0] = CAN_CMD_GET_FULL_SCALES; 
    data_out[1] = data_in[1]; 
    data_out[2] = CFG_6SG_EEDATA.full_scales[data_in[1]] >> 8; 
    data_out[3] = CFG_6SG_EEDATA.full_scales[data_in[1]] & 0xFF; 

    *len_data_out = 4;
    return(hal_res_OK); 
}
        

static hal_result_t s_cmd_set_full_scales(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out) //CAN_CMD_SET_FULL_SCALES: 
{ 
    if(data_in[1] >= AN_CHANNEL_NUM)
    {
        return(hal_res_NOK_wrongparam); 
    }
    parser_data.ee_data_is_saved = 0;

    //full scales is not used in calculations. So it isn't necessary to stop sender
    CFG_6SG_EEDATA.full_scales[data_in[1]]= data_in[2]<<8 | data_in[3];


    *len_data_out = 0;
    return(hal_res_OK); 
}

static hal_result_t s_cmd_get_serial_number(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out) // CAN_CMD_GET_SERIAL_NO:
{
    data_out[0] = CAN_CMD_GET_SERIAL_NO; 
    data_out[1] = CFG_6SG_EEDATA.serial_number[0];
    data_out[2] = CFG_6SG_EEDATA.serial_number[1]; 
    data_out[3] = CFG_6SG_EEDATA.serial_number[2]; 
    data_out[4] = CFG_6SG_EEDATA.serial_number[3];
    data_out[5] = CFG_6SG_EEDATA.serial_number[4]; 
    data_out[6] = CFG_6SG_EEDATA.serial_number[5]; 
    data_out[7] = CFG_6SG_EEDATA.serial_number[6];
    
    *len_data_out = 8;  
    return(hal_res_OK);          
}

static hal_result_t s_cmd_set_serial_number(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out) // CAN_CMD_SET_SERIAL_NO: //set serial number
{
    parser_data.ee_data_is_saved = 0;
 
    CFG_6SG_EEDATA.serial_number[0]= data_in[1];
    CFG_6SG_EEDATA.serial_number[1]= data_in[2];
    CFG_6SG_EEDATA.serial_number[2]= data_in[3];
    CFG_6SG_EEDATA.serial_number[3]= data_in[4];
    CFG_6SG_EEDATA.serial_number[4]= data_in[5];
    CFG_6SG_EEDATA.serial_number[5]= data_in[6];
    CFG_6SG_EEDATA.serial_number[6]= data_in[7];
    CFG_6SG_EEDATA.serial_number[7]= 0;
    
    *len_data_out = 0;
    return(hal_res_OK);
}  
        
      

static hal_result_t s_cmd_get_eeprom_status(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out) // CAN_CMD_GET_EEPROM_STATUS: 
{
    data_out[0] = CAN_CMD_GET_EEPROM_STATUS; 
    data_out[1] = parser_data.ee_data_is_saved;

    *len_data_out = 2; 
    return(hal_res_OK);       
}

static hal_result_t s_cmd_get_calib_tare(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out) // CAN_CMD_GET_CALIB_TARE:
{

    if(data_in[1] >= AN_CHANNEL_NUM)
    {
        return(hal_res_NOK_wrongparam); 
    }

    data_out[0] = CAN_CMD_GET_CALIB_TARE; 
    data_out[1] = data_in[1];
    data_out[2] = CFG_6SG_EEDATA.calibration_tare[data_in[1]] >> 8; 
    data_out[3] = CFG_6SG_EEDATA.calibration_tare[data_in[1]] & 0xFF; 
    
    *len_data_out = 4;  
    return(hal_res_OK);        
}
        
    
static hal_result_t s_cmd_set_calib_tare(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out) // CAN_CMD_SET_CALIB_TARE: 
{
    uint8_t i;
    int16_t ch_values[AN_CHANNEL_NUM];
    hal_result_t res = hal_res_OK;
    int16_t sender_status;
    adc_state_t adc_st;

    sender_status = sender_is_running();
    sender_stop();    

    
    switch(data_in[1])
    {
        case 0:
        {
            memset( CFG_6SG_EEDATA.calibration_tare, 0, (sizeof(uint16_t)*AN_CHANNEL_NUM) );
        } break;
        
        case 1:
        {
            adc_st =adc_is_running();
            adc_stop();

            adc_get_data(ch_values);
;
            if(adc_st_started == adc_st)
            {
                adc_start();
            }

            for (i=0; i<AN_CHANNEL_NUM; i++)
            {
                CFG_6SG_EEDATA.calibration_tare[i]= -(ch_values[i]);
            }
        } break;
        
        case 2:
        {
            if(data_in[2] >= AN_CHANNEL_NUM)
            {
                res= hal_res_NOK_wrongparam;
                break;
            }

            CFG_6SG_EEDATA.calibration_tare[data_in[2]] = (data_in[3]<<8) | data_in[4];
        } break;
        
        default:
        {
            res= hal_res_NOK_wrongparam;
        } break;
    
    }
    
    if(sender_status)
    {
        sender_start();
    }

    if(hal_res_OK != res)
    {
        parser_data.ee_data_is_saved = 0;
    }

    *len_data_out = 0; 
    return(res);
}



static hal_result_t s_cmd_get_curr_tare(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out) // CAN_CMD_GET_CURR_TARE: 
{
    hal_result_t res;
    int16_t value;

    res = calc_current_tare_get_value(data_in[1], &value);
    if(hal_res_OK != res)
    {
        return(res);
    }

    data_out[0] = CAN_CMD_GET_CURR_TARE; 
    data_out[1] = data_in[1];
    data_out[2] = value >> 8; 
    data_out[3] = value & 0xFF; 
    
    *len_data_out = 4; 

    return(hal_res_OK);          
}
    
static hal_result_t s_cmd_set_curr_tare(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out) // CAN_CMD_SET_CURR_TARE
{ 
    hal_result_t res = hal_res_OK;
    int16_t sender_status;
    adc_state_t adc_st;

//******************************************************************************
//********************* IMPORTANTE***********************************************
//ferma la spediziobne di messaggi perchè la current tare è coinvolta nei calcoli
    sender_status = sender_is_running();
    sender_stop();    
    
    switch(data_in[1])
    {
        case 0:
        {
            calc_current_tare_init();
        } break;
        
        case 1:
        {
            /* In this case I must stop adc because new tare values depend on adc values.*/
            adc_st = adc_is_running();
            adc_stop();

            calc_current_tare_reset();

            if(adc_st_started == adc_st)
            {
                adc_start();
            }
        } break;
        
        case 2:
        {

            res = calc_current_tare_set_value(data_in[2], (data_in[3]<<8 | data_in[4]));
        } break;

#warning -> acemor: nei default si devono mettere i break.        
        default:
        {
            res = hal_res_NOK_wrongparam; 
        } break;
    
    }

    if(sender_status)
    {
        sender_start();
    }

    if(hal_res_OK != res)
    {
        return(hal_res_NOK_wrongparam); 
    }

    *len_data_out = 0;
    return(res); 
}  
    


//#warning MODIFICATA LA TXMODE
//static hal_result_t s_cmd_set_tx_mode(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out) // CAN_CMD_SET_TXMODE: // set continuous or on demand tx  0x205 len 2  data 7 0/1
//{
//
//
//    switch(data_in[1])
//    {
//        case 0://transmit calibrated data
//        {
//            sender_stop();
//            adc_stop();
//        }
//        break;
//
//        case 1: //do acquisition, but do not transmit
//        {
//            CFG_6SG_BEHAV.tx_outMsg_mode = tx_outMsg_off;
//        }
//        break;
//
//        case 2: //debug mode
//        {
//            
//        }
//        break;
//
//        case 3: //TODO: transmit not calibrated data
//        {
//            CFG_6SG_BEHAV.tx_outMsg_mode = tx_outMsg_uncalibData_on;
//            adc_start();
//            sender_start();
//        }
//        break;
//
//        case 4: //TODO: transmit calibrated and not calibrated
//        {
//
//        }
//        break;
//
//
//    }
//    
//    *len_data_out = 0;
//    return(hal_res_OK);
//}
        
static hal_result_t s_cmd_set_tx_mode(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out) // CAN_CMD_SET_TXMODE: // set continuous or on demand tx  0x205 len 2  data 7 0/1
{
    sender_stop();
    adc_stop();

    switch(data_in[1])
    {
        case 0://transmit calibrated data
        {
            CFG_6SG_BEHAV.tx_outMsg_mode = tx_outMsg_torqueData_on;
            s_tx_mode_check();
            adc_start();
            sender_start();
        } break;

        case 1: //do acquisition, but do not transmit
        {
            CFG_6SG_BEHAV.tx_outMsg_mode = tx_outMsg_readOnly;
            adc_start();
        } break;

        case 2: //debug mode
        {
            CFG_6SG_BEHAV.tx_outMsg_mode = tx_outMsg_off;
            
        } break;

        case 3: //TODO: transmit not calibrated data
        {
            CFG_6SG_BEHAV.tx_outMsg_mode = tx_outMsg_uncalibData_on;
            sendAmsg = 1;
            sendBmsg = 1;
            adc_start();
            sender_start();
        } break;

        case 4: //TODO: transmit calibrated and not calibrated
        {

        } break;


    }
    
    *len_data_out = 0;
    return(hal_res_OK);
}
    

static void s_tx_mode_check(void)
{
    uint8_t ri, ci;
    sendAmsg = 0;
    sendBmsg = 0;
    
    for(ci=0; (ci<3) && (sendAmsg==0); ci++)
    {
        for(ri=0; (ri<6) && (sendAmsg==0); ri++)
        {
            if(CFG_6SG_EEDATA.tr_matrix[ci][ri] != 0)
            {
                sendAmsg = 1;
            }
        }
    }
    
    for(ci=3; (ci<6) && (sendBmsg==0); ci++)
    {
        for(ri=0; (ri<6) && (sendBmsg==0); ri++)
        {
            if(CFG_6SG_EEDATA.tr_matrix[ci][ri] != 0)
            {
                sendBmsg = 1;
            }
        }
    }
}            

static hal_result_t s_cmd_set_can_data_rate(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out) // CAN_CMD_SET_CANDATARATE: // set datarate for transmission in milliseconds 
{
    uint16_t sender_status;


    if( (data_in[1] < 1) || (data_in[1] > 210) ) //Todo: rivedere tempo massimo
    {
        return(hal_res_NOK_wrongparam);
    }

    parser_data.ee_data_is_saved = 0;

    sender_status = sender_is_running();
    sender_stop();

    CFG_6SG_EEDATA.can_msg_datarate = data_in[1];

    #ifdef _TIMER_HW_
        sender_config_datarate();
    #endif
    if(sender_status)
    {
        sender_start();
    }
    
    *len_data_out = 0;
    return(hal_res_OK);
}
            


static hal_result_t s_cmd_save2eeprom(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out) // CAN_CMD_SAVE2EE: // Save configuration data to EE
{
    hal_result_t res;

    *len_data_out = 0;
    if(1 == parser_data.ee_data_is_saved)
    {
        return(hal_res_OK);
    }
    
    res = SIXsg_config_save_to_eeprom(parser_data.SIXsg_config_ptr); 
    if(hal_res_OK != res)
    {
        return(res); 
    }
    
    parser_data.ee_data_is_saved=1;
    return(hal_res_OK);
}

static hal_result_t s_cmd_get_fw_version(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out) // CAN_CMD_GET_FW_VERSION:
{
    srcCode_version_info_t* src_ver_ptr = &parser_data.SIXsg_config_ptr->gen_ee_data.verinfo;

    if( (src_ver_ptr->can_protocol.major == data_in[1]) &&
        (src_ver_ptr->can_protocol.minor == data_in[2]) )
    {
        parser_data.can_proto_compatibility = 1;
    }
    else
    {
        parser_data.can_proto_compatibility = 0;
    }

    data_out[0] = CAN_CMD_GET_FW_VERSION;
    data_out[1] = BOARD_TYPE_6SG; 
    data_out[2] = src_ver_ptr->exe_file.major;
    data_out[3] = src_ver_ptr->exe_file.minor;
//    data_out[4] = src_ver_ptr->exe_file.build;
#warning VALE: cosa metta nella versione del build???

    data_out[5] = src_ver_ptr->can_protocol.major;
    data_out[6] = src_ver_ptr->can_protocol.minor;            
    data_out[7] = parser_data.can_proto_compatibility;
    
    
    *len_data_out = 8;   
    return(hal_res_OK);
        
}

static hal_result_t s_cmd_get_ch_gain(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out) // CAN_CMD_GET_CH_GAIN:
{
    if(data_in[1] >= AN_CHANNEL_NUM)
    {
        return(hal_res_NOK_wrongparam);
    }

    data_out[0] = CAN_CMD_GET_CH_GAIN;
    data_out[1] = data_in[1];
    data_out[2] = (CFG_6SG_EEDATA.an_channel_gain[data_in[1]].first_step) >> 8;
    data_out[3] = (CFG_6SG_EEDATA.an_channel_gain[data_in[1]].first_step) &0xFF; 
    data_out[4] = (CFG_6SG_EEDATA.an_channel_gain[data_in[1]].second_step) >> 8;
    data_out[5] = (CFG_6SG_EEDATA.an_channel_gain[data_in[1]].second_step) &0xFF; 
 

    *len_data_out = 6;   
    return(hal_res_OK);

}

static hal_result_t s_cmd_set_ch_gain(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out) // CAN_CMD_SET_CH_GAIN:
{
    if(data_in[1] >= AN_CHANNEL_NUM)
    {
        return(hal_res_NOK_wrongparam);
    }
    
    parser_data.ee_data_is_saved = 0;

    CFG_6SG_EEDATA.an_channel_gain[data_in[1]].first_step = (data_in[2]<<8) | data_in[3];  
    CFG_6SG_EEDATA.an_channel_gain[data_in[1]].second_step = (data_in[4]<<8) | data_in[5];  

    //TODO: 
    ampl_set_gain_and_offset();
    
    *len_data_out = 0;   
    return(hal_res_OK);

}


static hal_result_t s_cmd_unused(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out) // CAN_CMD_SET_RESOLUTION: // set data resolution   
{
    // IGNORE THESE COMMANDS
    *len_data_out = 255;

    return (hal_res_NOK_unsupported);
}



static hal_result_t s_parse_can_loaderMsg(uint8_t *data_in, uint8_t *data_out, uint8_t *len_data_out)
{
    uint8_t num_msg;
    static uint8_t addinfo_part=0; 
    hal_can_frame_t frame;
    hal_result_t res = hal_res_OK;
    

    srcCode_version_info_t* src_ver_ptr = &(parser_data.SIXsg_config_ptr->gen_ee_data.verinfo);
    switch (data_in[0])
    {
#warning -> acemor: qui al CMD_BROADCAST si risponde con tre bytes: version, release, build mentre nel bootloader con due. ???    
        case CMD_BROADCAST: 
        {
            CFG_6SG_BEHAV.tx_outMsg_mode = tx_outMsg_off;
            sender_stop();
            adc_stop();
            
            data_out[0] = CMD_BROADCAST;
            data_out[1] = BOARD_TYPE_6SG; 
            data_out[2] = src_ver_ptr->exe_file.major;    //Firmware version number for BOOTLOADER
            data_out[3] = src_ver_ptr->exe_file.minor; 
            data_out[4] = 0;
 //           data_out[4] = src_ver_ptr->exe_file.build;        //Firmware build number.
#warning VALE: --> cosa metto nel ver build???
            *len_data_out = 5; 
            
        } break;
                
    
        case CMD_BOARD:
        {
            hal_sys_systemreset(); 
            //asm ("reset");    // Jump to bootloader code      
        } break;
    
        case CMD_GET_ADDITIONAL_INFO:
        {
            frame.id = CAN_MSG_CLASS_LOADER | ( CFG_GEN_EEDATA.board_address << 4 ) | (0);;
            frame.id_type = hal_can_frameID_std;
            frame.frame_type = hal_can_frame_data;
            frame.size = 6;
             
            #warning -> acemor: cosa e' la additional info? sta nella parte rw comune sia a bootloader che ad applicazione?
            
            frame.data[0] = CMD_GET_ADDITIONAL_INFO;
            //since additional_info's length is 32 bytes and in each message write 4 bytes of additional_info,
            // i have to send 8 message
            for (num_msg = 0; num_msg< 8; num_msg++)
            {
                frame.data[1] = num_msg;

                memcpy(&frame.data[2], &CFG_6SG_EEDATA.additional_info[num_msg*4], 4);

                res = hal_can_put(hal_can_port1, &frame, hal_can_send_normprio_now);
                while(hal_res_NOK_busy == res)
                {
                    res = hal_can_put(hal_can_port1, &frame, hal_can_send_normprio_now); 
                }
            }
            *len_data_out = 255;
        
        } break;
    
        case CMD_SET_ADDITIONAL_INFO:
        {

            addinfo_part = data_in[1];

            memcpy(&(CFG_6SG_EEDATA.additional_info[addinfo_part*4]), &data_in[2], 4);
            
            if (7 == addinfo_part)
            {
                addinfo_part=0;
                SIXsg_config_save_to_eeprom(parser_data.SIXsg_config_ptr);
            }
            else
            {                    
                addinfo_part++;
            }
            *len_data_out = 0;
        
        } break;
        
                
        default:
        {
            return(hal_res_NOK_wrongparam);
        } break;
        
        
    } //end switch
    
    return(res);
}

static void s_error_manage(hal_result_t error)
{
    return;
}

static void s_frame_fill(hal_can_frame_t *msg_out, uint32_t receivedId, uint8_t *data, uint8_t len)
{
    msg_out->id_type    = hal_can_frameID_std;
    msg_out->frame_type = hal_can_frame_data;
    msg_out->id         = (GET_CMD_CLASS(receivedId) | (CFG_GEN_EEDATA.board_address << 4) | (GET_CMD_SRC(receivedId)));
    msg_out->size       = len;
    memcpy (msg_out->data, data, len);    
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



