 /****************************************************
 *  BDC board - iCub Facility - 2013                 *
 *  Main program                                     *
 *                                                   *
 *  Written by Andrea Mura                           *
 *  <andrea.mura@iit.it>                             *
 ****************************************************/
// ----------------------------------------------------------------------------
// Firmware Version
// ----------------------------------------------------------------------------
char Firmware_vers = 1;
char Revision_vers = 4;
char Build_number  = 0;

// ----------------------------------------------------------------------------
// Include files
// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "STM32F10x.h"
#include "define.h"
#include "globals.h"
#include "gpio.h"
#include "adc.h"
#include "can.h"
#include "exti.h"
#include "timer.h"
#include "hl_fifo.h"

typedef struct 
{
    uint32_t V_VBATT;
    uint32_t V_V12board;
    uint32_t V_V12motor;
    int32_t I_V12board;
    int32_t I_V12motor;
    int32_t I_HSM;
}adc_measure_t;     // definisco il tipo adc_measure



// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
adc_measure_t adc_measure = {0};    // initialize all adc values to 0
adc_measure_t mean = {0};           // initialize all average values to 0
hl_result_t res;
int nr_adc_sample = 40;             // number of samples for the average measure

int DCDC_status_A     = 0;
int DCDC_status_B     = 0;
int DCDC_ctrl       = 0;
bool HSM_PG         = 0;
bool HSM_F          = 0;

char timer_100us    = 0;
char timer_1ms      = 0;
char timer_10ms     = 0;
char timer_100ms    = 0;

bool can_led_active = 0;
bool toggle_timer   = 0;

// ----------------------------------------------------------------------------
// Local variables
// ----------------------------------------------------------------------------
int16_t  *adc_values;									// contains all ADC channels conversion
int V_VBATT_MAX         = 0;
int V_VBATT_MIN         = 0;
int I_V12board_MAX      = 6000;     // threshold in mA
int I_V12motor_MAX      = 10000;    // threshold in mA
int I_HSM_MAX           = 0;
int timer_fault_board   = 0;
int timer_fault_motors  = 0;
int timer_mosfet_broken = 0;
int timer_rearmHSM      = 0;
int rearmHSM_timevalue  = 20000;    // (value in second) = rearmHSM_timevalue * (loop in ms where is "timer_rearmHSM++")

bool V12board           = 0;		// DCDC board control
bool V12board_F         = 0;		// fault
bool V12motor           = 0;		// DCDC motor control
bool V12motor_F         = 0;		// fault
bool HSM_broken         = 0;		// HSM transistors broken
bool HSM                = 0;		// HSM control
bool DCrestart          = 0;

char state_fsm          = START;
char state_fsm_prev;
char timerFSM           = 0;		// timer macchina a stati finiti


// ----------------------------------------------------------------------------
// Initialize CAN interface
// ----------------------------------------------------------------------------
void CAN_init (void) {
    CAN_setup();																// setup CAN Controller     
    //CAN_wrFilter (0x602, STANDARD_FORMAT);			// Enable reception of PC messages 
    CAN_wrFilter (0x612, STANDARD_FORMAT);			// Enable reception of BCB messages 
    CAN_wrFilter (0x702, STANDARD_FORMAT);          // Enable reception of PC messages 
    CAN_wrFilter (0x70F, STANDARD_FORMAT);          // Enable reception of PC broadcast messages 
    
    // ! COMMENT LINE BELOW TO ENABLE DEVICE TO PARTICIPATE IN CAN NETWORK !    
    //CAN_testmode(CAN_BTR_SILM | CAN_BTR_LBKM);      // Loopback, Silent Mode (self-test)

    CAN_start ();																// start CAN Controller   
    CAN_waitReady ();														// wait till tx mbx is empty 
}






