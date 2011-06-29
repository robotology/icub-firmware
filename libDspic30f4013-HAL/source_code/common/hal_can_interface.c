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
/*	@file 	hal_can_interface.c
	@brief 	This file implements can driver.
	@author claudio.lorini@iit.it, marco.maggiali@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
	@date	-
*/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include <p30f4013.h>
#include <libpic30.h>
#include <string.h>
#include <can.h>

#include "hal_errors.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "hal_can_interface.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


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
static void s_can_isr_tx(hal_can_port_t can_port);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// Can RX BUFFER message buffer pointer
static int16_t s_can_buf_rx_index = 0; 
// can messages buffer
static hal_canmsg_t s_can_buffer_rx[CAN_MAX_MESSAGES_IN_BUFF];


/// CAN TX BUFFER 
static int16_t s_can_buf_tx_index;
static int16_t testcounter;
static hal_canmsg_t s_can_buffer_tx[CAN_TX_SOFTWARE_BUFFER_SIZE];


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern void hal_can_init(hal_can_port_t can_port)
{
  // Set request for configuration (inizialization) mode
  // Fcan=20MHz
  CAN1SetOperationMode(CAN_IDLE_CON & CAN_MASTERCLOCK_1 & CAN_REQ_OPERMODE_CONFIG & CAN_CAPTURE_DIS);
  // C1CTRL = CAN_CAPTURE_DIS & CAN_IDLE_CON & CAN_MASTERCLOCK_1 & CAN_REQ_OPERMODE_CONFIG ;
  while(C1CTRLbits.OPMODE <=3);

  // Load configuration register
  // 1Mbps: 
  // PRESCALER = 1 -> Tq = 4/Fcan, 
  // SINCROJUMP=1q, PHASE1=3q, PHASE2=5q,  PROPAGATION=1q  
  // q=1+3+5+1=10 quanti -> 1Meg 
  CAN1Initialize(CAN_SYNC_JUMP_WIDTH1 & CAN_BAUD_PRE_SCALE(1), CAN_WAKEUP_BY_FILTER_DIS &  CAN_PHASE_SEG2_TQ(5) & 
    CAN_PHASE_SEG1_TQ(3) & CAN_PROPAGATIONTIME_SEG_TQ(1) & CAN_SEG2_FREE_PROG & CAN_SAMPLE1TIME);
  // C1CFG1 = CAN_SYNC_JUMP_WIDTH1 & CAN_BAUD_PRE_SCALE(1)
  // C1CFG2 = CAN_WAKEUP_BY_FILTER_DIS & CAN_PHASE_SEG2_TQ(5) & CAN_PHASE_SEG1_TQ(3) & CAN_PROPAGATIONTIME_SEG_TQ(4) & CAN_SEG2_FREE_PROG & CAN_SAMPLE1TIME

  // Set transmitter and receiver mode
  char tx_rx_no = 0;  // C1TX0CON
  //CAN1SetTXMode(tx_rx_no, CAN_TX_REQ & CAN_TX_PRIORITY_HIGH );
  CAN1SetTXMode(tx_rx_no, CAN_TX_PRIORITY_HIGH );
  CAN1SetRXMode(tx_rx_no, CAN_RXFUL_CLEAR & CAN_BUF0_DBLBUFFER_EN);

  // Configure CAN IRQs 
  ConfigIntCAN1(CAN_INDI_INVMESS_EN & CAN_INDI_WAK_DIS & CAN_INDI_ERR_DIS &
    CAN_INDI_TXB2_DIS & CAN_INDI_TXB1_DIS & CAN_INDI_TXB0_EN &  // CAN TX IRQ ON
    CAN_INDI_RXB1_EN & CAN_INDI_RXB0_EN ,                        // CANRX IRQ ON 
    CAN_INT_PRI_3 & CAN_INT_ENABLE);
    
  s_can_buf_tx_index=-1;   
  s_can_buf_rx_index=0;
}



