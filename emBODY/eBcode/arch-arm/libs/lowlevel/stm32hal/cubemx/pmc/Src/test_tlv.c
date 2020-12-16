
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "gpio.h"

#include "stdio.h"
#include "string.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "test_tlv.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

void discover(void);

typedef enum { x3E = 0x3E, xBC = 0xBC } adr_t;
void set_address(adr_t adr);

void read_no_reg(uint8_t addr, void *pointer, uint8_t capacity);


void discovery(I2C_HandleTypeDef *bus, uint8_t *addresses, uint8_t sizeofaddresses, uint8_t *numof);

void change_address_i2c2();

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

uint8_t foundsI2C1[32] = {0};
uint8_t numI2C1 = 0;
uint8_t foundsI2C2[32] = {0};
uint8_t numI2C2 = 0;
uint8_t foundsI2C3[32] = {0};
uint8_t numI2C3 = 0;
uint8_t foundsI2C4[32] = {0};
uint8_t numI2C4 = 0;

void test_tlv_changeoftheaddress(void)
{   
    change_address_i2c2();    
}

typedef enum { led_01 = 0, led_02 = 1, led_03 = 2, led_04 = 3, led_05 = 4, led_06 = 5, led_07 = 6, led_numberof = 7, led_all = 31 } led_t;
typedef enum {led_off = 0, led_on = 1 } led_status_t;

void set_led(led_t l, led_status_t status)
{
    static const GPIO_PinState ON = GPIO_PIN_RESET;
    static const GPIO_PinState OFF = GPIO_PIN_SET;
    static uint16_t pins[] = { LED1_Pin, LED2_Pin, LED3_Pin, LED4_Pin, LED5_Pin, LED6_Pin, LED7_Pin };  
    static GPIO_TypeDef * ports[] = { LED1_GPIO_Port, LED2_GPIO_Port, LED3_GPIO_Port, LED4_GPIO_Port, LED5_GPIO_Port, LED6_GPIO_Port, LED7_GPIO_Port };    
    
    GPIO_PinState st = (status == led_on) ? ON : OFF;
    
    if(led_all == l)
    {            
        for(uint8_t n=0; n<led_numberof; n++)
        {
            HAL_GPIO_WritePin(ports[n], pins[n], st);
        }
    }
    else if(l < led_numberof)
    {
        HAL_GPIO_WritePin(ports[l], pins[l], st);
    }
    
}


    led_status_t prev[4] = {led_off};
    led_status_t curr[4] = {led_off};
    
    
//#define REMOVE_U27
//#define REMOVE_J13
    
void startup_adr()
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    // i2c1
    // power down j13
    HAL_GPIO_WritePin(GPIOE, MAGVCC1_Pin, GPIO_PIN_RESET);
    HAL_Delay(10); 

#if !defined(REMOVE_J13)     
    // sda-i2c1 PB9 low 
    __HAL_RCC_GPIOB_CLK_ENABLE();    
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL; 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);    
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
    HAL_Delay(10);
    // power up j13
    HAL_GPIO_WritePin(GPIOE, MAGVCC1_Pin, GPIO_PIN_SET);    
    HAL_Delay(10);  
#endif    
    
    // i2c2
    // power down u27
    HAL_GPIO_WritePin(GPIOE, MAGVCC2_Pin, GPIO_PIN_RESET);
    HAL_Delay(10); 
    
#if !defined(REMOVE_U27)        
    // sda-i2c2 PA8 low 
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL; 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);    
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
    HAL_Delay(10);
    // power up u27
    HAL_GPIO_WritePin(GPIOE, MAGVCC2_Pin, GPIO_PIN_SET);    
    HAL_Delay(10);    

#endif

}   


void oldies(void)
{
    
        
    
//    set_address(x3E);   
//    discover();
    
//    set_address(xBC);   
//    discover();    
    
    // ping the chip @ address 0x3E
    
    static const uint8_t tlvadr = 0x3E; // 0xBC
    static const uint32_t timeout = 5;
    
//    status = HAL_I2C_IsDeviceReady(&hi2c2, tlvadr, 3, timeout);
 //   status = status;
    
        
    // reset
    volatile HAL_StatusTypeDef r = HAL_I2C_Master_Transmit(&hi2c2, 0, NULL,0, 5);
    r = r;
    HAL_Delay(5);
    
    
    HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(&hi2c2, tlvadr, 3, timeout);
    status = status;
    
    discover();
}



