 /****************************************************
 *  BCB board - iCub Facility - 2013                 *
 *  I2C configuration                                *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/

#include "stm32f10x.h"
#include "stm32f10x_dma.h"
#include "bmon.h"
#include "globals.h"


uint32_t BMON_TimeOut = TIMEOUT_MAX; /*<! Value of Timeout when I2C communication fails */


// ----------------------------------------------------------------------------
// Initialize I2C pins, Baudrate
// ----------------------------------------------------------------------------
void BMON_Init (void) {

    I2C_InitTypeDef   I2C_InitStructure;

    GPIO_InitTypeDef  GPIO_InitStructure;

    /*!< BMON_I2C Periph clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    /*!< BMON_I2C_SCL_GPIO_CLK, LM75_I2C_SDA_GPIO_CLK 
       and LM75_I2C_SMBUSALERT_GPIO_CLK Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);

    /*!< Configure BMON_I2C pins: SCL */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /*!< Configure BMON_I2C pins: SDA */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOB, &GPIO_InitStructure);  
    
    I2C_DeInit(I2C1);
    /*!< BMON_I2C Init */
    //I2C_InitStructure.I2C_Mode = I2C_Mode_SMBusHost;
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    //I2C_InitStructure.I2C_Mode = I2C_Mode_SMBusDevice;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 100000; //100kHz
    I2C_Init(I2C1, &I2C_InitStructure);
    
      /*!< Enable SMBus Alert interrupt */
    //I2C_ITConfig(BMON_I2C, I2C_IT_ERR, ENABLE);

    /*!< LM75_I2C Init */
    I2C_Cmd(I2C1, ENABLE);

}

uint8_t BMON_ReadRegister(uint8_t DeviceAddr, uint8_t RegisterAddr, uint8_t* BMON_BufferRX, uint8_t bufferSize)
{
    
    BMON_DMA_Config(BMON_DMA_RX, (uint8_t*)BMON_BufferRX, (uint8_t)bufferSize); /* Configure DMA Peripheral */
    I2C_DMALastTransferCmd(I2C1, ENABLE);                                       /* Enable DMA NACK automatic generation */
    
    if(I2C_GetFlagStatus(I2C1, I2C_FLAG_BERR) || I2C_GetFlagStatus(I2C1, I2C_FLAG_AF)){
        BMON_Init();
    }
    
    I2C_GenerateSTART(I2C1, ENABLE);                                            /* Enable the I2C peripheral */
    BMON_TimeOut = TIMEOUT_MAX;
    while (!I2C_GetFlagStatus(I2C1,I2C_FLAG_SB))                                /* Test on SB Flag */
    {
        if (--BMON_TimeOut == 0) break;
    }
    
    I2C_Send7bitAddress(I2C1, DeviceAddr, I2C_Direction_Transmitter);           /* Send device address for write */
    BMON_TimeOut = TIMEOUT_MAX;
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))   /* Test on ADDR Flag */
    {
        if (--BMON_TimeOut == 0) break;
    }
    
    I2C_SendData(I2C1, RegisterAddr);                                                           /* Send the device's internal address to write to */
    BMON_TimeOut = TIMEOUT_MAX;
    while ((!I2C_GetFlagStatus(I2C1,I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(I2C1,I2C_FLAG_BTF)))  /* Test on TXE FLag (data dent) */
    {
        if (--BMON_TimeOut == 0) break;
    }

    I2C_GenerateSTART(I2C1, ENABLE);                /* Send START condition a second time */
    BMON_TimeOut = TIMEOUT_MAX;
    while (!I2C_GetFlagStatus(I2C1,I2C_FLAG_SB))    /* Test on SB Flag */
    {
        if (--BMON_TimeOut == 0) break;
    }
    
    I2C_Send7bitAddress(I2C1, DeviceAddr, I2C_Direction_Receiver);          /* Send BMON address for read */
    BMON_TimeOut = TIMEOUT_MAX;
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))  /* Test on ADDR Flag */
    {
        if (--BMON_TimeOut == 0) break;
    }
    
    I2C_DMACmd(I2C1,ENABLE);                /* Enable I2C DMA request */
    DMA_Cmd(BMON_DMA_RX_CHANNEL, ENABLE);   /* Enable DMA RX Channel */
    
    DMA_ITConfig(BMON_DMA_RX_CHANNEL, DMA_IT_TC, ENABLE);   // Enables or disables the specified DMAy Channelx interrupts
    