extern void hal_can_boardFilter_set(hal_can_port_t can_port, uint8_t can_board_addr)
{
    // Enable CAN IRQ
    DisableIntCAN1;
	
	// Set request for configuration (inizialization) mode
	CAN1SetOperationMode(CAN_IDLE_CON & CAN_MASTERCLOCK_1 & CAN_REQ_OPERMODE_CONFIG & CAN_CAPTURE_DIS);
	while(C1CTRLbits.OPMODE <=3);
	
	CAN1SetFilter(0, CAN_FILTER_SID( (CAN_MSG_CLASS_POLLING|can_board_addr) ) // 0x205
	  & CAN_RX_EID_DIS, CAN_FILTER_EID(0));

	CAN1SetFilter(1, CAN_FILTER_SID( (CAN_MSG_CLASS_LOADER|can_board_addr) )  // 0x705
    & CAN_RX_EID_DIS, CAN_FILTER_EID(0));

	CAN1SetFilter(2, CAN_FILTER_SID( (CAN_MSG_CLASS_LOADER|0x0F) )  // 0x70F
    & CAN_RX_EID_DIS, CAN_FILTER_EID(0));
	
	// set acceptance mask
	// answer commands from any source //0x70F
	CAN1SetMask(0, CAN_MASK_SID(0x70F) & CAN_MATCH_FILTER_TYPE, CAN_MASK_EID(0));
    CAN1SetMask(1, CAN_MASK_SID(0x70F) & CAN_MATCH_FILTER_TYPE, CAN_MASK_EID(0));
	
	// abort any spurious Tx
	CAN1AbortAll();
	
	// Set Operation Mode  NORMAL
	CAN1SetOperationMode(CAN_IDLE_CON & CAN_CAPTURE_DIS &	CAN_MASTERCLOCK_1 & CAN_REQ_OPERMODE_NOR); 
	
	// Enable CAN IRQ
	EnableIntCAN1;
}



extern hal_result_t hal_can_put(hal_can_port_t can_port, uint8_t can_board_addr, uint16_t MessageID,
                             hal_can_frameType_t FrameType, uint8_t Length, uint8_t *Data)
{
    uint8_t i=0;

    //CAN1_TX_DIS; 
    DisableIntCAN1;
  
    if (s_can_buf_tx_index<(CAN_TX_SOFTWARE_BUFFER_SIZE-1))
    {
        s_can_buf_tx_index++;
        s_can_buffer_tx[s_can_buf_tx_index].CAN_Msg_ID=MessageID;
        s_can_buffer_tx[s_can_buf_tx_index].CAN_Poll_Msg_CType=FrameType;
        
        s_can_buffer_tx[s_can_buf_tx_index].CAN_Msg_Length=Length;
        for (i=0;i<Length;i++)
        {
            s_can_buffer_tx[s_can_buf_tx_index].CAN_Msg_PayLoad[i]=Data[i];  
        }
        if (s_can_buf_tx_index==0)
        {
            while (!CAN1IsTXReady(0));
            C1INTFbits.TX0IF=1; //Transmit buffer 0 interurpt flag bit
								//interrupt requet has occurred
            //	C1TX0CONbits.TXREQ=1;
        }

        EnableIntCAN1;  
        return(HAL_RES_OK);
    } else
    {
	    
	    //CAN_TX_OVERRUN
        s_can_buf_tx_index=-1;
        MessageID= 0x100;
        MessageID |= can_board_addr << 4;
        while (!CAN1IsTXReady(0));    
        CAN1SendMessage((CAN_TX_SID(MessageID)) & CAN_TX_EID_DIS & CAN_SUB_NOR_TX_REQ,
                        (CAN_TX_EID(0)) & CAN_NOR_TX_REQ, 
                        Data, 0,0);
        return(HAL_RES_NOK);
    }
}



extern uint8_t hal_can_buff_rx_isEmpty(hal_can_port_t can_port)
{
    if(0 == s_can_buf_rx_index)
	{
	    return 1;
	}
	else
	{
	    return 0;
	}
	
}


extern hal_result_t hal_can_get(hal_can_port_t can_port, hal_canmsg_t *msg_ptr)
{
    if(0 == msg_ptr)
	{
	    return(HAL_RES_NOK);
	}
    
	DisableIntCAN1;
    memcpy(msg_ptr,&s_can_buffer_rx[s_can_buf_rx_index-1],sizeof(hal_canmsg_t));
    s_can_buf_rx_index--;
    EnableIntCAN1;
	 return(HAL_RES_OK);
	
}






/**
  * @brief CAN IRQ Service Routines
  * @param
  * @retval
  */
