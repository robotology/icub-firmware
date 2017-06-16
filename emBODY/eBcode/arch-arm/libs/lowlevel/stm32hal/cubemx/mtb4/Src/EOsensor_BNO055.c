/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
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
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

//to be added later
//#include "EoCommon.h" 
//#include "EOtheErrorManager.h"

#include "stdint.h"
#include "stm32f4.h"

#include "eventviewer.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "EOsensor_BNO055.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------




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
enum {eo_sens_bno055_command_startByte = 0xAA };
enum {eo_sens_bno055_command_header_length = 0x04};
enum {eo_sens_bno055_command_total_max_length = eo_sens_bno055_command_header_length + eo_sens_bno055_command_maxNumOfBytes};


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_eo_sens_bno055_init_sensor(void);
static void s_eo_sens_bno055_send_raw(uint8_t *data, uint8_t len);
static uint8_t s_eo_sens_bno055_readOneByte_raw(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static uint8_t buff_out[eo_sens_bno055_command_total_max_length] = {0};

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern void eo_sens_bno055_initialize(void)
{
   s_eo_sens_bno055_init_usart();
   s_eo_sens_bno055_init_sensor();
   
}

extern void eo_sens_bno055_sendCommand(eo_sens_bno055_command_t *cmd, uint8_t *data)
{
    if((NULL==cmd) || ( (NULL==data) && (cmd->operation == eo_sens_bno055_cmdOp_write) ))
        return;
        
    if(cmd->numofbytes > eo_sens_bno055_command_maxNumOfBytes)
        return;
        
        
    uint8_t byte2send = 0;
    uint8_t total_length = 0;
    
    //prepare data to send
    buff_out[0] = eo_sens_bno055_command_startByte;
    buff_out[1] = cmd->operation;
    buff_out[2] = cmd->regAddr;
    buff_out[3] = cmd->numofbytes;
    if(data!=NULL)
        memcpy(&buff_out[4], data, cmd->numofbytes);
    
    total_length = cmd->numofbytes + eo_sens_bno055_command_header_length;
    for(byte2send=0; byte2send < total_length; byte2send++)
    {
        while(USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET); //wait for empty
        USART_SendData(USART6, buff_out[byte2send]); 
    }
}


extern void eo_sens_bno055_sendWriteCommand(eo_sens_bno055_registerAddress_t regAddr, uint8_t *data, uint8_t len)
{
    if(NULL == data)
        return;
    
    uint8_t total_length = eo_sens_bno055_command_header_length + len;
    
    buff_out[0] = eo_sens_bno055_command_startByte;
    buff_out[1] = eo_sens_bno055_cmdOp_write;
    buff_out[2] = regAddr;
    buff_out[3] = len;
    memcpy(&buff_out[4], data, len);
    
    s_eo_sens_bno055_send_raw(&buff_out[0], total_length);
}


extern void eo_sens_bno055_sendReadCommand(eo_sens_bno055_registerAddress_t regAddr, uint8_t numOfByteToRead)
{

    buff_out[0] = eo_sens_bno055_command_startByte;
    buff_out[1] = eo_sens_bno055_cmdOp_read;
    buff_out[2] = regAddr;
    buff_out[3] = numOfByteToRead;
    
    s_eo_sens_bno055_send_raw(&buff_out[0], eo_sens_bno055_command_header_length);
}


extern void eo_sens_bno055_readResponse_raw(uint8_t *data, uint8_t *len)
{
    data[0] = s_eo_sens_bno055_readOneByte_raw(); //should be 0xEE or 0xBB
    
    
    if(0xEE == data[0])
    {
        data[1] = s_eo_sens_bno055_readOneByte_raw();
        *len=2;
        return;
    }
    if(0xBB == data[0])
    {   
        data[1] = s_eo_sens_bno055_readOneByte_raw(); //thi is the number of byte i'll receive
        
        uint8_t numofbyte2rec = data[1];
        uint8_t i = 0;
        uint8_t indexofdata = 2;
        for(uint8_t i=0; i<numofbyte2rec; i++)
        {
            data[indexofdata] = s_eo_sens_bno055_readOneByte_raw();
            indexofdata++;
        }
        *len = numofbyte2rec + 2 /*data[0] and data[1]*/;
        return;
    }
    
    //error
    *len = 1;
    return;
}

extern uint8_t eo_sens_bno055_check_fbk_is_ok(uint8_t *data, uint8_t len)
{
    if(2 != len)
        return 0;
    
    if(data[0] != 0xEE)
        return 0;
    
    if(data[1] != 0x01)
        return 0;
    
    return 1;
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
extern  void s_eo_sens_bno055_init_usart(void)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE); //RCC_APB1Periph_UART5???

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_USART6); 
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_USART6); 

    /* Configure USART1 pins:  Rx and Tx ----------------------------*/
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART6, &USART_InitStructure);

    USART_Cmd(USART6,ENABLE);

}