//     /* Wait until DMA Transfer Complete */
//     BMON_TimeOut = 2 * TIMEOUT_MAX;
//     while (!DMA_GetFlagStatus(BMON_DMA_RX_TCFLAG))
//     {
//         if (--BMON_TimeOut == 0) break;
// //         if (--BMON_TimeOut == 0) return(BMON_TimeoutUserCallback());
//     }        
//     
//     
// //     pippo=I2C_ReceiveData(I2C1);    
// //     while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
// //     {

// //     }
//  
//     I2C_GenerateSTOP(I2C1, ENABLE);             /* Send STOP Condition */
//     
//     DMA_Cmd(BMON_DMA_RX_CHANNEL, DISABLE);      /* Disable DMA RX Channel */
//      
//     I2C_DMACmd(I2C1,DISABLE);                   /* Disable I2C DMA request */ 

//     DMA_ClearFlag(BMON_DMA_RX_TCFLAG);          /* Clear DMA RX Transfer Complete Flag */

    /* return a pointer to the BMON_Buffer */
    
    if(BMON_TimeOut == 0){
        BMON_Init();
        BMON_attempt++;
    }
    else{
        BMON_attempt=0;
    }
    
    return (uint8_t) 1 ;

}







static void BMON_DMA_Config(BMON_DMADirection_TypeDef Direction, uint8_t* buffer, uint8_t bufferSize)
{
  DMA_InitTypeDef DMA_InitStructure;
  
  RCC_AHBPeriphClockCmd(BMON_DMA_CLK, ENABLE);
  
  /* Initialize the DMA_PeripheralBaseAddr member */
  DMA_InitStructure.DMA_PeripheralBaseAddr = BMON_I2C_DR;
  /* Initialize the DMA_MemoryBaseAddr member */
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buffer;
   /* Initialize the DMA_PeripheralInc member */
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  /* Initialize the DMA_MemoryInc member */
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  /* Initialize the DMA_PeripheralDataSize member */
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  /* Initialize the DMA_MemoryDataSize member */
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  /* Initialize the DMA_Mode member */
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  /* Initialize the DMA_Priority member */
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  /* Initialize the DMA_M2M member */
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  
  
  /* If using DMA for Reception */
  if (Direction == BMON_DMA_RX)
  {
    /* Initialize the DMA_DIR member */
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    
    /* Initialize the DMA_BufferSize member */
    DMA_InitStructure.DMA_BufferSize = bufferSize;
    
    DMA_DeInit(BMON_DMA_RX_CHANNEL);
    
    DMA_Init(BMON_DMA_RX_CHANNEL, &DMA_InitStructure);
  }
   /* If using DMA for Transmission */
  else if (Direction == BMON_DMA_TX)
  { 
    /* Initialize the DMA_DIR member */
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    
    /* Initialize the DMA_BufferSize member */
    DMA_InitStructure.DMA_BufferSize = 1;
    
    DMA_DeInit(BMON_DMA_TX_CHANNEL);
    
    DMA_Init(BMON_DMA_TX_CHANNEL, &DMA_InitStructure);
  }
  

  NVIC_EnableIRQ(DMA1_Channel7_IRQn);    // enable DMA channel 7 interrupt
  
}




// ----------------------------------------------------------------------------
// DMA IRQ: Executed when a transfer is completed
// ----------------------------------------------------------------------------
void DMA1_Channel7_IRQHandler(void) {
    if(DMA_GetFlagStatus(BMON_DMA_RX_TCFLAG)){
    
        I2C_GenerateSTOP(I2C1, ENABLE);             /* Send STOP Condition */
        DMA_Cmd(BMON_DMA_RX_CHANNEL, DISABLE);      /* Disable DMA RX Channel */
        I2C_DMACmd(I2C1,DISABLE);                   /* Disable I2C DMA request */
        DMA_ClearFlag(BMON_DMA_RX_TCFLAG);          /* Clear DMA RX Transfer Complete Flag */

    }
}