void __attribute__((interrupt, no_auto_psv)) _C1Interrupt(void)
{   

 /* 
  // Tx IRQs should be disabled
  if(C1INTFbits.TX0IF)      //  Tx0 Interrupt
    C1INTFbits.TX0IF = 0;   
  else 
  {
    if(C1INTFbits.TX1IF)    //  Tx1 Interrupt
      C1INTFbits.TX1IF = 0; 
  }
  
  */
  	//ready to send a CAN message
    if (C1INTFbits.TX0IF || C1INTFbits.TX1IF || C1INTFbits.TX2IF)
    {
        s_can_isr_tx(hal_can_portCAN1);
    }
  
  
  if(C1INTFbits.RX0IF)
  {
    C1INTFbits.RX0IF = 0;   // clear Rx0 Interrupt
    
    // Read received data from receive buffer and store it into user defined dataarray
    CAN1ReceiveMessage(&s_can_buffer_rx[s_can_buf_rx_index].CAN_Per_Msg_PayLoad[0], 8, 0);
    s_can_buffer_rx[s_can_buf_rx_index].CAN_Msg_AsBytes[0] = C1RX0SID; // get Dest, Source and Class 

    // clear Rx buffer0 full flag
    C1RX0CONbits.RXFUL = 0;
 
    // increase buff pointer
    if (s_can_buf_rx_index < (CAN_MAX_MESSAGES_IN_BUFF-1))
      s_can_buf_rx_index++;
    else
      hal_error_manage(ERR_CAN_RXBUFF_OVERFLOW);
  }
  else
  {
    if(C1INTFbits.RX1IF)
    {
      C1INTFbits.RX1IF = 0; //If the Interrupt is due to Receive1 of CAN1 Clear the Interrupt
  
      // Read received data from receive buffer and store it into user defined dataarray
      CAN1ReceiveMessage(&s_can_buffer_rx[s_can_buf_rx_index].CAN_Per_Msg_PayLoad[0], 8, 1);
      s_can_buffer_rx[s_can_buf_rx_index].CAN_Msg_AsBytes[0] = C1RX1SID; // get Dest, Source and Class 
      // clear Rx buffer0 full flag
      C1RX1CONbits.RXFUL = 0;
 
      // increase buff pointer
      if (s_can_buf_rx_index < (CAN_MAX_MESSAGES_IN_BUFF-1))
        s_can_buf_rx_index++;
      else
        hal_error_manage(ERR_CAN_RXBUFF_OVERFLOW);
    }
  }
  
  	if (C1INTFbits.WAKIF) C1INTFbits.WAKIF = 0; // Add wake-up handler code
	if (C1INTFbits.ERRIF) C1INTFbits.ERRIF = 0; // Add error handler code
	if (C1INTFbits.IVRIF) C1INTFbits.IVRIF = 0;
    
 	if ( (C1INTF & C1INTE) == 0 ) IFS1bits.C1IF = 0;  
    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static void s_can_isr_tx(hal_can_port_t can_port)
{
    unsigned char buffer;
    DisableIntCAN1;
    
    switch (C1CTRLbits.ICODE)
    {
    	case 4: 
    	{
    		C1INTFbits.TX0IF = 0; 
    		buffer=0;
    	}	
    	break;
		
		case 3: 
		{
			C1INTFbits.TX1IF = 0; 
			buffer=1;
		}	
		break;
		
		case 2: 
		{
			C1INTFbits.TX2IF = 0;
			buffer=2; 
		}
		break;
    }
    if (s_can_buf_tx_index!=-1)
    {
        testcounter++;
        if (testcounter>=255) testcounter=0;
       CAN1SendMessage((CAN_TX_SID(s_can_buffer_tx[s_can_buf_tx_index].CAN_Msg_ID)) & CAN_TX_EID_DIS & CAN_SUB_NOR_TX_REQ,
                        (CAN_TX_EID(0)) & CAN_NOR_TX_REQ,
                        s_can_buffer_tx[s_can_buf_tx_index].CAN_Msg_PayLoad,
                        s_can_buffer_tx[s_can_buf_tx_index].CAN_Msg_Length,
                        buffer);
  
        s_can_buf_tx_index--;
        if (s_can_buf_tx_index!=-1)
        {
            EnableIntCAN1;
        }
    }
    EnableIntCAN1;
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