//static void s_eo_sens_bno055_init_sensor(void)
//{

//    eo_sens_bno055_command_t mycmd;
//    uint8_t resp[30] = {0};
//    uint8_t respLen = 0;
//    uint8_t fbk = 0;
//    uint8_t data = 0;
//    
//    //common to following commands
//    mycmd.operation = eo_sens_bno055_cmdOp_write;
//    mycmd.numofbytes = 1;
//    
//    //1) set external clock
//    mycmd.regAddr = eo_sens_bno055_register_SYS_CLK_STATUS;
//    data = 0x80; //set external clock
//    eo_sens_bno055_sendCommand(&mycmd, &data);
//    
//    eo_sens_bno055_readResponse_raw(resp, &respLen);
//    fbk = eo_sens_bno055_check_fbk_is_ok(resp, respLen);
//    
//    if(!fbk)
//        return;
//    
//    //2) set operational mode NDOF
//    mycmd.regAddr = eo_sens_bno055_register_OP_MODE;
//    data = 0x0C; //set NDOF_MODE
//    eo_sens_bno055_sendCommand(&mycmd, &data);
//    
//    eo_sens_bno055_readResponse_raw(resp, &respLen);
//    fbk = eo_sens_bno055_check_fbk_is_ok(resp, respLen);
//    
//    if(!fbk)
//        return;

//}

static void s_eo_sens_bno055_init_sensor(void)
{

    
    uint8_t resp[30] = {0};
    uint8_t respLen = 0;
    uint8_t fbk = 0;
    uint8_t data = 0;
    uint8_t numOfBytes = 1;
    
    //1) set external clock
    data = 0x80; //set external clock
    eo_sens_bno055_sendWriteCommand(eo_sens_bno055_register_SYS_TRIGGER, &data, numOfBytes);
    
    eo_sens_bno055_readResponse_raw(resp, &respLen);
    fbk = eo_sens_bno055_check_fbk_is_ok(resp, respLen);
    
    if(!fbk)
        return;
    
    //2) set operational mode NDOF
    data = 0x0C; //set NDOF_MODE
    eo_sens_bno055_sendWriteCommand(eo_sens_bno055_register_OP_MODE, &data, numOfBytes);
    
    eo_sens_bno055_readResponse_raw(resp, &respLen);
    fbk = eo_sens_bno055_check_fbk_is_ok(resp, respLen);
    
    if(!fbk)
        return;

}

static void s_eo_sens_bno055_send_raw(uint8_t *data, uint8_t len)
{
    //event viewer
    evEntityId_t prev;
    prev = eventviewer_switch_to(ev_ID_first_usrdef+3);
    for(uint8_t i=0; i < len; i++)
    {
        while(USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET); //wait for empty
        USART_SendData(USART6, data[i]); 
    }
    eventviewer_switch_to(prev);
}

static uint8_t s_eo_sens_bno055_readOneByte_raw(void)
{
        //event viewer
    evEntityId_t prev;
    uint8_t data;
    
    prev = eventviewer_switch_to(ev_ID_first_usrdef+4);
    
    eventviewer_switch_to(ev_ID_first_usrdef+5);
    while(USART_GetFlagStatus(USART6, USART_FLAG_RXNE) == RESET); //wait for something
    eventviewer_switch_to(ev_ID_first_usrdef+4);
    data = USART_ReceiveData(USART6);
    eventviewer_switch_to(prev);
    return(data);
    
}
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



