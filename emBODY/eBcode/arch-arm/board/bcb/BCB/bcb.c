 /****************************************************
 *  BCB board - iCub Facility - 2013                 *
 *  Main program                                     *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/
// ----------------------------------------------------------------------------
// Firmware Version
// ----------------------------------------------------------------------------
char Firmware_vers = 1;
char Revision_vers = 5;
char Build_number  = 0;


// ----------------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------------
#include <stdio.h>
#include <stdbool.h>
#include "STM32F10x.h"
#include "define.h"
#include "globals.h"
#include "gpio.h"
#include "adc.h"
#include "can.h"
#include "exti.h"
#include "timer.h"
#include "usart.h"
#include "bmon.h"

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
bool V12board           = 0;        // DCDC board control
bool V12board_F         = 0;        // fault
bool V12board_bdc       = 0;        // feedback from BDC
bool V12motor           = 0;        // DCDC motor control
bool V12motor_F         = 0;        // fault
bool V12motor_bdc       = 0;        // feedback from BDC
bool HSM                = 0;
bool HSM_PG             = 0;
bool HSM_F              = 0;
bool HSM_bdc            = 0;        // feedback from BDC
bool DCrestart          = 0;
int DCDC_ctrl           = 0;
int DCDC_status         = 0;
int ext_status          = 0;

int filter_PB1          = 0;
int filter_PB2          = 0;
int filter_fault_gpio   = 0;
int timer_debounce      = 10;       // 10x (10ms timer) = 100ms
int timer_restart       = 100;		// 100x (10ms timer) = 1s
int timer_fan           = 0;

char timer_100us        = 0;
char timer_1ms          = 0;
char timer_10ms         = 0;
char timer_100ms        = 0;
char timer_1s           = 0;

char BMON_attempt       = 0;

bool sendBT             = 0;
bool can_led_active     = 0;
bool toggle             = 0;
bool togglex2           = 0;
bool fault              = 1;
bool jack               = 0;

bool PB1_debounce       = 0;
bool PB1_restart        = 0;
bool PB2_debounce       = 0;
bool PB2_restart        = 0;
bool int_fault          = 0;

uint8_t BMON_RX[6] = {0};

unsigned int ledExti=0;

// ----------------------------------------------------------------------------
// Local variables
// ----------------------------------------------------------------------------
int *adc_values;
int adc_val[1];
int VBATT;

char state_fsm = START;
char state_fsm_prec;
char timerFSM = 0;        // timer macchina a stati finiti
char BMON_max_attempts  = 10;
char BMON_filter        = 0;
char BMON_max_filter    = 10;

int cont=0;
int rx=0;
int *rxarray;
int message=0;
char text[17];
unsigned int val_Tx = 0, val_Rx = 0;    // Globals used for display


// ----------------------------------------------------------------------------
// Initialize CAN interface
// ----------------------------------------------------------------------------
void CAN_init(void) {
    CAN_setup();                                    // setup CAN Controller     
    CAN_wrFilter (0x621, STANDARD_FORMAT);          // Enable reception of BDC messages 
    CAN_wrFilter (0x701, STANDARD_FORMAT);          // Enable reception of PC messages 
    CAN_wrFilter (0x70F, STANDARD_FORMAT);          // Enable reception of PC broadcast messages 

    // ! COMMENT LINE BELOW TO ENABLE DEVICE TO PARTICIPATE IN CAN NETWORK !    
    //CAN_testmode(CAN_BTR_SILM | CAN_BTR_LBKM);      // Loopback, Silent Mode (self-test)

    CAN_start ();																// start CAN Controller   
    CAN_waitReady ();														// wait till tx mbx is empty 
}


// ----------------------------------------------------------------------------
// Finite state machine - DCDC power management
// ----------------------------------------------------------------------------
void dcdc_management(void){
	
	if(V12board_F==0 && V12motor_F==0 && HSM_F==0){                                         // if BDC is ok...
		if(PB1_debounce){
			PB1_debounce=0;
			if(V12board)                            {state_fsm = OFF_BOARD_MOTORS;}         // if V12board is on, switch off
			else                                    {state_fsm = ON_BOARD;}                 // else if V12board is off, switch on
		}
		
		if(PB2_debounce){
			PB2_debounce=0;
			if      (V12motor)                      {state_fsm = ON_BOARD;}                 // if V12motor is on, switch off
			else if (!V12motor && V12board)         {state_fsm = ON_BOARD_MOTORS;}          // else if V12motor is off and V12board is on, switch on
		}	
	}
	else{																																			// else if BDC has fault...
		if(V12board_F==0 && (V12motor_F==1 || HSM_F==1)){
			if(PB1_debounce){
				PB1_debounce=0;
				if(V12board)                        {state_fsm = OFF_BOARD_MOTORS;}         // if V12board is on, switch off
				else                                {state_fsm = ON_BOARD;}                 // else if V12board is off, switch on
			}
			else{
				if(PB2_restart==0)                  {state_fsm = FAULT_MOTORS;}
				else{
					if(V12board)                    {state_fsm = ON_BOARD_MOTORS;}          // if V12board is on, switch off
					else                            {state_fsm = OFF_BOARD_MOTORS;}         // else if V12board is off, switch on
				}
			}
		}
		else if(V12board_F==1){
			if(PB1_restart==0){
				if(V12motor_F==1 || HSM_F==1)       {state_fsm = FAULT_BOARD_MOTORS;}
				else                                {state_fsm = FAULT_BOARD;}
			}
			else if(PB1_restart==1){
				state_fsm = ON_BOARD;
				//V12board_F=0;
			}
		}
	}
	
	
	switch(state_fsm){
	
		case START:                 // ----------------------------------------------------------- //
			timerFSM++;
			state_fsm_prec = START;
		
			if(timerFSM >= 10){
				state_fsm = READY;
			}
		break;
		
		case READY:                 // ----------------------------------------------------------- //
			timerFSM = 0;
						
		break;
		
		case OFF_BOARD_MOTORS:		// ----------------------------------------------------------- //
			V12board=0;             // switch off V12board
			V12motor=0;             // switch off V12motor
			HSM=0;                  // switch off HSM
			PB1_LED_RED;
			PB2_LED_RED;
		break;
		
		case ON_BOARD:              // ----------------------------------------------------------- //
			V12board=1;             // switch on V12board
			V12motor=0;             // switch off V12motor
			HSM=0;                  // switch off HSM
            if(V12board_bdc==0){
                if(toggle)  {PB1_LED_GREEN;}
                else        {PB1_LED_OFF;}
			}
            else{
                PB1_LED_GREEN;
            }
			PB2_LED_RED;
		break;
		
		case ON_BOARD_MOTORS:       // ----------------------------------------------------------- //
			V12board=1;             // switch on V12board
			V12motor=1;             // switch on V12motor
			HSM=1;                  // switch on HSM
            if(V12board_bdc==0){
                if(toggle)  {PB1_LED_GREEN;}
                else        {PB1_LED_OFF;}
			}
            else{
                PB1_LED_GREEN;
            }
            if(V12motor_bdc==0 || HSM_PG==0){
                if(toggle)  {PB2_LED_GREEN;}
                else        {PB2_LED_OFF;}
			}
            else{
                PB2_LED_GREEN;
            }
		break;
		
		case FAULT_BOARD:           // ----------------------------------------------------------- //
			V12board=0;             // switch off V12board
			V12motor=0;             // switch off V12motor
			HSM=0;                  // switch off HSM
			if(toggle)  {PB1_LED_RED;}
			else        {PB1_LED_OFF;}
			PB2_LED_RED;
		break;
			
		case FAULT_MOTORS:          // ----------------------------------------------------------- //
			//V12board=1;           // switch on V12board
			V12motor=0;             // switch off V12motor
			HSM=0;                  // switch off HSM
            PB1_LED_GREEN;
			if(toggle)  {PB2_LED_RED;}
			else        {PB2_LED_OFF;}
		break;
			
		case FAULT_BOARD_MOTORS:    // ----------------------------------------------------------- //
			V12board=0;             // switch off V12board
			V12motor=0;             // switch off V12motor
			HSM=0;                  // switch off HSM
			if(toggle)  {PB1_LED_RED;       PB2_LED_RED;}
			else        {PB1_LED_OFF;       PB2_LED_OFF;}
		break;
            
        case FAULT:                 // ----------------------------------------------------------- //
			//V12board=1;             // switch on V12board
			//V12motor=1;             // switch on V12motor
			//HSM=1;                  // switch on HSM
            PB1_LED_GREEN;
			if(toggle)  {PB2_LED_GREEN;}
			else        {PB2_LED_OFF;}
		break;
		
	
	}
	
	
	
    DCDC_ctrl = ((V12board		& 0x01) << 7) +
                ((V12motor		& 0x01)	<< 5) +
                ((HSM           & 0x01)	<< 3) +
                (((PB1_restart || PB2_restart) & 0x01)	<< 0);
    
    //ext_status = 
	
    if(DCrestart){
        PB1_restart=0;
        PB2_restart=0;
    }
}


// ----------------------------------------------------------------------------
// CAN messages management
// ----------------------------------------------------------------------------
void CANBUS(void){   
    
    if (CAN_RxRdy){                                 //rx msg on CAN Ctrl
        
        if(CAN_RxMsg.id == 0x701 || CAN_RxMsg.id == 0x70F){
            
            switch (CAN_RxMsg.data[0]){                             // Control status byte
                    
                case 0xFF:  CAN_TxMsg.id = 0x710;                   // Polling
                            CAN_TxMsg.data[0] = 0xFF;				// Firmware version message
                            CAN_TxMsg.data[1] = 0x01;               // Board type
                            CAN_TxMsg.data[2] = Firmware_vers;      // Firmware version
                            CAN_TxMsg.data[3] = Revision_vers;      // Revision version
                            CAN_TxMsg.data[4] = Build_number;       // Build number
                            CAN_TxMsg.data[5] = 0;
                            CAN_TxMsg.data[6] = 0;
                            CAN_TxMsg.data[7] = 0;
                            while (!CAN_TxRdy); 
                            CAN_TxRdy = 0;
                            CAN_wrMsg (&CAN_TxMsg);   
                            break;
            }
        }
                                        
        else{
            DCDC_status = CAN_RxMsg.data[1];
            
            V12board_bdc = (DCDC_status >> 7 ) & 0x01;   // new
            V12board_F   = (DCDC_status >> 6 ) & 0x01;
            V12motor_bdc = (DCDC_status >> 5 ) & 0x01;   // new
            V12motor_F   = (DCDC_status >> 4 ) & 0x01;
            HSM_bdc      = (DCDC_status >> 3 ) & 0x01;   // new
            HSM_PG       = (DCDC_status >> 2 ) & 0x01;
            HSM_F        = (DCDC_status >> 1 ) & 0x01;
            DCrestart    = (DCDC_status >> 0 ) & 0x01;
        }
        
        CAN_RxRdy = 0;
    }
  
    
    if (CAN_TxRdy) {		
            CAN_TxRdy = 0;
            CAN_TxMsg.id = 0x612;

            if(message==0){
                CAN_TxMsg.data[0] = 0x00;				//
                CAN_TxMsg.data[1] = DCDC_ctrl;
                message=1;
            }
            else if(message==1){
                CAN_TxMsg.data[0] = 0x01;				// Voltage message
                CAN_TxMsg.data[1] = DCDC_ctrl;
                message=2;
            }
            else if (message==2){
                CAN_TxMsg.data[0] = 0x02;				// Current message
                CAN_TxMsg.data[1] = DCDC_ctrl;
                message=0;
            }

            CAN_TxMsg.data[2] = ((VBATT>> 8) & 0xFF);
            CAN_TxMsg.data[3] = VBATT & 0xFF;
            CAN_TxMsg.data[4] = cont & 0xFF;
            CAN_TxMsg.data[5] = BMON_RX[2];
            CAN_TxMsg.data[6] = BMON_RX[3];
            //CAN_TxMsg.data[7] = BMON_RX[2];
            //CAN_TxMsg.data[8] = BMON_RX[3];
            //CAN_TxMsg.data[4] = V12board;
            //CAN_TxMsg.data[5] = V12motor;
            //CAN_TxMsg.data[6] = HSM;
            CAN_wrMsg (&CAN_TxMsg);
    }
    //else {CAN_LED_OFF;}


//     if (CAN_RxRdy){                                 //rx msg on CAN Ctrl
//             CAN_RxRdy = 0;
//             DCDC_status = CAN_RxMsg.data[1];

//             V12board_F  = (DCDC_status >> 6 ) & 0x01;
//             V12motor_F  = (DCDC_status >> 4 ) & 0x01;
//             HSM_PG      = (DCDC_status >> 2 ) & 0x01;
//             HSM_F       = (DCDC_status >> 1 ) & 0x01;
//             DCrestart   = (DCDC_status >> 0 ) & 0x01;
//     }

   
    if(can_led_active>0){                           // blink can led
            if(toggle)  {CAN_LED_ON;}
            else        {CAN_LED_OFF;}
            can_led_active--;
    }
    else	{CAN_LED_OFF;}

}


// ----------------------------------------------------------------------------
// BMON messages management
// ----------------------------------------------------------------------------
void BMON(char type){
    if(type==0){            // Test the i2c communication and read the BMON board    
        if(BMON_attempt < BMON_max_attempts){
            BMON_ReadRegister(0x5A, 0x00, BMON_RX, 6);       // BMON address
        }
    }
    if(type==1){            // Repeat the i2c communication test if it was failed
        if(BMON_attempt >= BMON_max_attempts){
            if(BMON_filter++ > BMON_max_filter){
                BMON_attempt = 0;
                BMON_filter = 0;
            }
        }
    }
}


// ----------------------------------------------------------------------------
// FANs management
// ----------------------------------------------------------------------------
void FAN(void){
    int time_max=1500;
    
    if(state_fsm == ON_BOARD || state_fsm == ON_BOARD_MOTORS){
        FAN_ON;
        timer_fan=0;
    }
    else{
        if(timer_fan < time_max){
            timer_fan++;
        }
        else{
            FAN_OFF;
        }
        
    }
}


// ----------------------------------------------------------------------------
// FAULT check
// ----------------------------------------------------------------------------
void FAULT_CHECK(void){
    if(int_fault){       
        
        if((!(GPIOC->IDR & JACK) == 0)) {FAULT_REM_ON;}     // check for the link of the fault push button
        else                            {FAULT_REM_OFF;}
        
        if( (!(GPIOC->IDR & FAULT_REM) == 0) && (
            ( ((GPIOC->IDR & JACK)  == 0) && (!(GPIOC->IDR & FAULT_GPIO) == 0) ) ||     // there isn't the jack
            ( (!(GPIOC->IDR & JACK) == 0) && (GPIOC->IDR & FAULT_GPIO)  == 0)  )        // there is the jack
            ) {             // NO FAULT
            if(filter_fault_gpio<timer_debounce){             
                filter_fault_gpio++;                    
            }
            else if(filter_fault_gpio==timer_debounce){     // valid command
                //LED_BT_OFF;
                filter_fault_gpio=0;
                int_fault=0;
            }
        }
        else{               // FAULT!!!!
            if(filter_fault_gpio>-timer_debounce){             
                filter_fault_gpio--;                    
            }
            else if(filter_fault_gpio==-timer_debounce){    // valid command
                //LED_BT_ON;
                filter_fault_gpio=0;
                int_fault=0;
            }   
        }
    }
}


// ----------------------------------------------------------------------------
// BLUETOOTH commands
// ----------------------------------------------------------------------------
void BLUETOOTH(void){
    rx=GetKey2();
    //rxarray=GetKey2();
    
    if(rx==0x00){           // Disable Bluetooth data transmission
        sendBT=0;
    }
    else if(rx==0x01){      // Enable Bluetooth data transmission
        sendBT=1;
    }
    else if(rx==0x10){      // V12board power on
        if(state_fsm == READY ||
           state_fsm == OFF_BOARD_MOTORS)    {PB1_debounce=1;}
        
    }
    else if(rx==0x11){      // V12board power off
        if(state_fsm == ON_BOARD ||
           state_fsm == ON_BOARD_MOTORS)    {PB1_debounce=1;}
        
    }
    else if(rx==0x20){      // V12board power on
        if(state_fsm == ON_BOARD)    {PB2_debounce=1;}
    } 
    else if(rx==0x21){      // V12board power on
        if(state_fsm == ON_BOARD_MOTORS)    {PB2_debounce=1;}
    }
    
    else if(rx==0xFF){
        BMON_RX[5] = 0xFF;
        BMON_RX[4] = 0xFF;
        BMON_RX[3] = 0xFF;
        BMON_RX[2] = Firmware_vers;      // Firmware version
        BMON_RX[1] = Revision_vers;      // Revision version
        BMON_RX[0] = Build_number;       // Build number                    
    }
               
    if(sendBT || rx==0xFF){
        SendChar2(0x00);
        SendUart2(BMON_RX, sizeof(BMON_RX));
        SendChar2(DCDC_status);
        SendChar2(0x0D);        // send \r
        SendChar2(0x0A);        // send \n
        //SendChar2(DCDC_ctrl);
    }
}
/*
    START,
    READY,
    ON_BOARD,
    ON_BOARD_MOTORS,
    OFF_BOARD_MOTORS,
    FAULT,
    FAULT_BOARD,
    FAULT_MOTORS,
    FAULT_BOARD_MOTORS
*/