// ----------------------------------------------------------------------------
// Finite state machine
// ----------------------------------------------------------------------------
void dcdc_management(void) {
	
//      V12board_F  V12motor_F  HSM_F   V12board    V12motor    HSM DCrestart   STATE
//      ------------------------------------------------------------------------------------------------------------
//         x            x       x           x           x       x       1       START
//         1            0       0           x           x       x       0       FAULT_BOARD
//         1            1       1           x           x       x       0       FAULT_BOARD_MOTORS
//         0            0       0           0           0       0       0       OFF_BOARD_MOTORS
//         0            1       x           1           x       x       0       FAULT_MOTORS
//         0            x       1           1           x       x       0       FAULT_MOTORS
//         0            0       0           1           0       0       0       ON_BOARD
//         0            0       0           1           1       1       0       ON_BOARD_MOTORS




    if(DCrestart==1)                                {state_fsm = START;}
    else{
        if(V12board_F==1){
            if(V12motor_F==0 && HSM_F==0)           {state_fsm = FAULT_BOARD;}
            else                                    {state_fsm = FAULT_BOARD_MOTORS;}
        }
            else{
                if(V12board==0)                     {state_fsm = OFF_BOARD_MOTORS;}
                else{
                    if(V12motor_F==1 || HSM_F==1)   {state_fsm = FAULT_MOTORS;}
                    else{
                        if(V12motor==1 && HSM==1 && timer_rearmHSM==rearmHSM_timevalue)     {state_fsm = ON_BOARD_MOTORS;}
                        else                                                                {state_fsm = ON_BOARD;}
                    }
                }
            }
    }

    
    if(state_fsm_prev==ON_BOARD_MOTORS && state_fsm!=ON_BOARD_MOTORS)   {timer_rearmHSM=0;} // Con queste 3 righe si crea il timer di riarmo
    if(timer_rearmHSM < rearmHSM_timevalue)                             {timer_rearmHSM++;} // della tensione ai motori. Il timer riparte da
    state_fsm_prev=state_fsm;                                                               // quando i motori si spengono per qualsiasi motivo
    
    switch(state_fsm){
		
        case START:                 // ----------------------------------------------------------- //
            V12board_F=0;
            V12motor_F=0;
            HSM_F=0;
            if((GPIOB->IDR & HSM_nFLT)!=0)  {V48motor_FLT_OFF;}
        break;
                
        case OFF_BOARD_MOTORS:      // ----------------------------------------------------------- //
            EN_V12board_OFF;                            // switch off V12board
            EN_V12motor_OFF;                            // switch off V12motor
            HSM_EN_OFF;                                 // switch off HSM
        break;

        case ON_BOARD:              // ----------------------------------------------------------- //
            EN_V12board_ON;                             // switch on V12board
            EN_V12motor_OFF;                            // switch off V12motor
            HSM_EN_OFF;                                 // switch off HSM
        break;

        case ON_BOARD_MOTORS:       // ----------------------------------------------------------- //        
            EN_V12board_ON;                             // switch on V12board
            EN_V12motor_ON;                             // switch on V12motor
            HSM_EN_ON;                                  // switch on HSM
        break;

        case FAULT_BOARD:           // ----------------------------------------------------------- //				
            EN_V12board_OFF;                            // switch off V12board
            EN_V12motor_OFF;                            // switch off V12motor
            HSM_EN_OFF;                                 // switch off HSM
            V12board=0;
            V12motor=0;
            HSM=0;
            V12board_F=1;
            timer_fault_board=0;
        break;

        case FAULT_MOTORS:          // ----------------------------------------------------------- //
            EN_V12board_ON;                             // switch on V12board
            EN_V12motor_OFF;                            // switch off V12motor
            HSM_EN_OFF;                                 // switch off HSM
            V12motor=0;
            V12motor_F=1;
            timer_fault_motors=0;
        break;
        		
	}
	
    DCDC_status_A = ((V12board      & 0x01) << 7) +
                    ((V12board_F    & 0x01) << 6) +
                    ((V12motor      & 0x01) << 5) +
                    ((V12motor_F    & 0x01) << 4) +
                    ((HSM           & 0x01) << 3) +
                    ((HSM_PG        & 0x01) << 2) +
                    ((HSM_F         & 0x01) << 1) +
                    ((DCrestart     & 0x01) << 0);
    
    DCDC_status_B = ((HSM_broken    & 0x01) << 7);
}