void setTLVleds(void)
{
    typedef enum { posJ5 = 0, posJ6 = 1, posJ7 = 2, posJ13 = 3, posJ4 = 4, posU27 = 5, posNUM = 6 } posTLV_t;     
    static const led_t ledmap[posNUM] = { led_01, led_02, led_03, led_04, led_05, led_06 };
    
    static uint8_t presence[posNUM] = {0, 0, 0, 0, 0, 0};
    
    memset(presence, 0, sizeof(presence));

    // i2c1: J4 + J13
    for(uint8_t i=0; i<numI2C1; i++)
    {
        if(0xBC == foundsI2C1[i])
        {   // J4
            presence[posJ4] = 1;                               
        }
        else if(0x3E == foundsI2C1[i])
        {   // J13
            presence[posJ13] = 1;               
        }
    }

    // i2c2: J5 + U27
    for(uint8_t i=0; i<numI2C2; i++)
    {
        if(0xBC == foundsI2C2[i])
        {   // J5
            presence[posJ5] = 1;                               
        }
        else if(0x3E == foundsI2C2[i])
        {   // U27
            presence[posU27] = 1;               
        }
    }

    // i2c3: J6
    for(uint8_t i=0; i<numI2C3; i++)
    {
        if(0xBC == foundsI2C3[i])
        {   // J6
            presence[posJ6] = 1;                               
        }
    }  

    // i2c4: J7
    for(uint8_t i=0; i<numI2C4; i++)
    {
        if(0xBC == foundsI2C4[i])
        {   // J7
            presence[posJ7] = 1;                               
        }
    } 
    
    for(uint8_t i=0; i<posNUM; i++)
    {
        set_led(ledmap[i], (1 == presence[i]) ? led_on : led_off);
    }
    
}

void toggle_1hz_led7()
{
    static led_status_t st = led_off;
    static uint32_t prev = 0;
    
    uint32_t now = HAL_GetTick();
    if((now - prev) < 500)
    {
        return;
    }
    
    prev = now;
    
    set_led(led_07, st);
    st = (led_off == st) ? led_on : led_off;
}


void test_presence_of_tlvs(void)
{
    uint64_t count = 0;
    
    for(uint8_t n=0; n<5 ; n++)
    {
        set_led(led_07, led_on);
        HAL_Delay(50);
        set_led(led_07, led_off);
        HAL_Delay(50);
    }
    
    for(;;)
    {  
        HAL_Delay(100);
        
        count ++;
        
        if(count >= 50)
        {
            NVIC_SystemReset();
        }
        
        discovery(&hi2c1, foundsI2C1, sizeof(foundsI2C1), &numI2C1);
        discovery(&hi2c2, foundsI2C2, sizeof(foundsI2C2), &numI2C2);
        discovery(&hi2c3, foundsI2C3, sizeof(foundsI2C3), &numI2C3);
        discovery(&hi2c4, foundsI2C4, sizeof(foundsI2C4), &numI2C4);
        
        setTLVleds();
        toggle_1hz_led7();
    }    
    
}

void test_presence_of_i2cs(void)
{
    
    uint8_t first = 1;
    
    volatile uint8_t changed = 0;
       
 
    for(;;)
    {
        HAL_Delay(100);
        discovery(&hi2c1, foundsI2C1, sizeof(foundsI2C1), &numI2C1);
        discovery(&hi2c2, foundsI2C2, sizeof(foundsI2C2), &numI2C2);
        discovery(&hi2c3, foundsI2C3, sizeof(foundsI2C3), &numI2C3);
        discovery(&hi2c4, foundsI2C4, sizeof(foundsI2C4), &numI2C4);
        
        memcpy(prev, curr, sizeof(curr));
        
        if(numI2C3 == 0)
        {
            changed = 1;
            changed = changed;
        }
        
        curr[0] = (0 == numI2C1) ? led_off : led_on;
        curr[1] = (0 == numI2C2) ? led_off : led_on;
        curr[2] = (0 == numI2C3) ? led_off : led_on;
        curr[3] = (0 == numI2C4) ? led_off : led_on;

        
        set_led(led_04, curr[0]); // J4
        set_led(led_03, curr[1]); // J5
        set_led(led_02, curr[2]); // J6
        set_led(led_01, curr[3]); // J7
        
        if((1 != first) && (0 != memcmp(prev, curr, sizeof(prev))))
        {
            set_led(led_07, led_on);
            HAL_Delay(500);
            set_led(led_07, led_off);  
            HAL_Delay(500);
            set_led(led_07, led_on);  
            HAL_Delay(500);
            set_led(led_07, led_off);             
        }
        
        if(1 == first)
        {
            first = 0;
        }
    }    
    
}
    