// ----------------------------------------------------------------------------
// MAIN function
// ----------------------------------------------------------------------------
int main (void) {
	int i;
	
    BCB_GPIO_Init();        // Initialize GPIO
    EXTI_Init();            // Initialize interrupts
    TIM1_Init();            // Initialize Timer1
    TIM2_Init();            // Initialize Timer2
    TIM3_Init();            // Initialize Timer3
    TIM4_Init();            // Initialize Timer4
    ADC_Init();             // Initialize ADC
    CAN_init();             // Initialize CAN
    //USART1_Init();        // Initialize USART1
    USART2_Init();          // Initialize USART2
    BMON_Init();            // Initialize I2C for BMON reading 
    BT_ON;                  // Switch on Bluetooth module
    FAULT_REM_OFF;  //FAULT_REM_ON;
	
    CAN_TxMsg.id = 0x612;
    for (i = 0; i < 8; i++) CAN_TxMsg.data[i] = 0;
    CAN_TxMsg.len = 8;
    CAN_TxMsg.format = STANDARD_FORMAT;
    CAN_TxMsg.type = DATA_FRAME;

    PB1_LED_RED;
    PB2_LED_RED;
    FAN_OFF;
    
  while (1) {
        if (timer_100us >= 1){      //----------------------------- 100us -----------------------------
            timer_100us=0;
            timer_1ms++;
        }
		
        if (timer_1ms >= 10){       //----------------------------- 1ms -------------------------------
            timer_1ms=0;
            timer_10ms++;

            ADC_StartCnv();                      // start A/D conversion 12bit
            adc_values = ADC_Conv();
            ADC_StopCnv();                       // stop A/D conversion

            VBATT=(*adc_values)*14.195;          // conversion in mV

            dcdc_management();
            CANBUS();
            BMON(0);
        }
		
		
        if (timer_10ms >= 10){      //----------------------------- 10ms -----------------------------
            timer_10ms=0;
            timer_100ms++;
            
            FAULT_CHECK();
            
            if(sendBT){
                if(toggle)  {LED_BT_ON;}
                else        {LED_BT_OFF;}
            }
            else            {LED_BT_OFF;}
		}
		
		if (timer_100ms >= 10){		//----------------------------- 100ms -----------------------------
			timer_100ms=0;
			timer_1s++;
			
            FAN();
            BLUETOOTH();
            BMON(1);
            
            toggle ^= 1;
            if(toggle)  {togglex2 ^=1;}
		}
		
		if (timer_1s >= 10){		//----------------------------- 1s ---------------------------------
			timer_1s=0;
                       
			cont++;
		}
       
	}
}