// ----------------------------------------------------------------------------
// CAN messages management
// ----------------------------------------------------------------------------
void CANBUS(void){
	if (CAN_RxRdy){                                                 // rx msg on CAN Ctrl

        if (CAN_TxRdy) {
			
            if(CAN_RxMsg.id == 0x702 || CAN_RxMsg.id == 0x70F){
            
                switch (CAN_RxMsg.data[0]){                             // Control status byte
                        
                    case 0xFF:  CAN_TxMsg.id = 0x720;                   // Polling
                                CAN_TxMsg.data[0] = 0xFF;				// Firmware version message
                                CAN_TxMsg.data[1] = 0x02;               // Board type
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
                
                switch (CAN_RxMsg.data[0]){                             // Control status byte
                        
                    case 0x00:  CAN_TxMsg.id = 0x621;                   // Polling
                                CAN_TxMsg.data[0] = 0x80;
                                CAN_TxMsg.data[1] = DCDC_status_A;
                                CAN_TxMsg.data[2] = DCDC_status_B;
                                CAN_TxMsg.data[3] = 0;
                                CAN_TxMsg.data[4] = 0;
                                CAN_TxMsg.data[5] = 0;
                                CAN_TxMsg.data[6] = 0;
                                CAN_TxMsg.data[7] = 0;
                                CAN_wrMsg (&CAN_TxMsg);
                                break;
                        
                    case 0x01:  CAN_TxMsg.id = 0x621;                   // Polling
                                CAN_TxMsg.data[0] = 0x81;               // Voltage message
                                CAN_TxMsg.data[1] = DCDC_status_A;
                                CAN_TxMsg.data[2] = ((mean.V_VBATT>> 8) & 0xFF);
                                CAN_TxMsg.data[3] = mean.V_VBATT & 0xFF;
                                CAN_TxMsg.data[4] = ((mean.V_V12board>> 8) & 0xFF);
                                CAN_TxMsg.data[5] = mean.V_V12board & 0xFF;
                                CAN_TxMsg.data[6] = ((mean.V_V12motor>> 8) & 0xFF);
                                CAN_TxMsg.data[7] = mean.V_V12motor & 0xFF;
                                CAN_wrMsg (&CAN_TxMsg);
                                break;
                        
                    case 0x02:  CAN_TxMsg.id = 0x621;
                                CAN_TxMsg.data[0] = 0x82;               // Current message
                                CAN_TxMsg.data[1] = DCDC_status_A;
                                CAN_TxMsg.data[2] = ((mean.I_V12board>> 8) & 0xFF);
                                CAN_TxMsg.data[3] = mean.I_V12board & 0xFF;
                                CAN_TxMsg.data[4] = ((mean.I_V12motor>> 8) & 0xFF);
                                CAN_TxMsg.data[5] = mean.I_V12motor & 0xFF;
                                CAN_TxMsg.data[6] = ((mean.I_HSM>> 8) & 0xFF);
                                CAN_TxMsg.data[7] = mean.I_HSM & 0xFF;
                                CAN_wrMsg (&CAN_TxMsg);
                                break;
                                            
                    default:    CAN_TxMsg.id = 0x621;
                                CAN_TxMsg.data[0] = 0xAA;               // Voltage message
                                CAN_TxMsg.data[1] = CAN_RxMsg.data[0];
                                CAN_TxMsg.data[2] = CAN_RxMsg.data[1];
                                CAN_TxMsg.data[3] = CAN_RxMsg.data[2];
                                CAN_TxMsg.data[4] = CAN_RxMsg.data[3];
                                CAN_TxMsg.data[5] = CAN_RxMsg.data[4];
                                CAN_TxMsg.data[6] = CAN_RxMsg.data[5];
                                CAN_TxMsg.data[7] = CAN_RxMsg.data[6];
                                CAN_wrMsg (&CAN_TxMsg);
                                break;
                }
            }
        }

        DCDC_ctrl   = CAN_RxMsg.data[1];
        V12board    = (DCDC_ctrl >> 7) & 0x01;
        V12motor    = (DCDC_ctrl >> 5) & 0x01;
        HSM         = (DCDC_ctrl >> 3) & 0x01;
        DCrestart   = (DCDC_ctrl >> 0) & 0x01;

        CAN_RxRdy = 0;
				
    }
}


// ----------------------------------------------------------------------------
// Analog measurement
// ----------------------------------------------------------------------------
void analog_measure(void){
	ADC_StartCnv();                     // start A/D conversion 12bit
	adc_values	= ADC_Conv();           // save the conversion results 
	ADC_StopCnv();                      // stop A/D conversion
	
    
    adc_measure.V_VBATT		= (*(adc_values))*13.26;       	// mV    //14.195
 	adc_measure.V_V12board	= (*(adc_values+1))*3.72;       // mV    //3.956
 	adc_measure.V_V12motor	= (*(adc_values+2))*3.72;       // mV    //3.956
    adc_measure.I_V12board	= ((*(adc_values+3))-2048)*9.5;
	adc_measure.I_V12motor	= ((*(adc_values+4))-2048)*9.5;
	adc_measure.I_HSM		= (*(adc_values+5))*5.8;
 	
    if(adc_measure.I_V12board > I_V12board_MAX){                    // Check for board overcurrent
        if(timer_fault_board < 10)  {timer_fault_board++;}
        else                        {V12board_F = 1;}
    }else{
        if(timer_fault_board > 0)   {timer_fault_board--;}
    }
        
    if(adc_measure.I_V12motor > I_V12motor_MAX){                    // Check for motors overcurrent
        if(timer_fault_motors < 10) {timer_fault_motors++;}
        else                        {V12motor_F = 1;}
    }else{
        if(timer_fault_motors > 0)  {timer_fault_motors--;}
    }
}


// ----------------------------------------------------------------------------
// Average values of adc output channels (filtering)
// ----------------------------------------------------------------------------
void calcMean(hl_fifo_t *fifo_ptr, adc_measure_t *mean)
{
    int i;
    uint8_t *miodata_ptr = fifo_ptr->data;                  // 
    adc_measure_t *mioptr = (adc_measure_t *)miodata_ptr;   // casting al tipo adc_measure

    adc_measure_t sum ={0};

    for(i=0; i<nr_adc_sample; i++){
        sum.V_VBATT     += mioptr[i].V_VBATT;
        sum.V_V12board  += mioptr[i].V_V12board;
        sum.V_V12motor  += mioptr[i].V_V12motor;
        sum.I_V12board  += mioptr[i].I_V12board;
        sum.I_V12motor  += mioptr[i].I_V12motor;
        sum.I_HSM       += mioptr[i].I_HSM;
    }
    
    mean->V_VBATT       = (sum.V_VBATT    /nr_adc_sample);//*14.195;
    mean->V_V12board    = (sum.V_V12board /nr_adc_sample);//*3.956;
    mean->V_V12motor    = (sum.V_V12motor /nr_adc_sample);//*3.956;
    mean->I_V12board    = (sum.I_V12board /nr_adc_sample);//-2048)*10;
    mean->I_V12motor    = (sum.I_V12motor /nr_adc_sample);//-2048)*10;
    mean->I_HSM         = (sum.I_HSM /nr_adc_sample);
}


// ----------------------------------------------------------------------------
// Check for HSM mosfet broken
// ----------------------------------------------------------------------------
void checkHSM(){
    if(HSM==0 && HSM_PG==1){                                        // If HSM Power Good asserted but no command -> probably MOSFET broken
        if(timer_mosfet_broken < 20)    {timer_mosfet_broken++;}    // filtering with timeout....
        else{                                                       // timeout finished -> send warning and toggle leds (red & green)!
            HSM_broken = 1;
            if(toggle_timer)    {V48motor_PG_ON;    V48motor_FLT_ON;}
            else                {V48motor_PG_OFF;   V48motor_FLT_OFF;}
        }
    }else{
        if(timer_mosfet_broken > 1)     {timer_mosfet_broken--;}    // filtering with timeout....
        else if(timer_mosfet_broken == 1){                          // last filtering
            timer_mosfet_broken--;
            if((GPIOB->IDR & HSM_nFLT)==0)  {V48motor_FLT_ON;}      // check for fault and power good pins and change leds status
            else                            {V48motor_FLT_OFF;}     //
            EXTI2_IRQHandler();                                     //
        }
        else                            {HSM_broken = 0;}           // remove warning
    }
}



// ----------------------------------------------------------------------------
// MAIN function
// ----------------------------------------------------------------------------
int main (void) {
    int i;
    hl_fifo_t* fifo_ptr = NULL;
    
    GPIO_Init();
    EXTI_Init();
    ADC_Init();
    TIM1_Init();
    TIM2_Init();
    //USART2_Init();
    CAN_init();

    CAN_TxMsg.id = 0x621;
    for (i = 0; i < 8; i++) CAN_TxMsg.data[i] = 0x0;
    CAN_TxMsg.len = 8;
    CAN_TxMsg.format = STANDARD_FORMAT;
    CAN_TxMsg.type = DATA_FRAME;


    fifo_ptr =  hl_fifo_new(nr_adc_sample, sizeof(adc_measure_t), NULL);  
    hl_fifo_clear(fifo_ptr);

    while (1) {
        if (timer_100us >= 1){	//----------------------------- 100us -----------------------------
            timer_100us=0;
            timer_1ms++;
            
        }

        if (timer_1ms >= 10){		//----------------------------- 1ms -----------------------------
            timer_1ms=0;
            timer_10ms++;

            analog_measure();
            
            if(fifo_ptr->size < fifo_ptr->capacity) {                   // riempe la fifo
                res = hl_fifo_put(fifo_ptr, (uint8_t*)&adc_measure);
            }
            else{                                                       // fifo piena, calcolo media
                hl_fifo_pop(fifo_ptr);
                res = hl_fifo_put(fifo_ptr, (uint8_t*)&adc_measure);
                calcMean(fifo_ptr, &mean);
            }
                            
            dcdc_management();

            CANBUS();
        }

        if (timer_10ms >= 10){		//----------------------------- 10ms -----------------------------
            timer_10ms=0;
            timer_100ms++;
            
        }

        if (timer_100ms >= 10){		//----------------------------- 100ms -----------------------------
            timer_100ms=0;
            
            checkHSM();
            
            if(can_led_active>0){									// blink can led
                if(toggle_timer)    {CAN_LED_ON;}
                else                {CAN_LED_OFF;}
                can_led_active--;
            }
            else    {CAN_LED_OFF;}

        }
		
	}
}