void test_tlv_init(void)
{
    volatile HAL_StatusTypeDef status = HAL_ERROR;
    
    MX_GPIO_Init();
        
    set_led(led_all, led_off);
    set_led(led_01, led_on);
    set_led(led_02, led_on);
    set_led(led_03, led_on);
    set_led(led_04, led_on);
    set_led(led_05, led_on);
    set_led(led_06, led_on);
    set_led(led_07, led_on);
    set_led(led_all, led_off);
       
    startup_adr();



    
    // init dma + i2c
    
    MX_DMA_Init();
    MX_I2C1_Init(); 
    MX_I2C2_Init(); 
    MX_I2C3_Init();
    MX_I2C4_Init();    
    HAL_Delay(20);
    
    //test_presence_of_i2cs();
    test_presence_of_tlvs();
    
    for(;;);       
    
}

void test_tlv_ping(void)
{
    
}

uint8_t rxdata[20] = {0};

void test_tlv_config(void)
{
    memset(rxdata, 0, sizeof(rxdata));
    read_no_reg(0x3E, rxdata, 10);
    for(;;);    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


void set_address(adr_t adr)
{
    // power off, power on
    // send 2 or 0
    HAL_GPIO_WritePin(GPIOE, MAGVCC2_Pin|MAGVCC1_Pin, GPIO_PIN_RESET);
    HAL_Delay(5);
    HAL_GPIO_WritePin(GPIOE, MAGVCC2_Pin|MAGVCC1_Pin, GPIO_PIN_SET);
    HAL_Delay(5);

    HAL_I2C_IsDeviceReady(&hi2c2, (x3E == adr) ? 0 : 4, 1, 5);    
}

uint8_t map[256] = {0};
volatile uint8_t found = 0;
uint8_t addresses[16] = {0};
void discover(void)
{
    memset(addresses, 0, sizeof(addresses));
    memset(map, 0, sizeof(map));
    found = 0;
    
    static const uint8_t start = 2;
    for(int i=start; i<256; i++)
    {
        //map[i] = (HAL_OK == HAL_I2C_IsDeviceReady(&hi2c2, i, 1, 5)) ? 1 : 0;
        if(HAL_OK == HAL_I2C_IsDeviceReady(&hi2c2, i, 1, 5))
        {
            map[i] = 1;
            if(found < sizeof(addresses))
            {
                addresses[found] = i;
            }
            found++;
        }
    }
    
    // se start = 0 oppure 1, allora indirizzo  = 3E / 3F
    // altrimenti se start >= allora = BC / BD
    
    found = found;
}

void read_no_reg(uint8_t addr, void *pointer, uint8_t capacity)
{
    
    volatile HAL_StatusTypeDef r = HAL_I2C_Master_Receive_DMA( &hi2c2, 
                                        addr, 
                                        pointer,  
                                        capacity
                                    );  
    r = r;
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{   
    static volatile uint32_t tx = 0;
    tx++;  
    
}


void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
    static volatile uint32_t rx = 0;
    rx++;       
} 

void discovery(I2C_HandleTypeDef *bus, uint8_t *addrs, uint8_t sizeofaddrs, uint8_t *numof)
{
    
    memset(addrs, 0xff, sizeofaddrs);
    
    uint8_t ff = 0;
    *numof = 0;
    
    static const uint8_t start = 2;
    for(int i=start; i<256; i++)
    {
        if((i%2) == 1)
        {
            continue;
        }
        if(HAL_OK == HAL_I2C_IsDeviceReady(bus, i, 1, 5))
        {
            if(ff < sizeofaddrs)
            {
                addrs[ff] = i;
            }
            ff++;
            (*numof)++;
        }
    }    
    
    
    
}
typedef enum { dev_j4 = 0, dev_j13 = 1, dev_j5 = 2, dev_u27 = 3 } dev_t;

void address_default(dev_t dev)
{
    
    I2C_HandleTypeDef *bus = NULL;
    
    if(dev_u27 == dev)
    {
        bus = &hi2c2;   
        
        // now i switch device on.
        MX_GPIO_Init();   

        
        // init i2c2
        MX_DMA_Init();
        MX_I2C2_Init(); 
        HAL_Delay(20); 
        

        
        HAL_GPIO_WritePin(GPIOE, MAGVCC2_Pin, GPIO_PIN_RESET);
        HAL_Delay(20);
        HAL_GPIO_WritePin(GPIOE, MAGVCC2_Pin, GPIO_PIN_SET);
        HAL_Delay(20);   

        // now i discover on i2c2
        discovery(&hi2c2, foundsI2C2, sizeof(foundsI2C2), &numI2C2);
        // now i reset the chip by sending zero
        HAL_I2C_Master_Transmit(bus, 0, NULL, 0, 3);
        HAL_Delay(20);
        // now i discover again: i expect the default address 0xBC, but i read 0x3E
        discovery(&hi2c2, foundsI2C2, sizeof(foundsI2C2), &numI2C2);
    }
    
    for(;;);
            
}

void address_other(dev_t dev)
{
    
    I2C_HandleTypeDef *bus = NULL;
    
    if(dev_u27 == dev)
    {
        bus = &hi2c2;   
        
        // init i2c2
//        MX_DMA_Init();
//        MX_I2C2_Init(); 
//        HAL_Delay(20); 
        
        // chip off
        MX_GPIO_Init();        
        HAL_GPIO_WritePin(GPIOE, MAGVCC2_Pin, GPIO_PIN_RESET);
        HAL_Delay(20);
        
        // i config gpio (I2C2_SDA) sul port PB9 (PA8) come GPIO Out Open-Drain forzando lo stato LOW in uscita.
        __HAL_RCC_GPIOA_CLK_ENABLE();
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin = GPIO_PIN_8;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
        GPIO_InitStruct.Pull = GPIO_NOPULL; //GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        
//        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
        HAL_Delay(20);

        // now i put MAGVCC2 high
        HAL_GPIO_WritePin(GPIOE, MAGVCC2_Pin, GPIO_PIN_SET);
        HAL_Delay(20);
        
        // init i2c2
        MX_DMA_Init();
        MX_I2C2_Init(); 
        HAL_Delay(20);         

        // now i discover on i2c2. i expect 0x3e
        discovery(&hi2c2, foundsI2C2, sizeof(foundsI2C2), &numI2C2);

    }
    
    for(;;);
            
}

void change_address_i2c2()
{
    // 1. power off u27. chip on j5 gets address 0xBC
    // 2. put sda pin low
    // 3. power on u27
    // 4. discover i2c2
    
  
    HAL_Delay(20);
    MX_GPIO_Init();

    // power down u27
    HAL_GPIO_WritePin(GPIOE, MAGVCC2_Pin, GPIO_PIN_RESET);
    HAL_Delay(5); 
    
    // sda PA8 low 
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL; 
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);    
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
    HAL_Delay(10);
    // power down and up u27
//    HAL_GPIO_WritePin(GPIOE, MAGVCC2_Pin, GPIO_PIN_RESET);
//    HAL_Delay(5); 
    HAL_GPIO_WritePin(GPIOE, MAGVCC2_Pin, GPIO_PIN_SET);    
    HAL_Delay(10);

    // init i2c2
    MX_DMA_Init();
    MX_I2C2_Init(); 
    HAL_Delay(20);
    // and discover
    discovery(&hi2c2, foundsI2C2, sizeof(foundsI2C2), &numI2C2);
    for(;;);
    
    // 2. set sda 
//    address_other(dev_u27);
//    address_default(dev_u27);
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




