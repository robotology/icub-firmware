/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero, Andrea Merello
 * email:   valentina.gaggero@iit.it, andrea.merello@iit.it
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

/* @file        amplifier.c
    @brief      This file implements interface to use adc AD8555. 
    @author     valentina.gaggero@iit.it, andrea.merello@iit.it
    @date       07/14/2011
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <string.h>
#include <p33fj128mc802.h>
#include <libpic30.h>
#include "hal_timer.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "amplifier.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
//#include "entity_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

// possible parameters for Function field 
#define FNC_CHANGE_SENSE_CURRENT   0x0
#define FNC_SIMULATE_PARAMETER     0x1
#define FNC_PROGRAM_PARAMETER      0x2
#define FNC_READ_PARAMETER         0x3

// possible values for Parameter field 
#define PAR_SECOND_STAGE_GAIN      0x0
#define PAR_FIRST_STAGE_GAIN       0x1
#define PAR_OUTPUT_OFFSET          0x2
#define PAR_OTHER                  0x3

// possible values for user parameter selection  
#define PROGRAM_SECOND_STAGE_GAIN  PAR_SECOND_STAGE_GAIN
#define PROGRAM_FIRST_STAGE_GAIN   PAR_FIRST_STAGE_GAIN
#define PROGRAM_OUTPUT_OFFSET      PAR_OUTPUT_OFFSET

//shortcut to access static structure data
#define CFG_6SG_BEHAV               ampl_data.SIXsg_config_ptr->behaviour_cfg
#define CFG_GEN_EEDATA              ampl_data.SIXsg_config_ptr->gen_ee_data
#define CFG_6SG_EEDATA              ampl_data.SIXsg_config_ptr->SIXsg_ee_data


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

// Variable for accessing the AD8555 configuration register 38 bit long word
// NOTE: fields are in reversed order because we must write MSB first. 
// in this way, accessing the struct byte-wise in the reverse order, things
// works.
typedef union
{
    //human-wise access window
    struct 
    {
      // End of Packet 12 bit word
      unsigned EOP:12;  // has to be 0b011111111110

      // The value for the parameter being changed in "Parameter" field.
      unsigned ParameterVal:8;
      
      // dummy
      unsigned Dummy:2;

      // 2-Bit Parameter
      // 00: Second Stage Gain Code
      // 01: First Stage Gain Code
      // 10: Output Offset Code
      // 11: Other Functions
      unsigned Parameter:2;

     // 2-Bit Function
      // 00: Change Sense Current
      // 01: Simulate Parameter Value
      // 10: Program Parameter Value
      // 11: Read Parameter Value
      unsigned Function:2;

        // Start of Packet 12 bit word
      unsigned SOP:12;  // has to be 0b100000000001

        // 12+2+2+2+8+12 = 38 -> fill to 40bit
      unsigned fill:2;
    } __attribute__((__packed__))  bits;
    
    //byte-wise access window.
    char b[5];
    
}  __attribute__((__packed__)) ampl_register_t; //tAD8555Reg;


// type definition for the internal state machine variables
typedef struct
{
    int16_t state;
    int16_t pulse;
    int16_t running;
} ampl_state_machine_t;


typedef struct
{
    volatile ampl_state_machine_t       st_m;
    SIXsg_config_data_t                 *SIXsg_config_ptr;
    ampl_register_t                     ch_reg_list[AN_CHANNEL_NUM];

} ampl_datastruct_t;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_ampl_gpio_init(void);
static void s_ampl_timer_callback(void *arg);
static int16_t s_ampl_reg_set(int16_t cmd);
static void s_ampl_timer_init(void);
static int16_t s_ampl_is_busy(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//contains data used by amplifier in run time
static ampl_datastruct_t        ampl_data;


// contains some initializations for fields that are supposed
// to be programmed always in this way (magic numbers, etc..)
static const ampl_register_t ch_reg_default_values =
{
  .bits.SOP = 0b100000000001,               // start of frame magic number
  .bits.EOP = 0b011111111110,               // end of frame magic number
  .bits.Function = FNC_SIMULATE_PARAMETER,  // apply the param setting, but do not burn any fuse.
  .bits.Dummy = 0b10,                       // another magic number... (MUST be 0b10)
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern hal_result_t ampl_init(SIXsg_config_data_t *cfg_ptr)
{
    uint8_t i;
    
    if(NULL == cfg_ptr)
    {
        return(hal_res_NOK_nullpointer);
    }
    
    //set poiter to 6SG configuration data
    ampl_data.SIXsg_config_ptr = cfg_ptr;

    s_ampl_gpio_init();
    s_ampl_timer_init();

    
    // init register data struct of each channel with default values
    for(i=0; i<AN_CHANNEL_NUM; i++)
    {
        memcpy(&ampl_data.ch_reg_list[i], &ch_reg_default_values, sizeof(ampl_register_t));
    }
    
    //init state machine 
    ampl_data.st_m.state = 0;
    ampl_data.st_m.pulse = 0;
    ampl_data.st_m.running = 0;
    
    return(hal_res_OK);
}



extern void ampl_set_gain_and_offset(void)
{
  
  // write initial gain 1
  while(s_ampl_is_busy());
  s_ampl_reg_set(PAR_FIRST_STAGE_GAIN);

  // write initial gain 2
  while(s_ampl_is_busy());
  s_ampl_reg_set(PAR_SECOND_STAGE_GAIN);

  // write initial offset
  while(s_ampl_is_busy());
  s_ampl_reg_set(PAR_OUTPUT_OFFSET);
}


extern void ampl_set_offset(void)
{
  // write initial offset
  while(s_ampl_is_busy());
  s_ampl_reg_set(PAR_OUTPUT_OFFSET);
}


extern void ampl_set_gain(void)
{
  
  // write initial gain 1
  while(s_ampl_is_busy());
  s_ampl_reg_set(PAR_FIRST_STAGE_GAIN);

  // write initial gain 2
  while(s_ampl_is_busy());
  s_ampl_reg_set(PAR_SECOND_STAGE_GAIN);

}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static int16_t s_ampl_is_busy(void)
{
    return(ampl_data.st_m.running);
}

static void s_ampl_gpio_init(void)
{
  // PGAs 1-wires interfaces
  TRISBbits.TRISB0 = 0;
  TRISBbits.TRISB1 = 0;
  TRISBbits.TRISB2 = 0;
  TRISBbits.TRISB3 = 0;
  TRISAbits.TRISA0 = 0;
  TRISAbits.TRISA1 = 0;
}

static void s_ampl_timer_init(void)
{
    // Setup timer 3 registers for AD8555 single wire programming

    hal_timer_cfg_t t3_cfg = 
    {
        .prescaler = hal_timer_prescaler1,         
        .countdown = 10,            //tmr3_cfg.match_value = 400;
        .priority = hal_int_priority03,
        .mode = hal_timer_mode_periodic,
        .callback_on_exp = s_ampl_timer_callback,
        .arg = NULL
    };
    
    hal_timer_init(hal_timer3, &t3_cfg, NULL);
    hal_timer_interrupt_enable(hal_timer3);

}


// Wait for 50ns nominal stalling the CPU.
// low pulses should be more than 50ns and less than 10us
// this function waits for 25*4 = 100 ns
inline void s_ampl_wait50ns(void) //AD8555Wait50ns
{
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
}




// this writes the register set on the 1-wite for all 
// the six ad8555 ICs
static int16_t s_ampl_reg_set(int16_t cmd) 
{
    int16_t i;
    
    // If a write was already in progress return with error
    if(ampl_data.st_m.running)
    {
        return(-1);
    }

    // check what command to send
    switch(cmd)
    {
        case PAR_SECOND_STAGE_GAIN:
        {
            // for each channel
            for(i=0; i<AN_CHANNEL_NUM; i++)
            {
                // set the command to be bit banged
                ampl_data.ch_reg_list[i].bits.Parameter = cmd;
                // set the gain value
                ampl_data.ch_reg_list[i].bits.ParameterVal = CFG_6SG_EEDATA.an_channel_gain[i].second_step; 
            }
        } break;

        case PAR_FIRST_STAGE_GAIN:
        {
            // for each channel
            for(i=0; i<AN_CHANNEL_NUM; i++)
            {
                // set the command to be bit banged
                ampl_data.ch_reg_list[i].bits.Parameter = cmd;
                // set the gain value
                ampl_data.ch_reg_list[i].bits.ParameterVal = CFG_6SG_EEDATA.an_channel_gain[i].first_step; 
            }
        } break;

        case PAR_OUTPUT_OFFSET:
        {
            // for each channel
            for(i=0; i<AN_CHANNEL_NUM; i++)
            {
                // set the command to be bit banged
                ampl_data.ch_reg_list[i].bits.Parameter = cmd;
                // set the offs value
                ampl_data.ch_reg_list[i].bits.ParameterVal = CFG_6SG_EEDATA.an_channel_offset[i]; 
            }
        } break;
    }

    // Initialize the state machine
    ampl_data.st_m.state = 0;
    ampl_data.st_m.pulse = 0;
    ampl_data.st_m.running = 1;
    
#warning --> asfidanken: the offset is always in nanoseconds
    // Initialize the timer. Note the timer is DISABLED.
    hal_timer_offset_write(hal_timer3, 0); //WriteTimer3(0);
    
    // Fire a fake timer int (without waiting for anything) 
    IFS0bits.T3IF = 0;
    IFS0bits.T3IF = 1; 
    
    
    // The timer will run, return to caller
    return 0;
}



// this is the "low level" write implementation that
// bangs the six 1-wire channels data bits over the 
// physical MCU GPIO pins.
//
// IMPORTANT: It can be done in 2 ways
// 1- (as it is done now): Read the LATCH register, apply
// modifications (bit-block) and write again back
// 2- modify one bit at a time iterating over all bits.
// Way 1 might produce less ASM code  - TODO: verify -, but it
// needs that this function is called ALWAYS with INTs disabled
// to avoid races.
// Way 2 might produce more ASM code - TODO: verify -, but in
// certain situations ( not always - see below) it may be called
// with INTs enabled if the compiler uses bitwise ASM 
// instruction (likely).
// Now the point is that this function will be called in two
// different places. While in one place using "way 2" make
// it possible to save an INT disable-enable sequence, in the 
// other place we still have to disable ints for other timing
// reasons. At this point it might be better to disable-enable
// INTs two times, but saving some ASM instructions inside those
// sections (using way 1) rather than disable and enable INTs
// only one time at the cost of a slightly longer critical 
// section (using way 2)
inline void s_ampl_write(uint8_t port)//AD8555Write
{

    uint16_t tmp1,tmp2;
    
    //input word BITs:
    // 0: DICH0 (RP0)
    // 1: DICH5 (RP1)
    // 2: DICH2 (RP2)
    // 3: DICH3 (RP3)
    // 4: DICH4 (RA0)
    // 5: DICH1 (RA1)
    
    
    // excange channel 5 with channel 1 
    tmp1 = port & (1<<1);
    tmp2 = port & (1<<5);
    port &= ~((1<<5)|(1<<1));
    
    if(tmp2)
    {
        port |= (1<<1); 
    }
    if(tmp1)
    {
        port |= (1<<5);
    }
    
    // maps the 4 LSB of the data word to the 4 LSB of PORTB 
    LATB = (LATB & 0xFFF0) | (port & 0xf);
    // maps the 2 MSB of the data word to the 2 LSB of PORTA
    LATA = (LATA & 0xFFFC) | ((port & 0x30) >> 4); 
}

// This is the timer 3 interrupt.
// Timer is used to manage 50us "1-bit" pulses and 10us pause times.
// 50ns are handled by stalling the CPU
static void s_ampl_timer_callback(void *arg)
{

    int16_t j;
    uint8_t port;
    int16_t pulse;

    // Disables the timer
    //hal_timer_interrupt_disa(hal_timerT3);  //T3CONbits.TON = 0;  
    hal_timer_stop(hal_timer3);
    // Clears the int flag.
    //IFS0bits.T3IF = 0; lo fa la hal
    // Reset timer counter (be ready to enable again)
#warning --> asfidanken: the offset is always in nanoseconds
    hal_timer_offset_write(hal_timer3, 0); //WriteTimer3(0);

    // was already finished! (to be paranoid)
    if(ampl_data.st_m.pulse == 38)
    {
        return;
    }

    // rising edge (long or short), generate pulse
    if(ampl_data.st_m.state == 0)
    {  
        // defaults all to "not need to be pulled down quickly" (bit 1)
        port = 0x3f;

        // the 1-wire requires field to be bit banged MSB first.
        // the structure also has been inverted in order to make
        // things easyer
        pulse = 37 - ampl_data.st_m.pulse;  
  
        // now search for pins that have to be moved down quickly (bit 0)

        // for each pin..
        for(j=0;j<6;j++)
        {  
            // check if the value required for this pulse is 0
            // if it is 0, then mark the pin as "must be lowered quickly"
            if(0==(ampl_data.ch_reg_list[j].b[pulse/8] & (1<<(pulse%8))))
            {
                port &= ~(1<<j);
            }
        }

        // we have finished to calculate all pins. Nothing has been output yet.
        // raise ALL bits, and quickly lower only the "0-bits"

        // BEGIN CRITICAL SECTION ("0-bits" must be short).
        // disable interrupts
        __builtin_disi(0x3fff);
        // raise all pins
        s_ampl_write(0x3f);
        // wait 50 ns
        s_ampl_wait50ns();
        // lower only the "0-value" bits
        s_ampl_write(port);
        // enable interrupts again
        __builtin_disi(0);
        //  END CRIT SECTION
  
    // we have TXed 0-bits. now we must wait for 1-bits.
    // move to next state
        if(port != 0)
        {
            // some bit is 1: wehave to wait 50us, move to state 1.
            // at the next timer tick (10us) state 1 will be processed
            ampl_data.st_m.state = 1;

        }
        else
        {
            // all is already low. No need to wait anymore.
            // go to next bit
            ampl_data.st_m.pulse++;
            // at the next timer tick (10 us wait time)
            // the next bit will be processed
            ampl_data.st_m.state = 0;
        }
  
    
    }
    else if(ampl_data.st_m.state < 6)// staes 1 to 5: wait 10 us more (to reach 50us total)
    {
        // go to next state. states 1 to 5 will count 10us x 5
        ampl_data.st_m.state++;
        
        // we reached state 6 from transitions 1->2->3->4->5->6
        // this means that 50us wait has been done.
        // NOTE: PHILOSOPY:: it is possible to avoid this chain
        // and do a direct transition 1->6 if the timer is 
        // reprogrammed to 50us (instead of 10). Care must be
        // taken in order to set timer to 10us back.
    }
    else if(ampl_data.st_m.state == 6)
    {
        // lower all the remaining bits. 
        // disable interrupts
        __builtin_disi(0x3fff);
        // perform pin write
        s_ampl_write(0);
        // enable ints again
        __builtin_disi(0);
        
        // next timer tick (after 10us time wait)
        // state 0 (new bit) will be processed
        ampl_data.st_m.state = 0;
        // increment bit counter to process next bit in the word
        ampl_data.st_m.pulse++;
    }
  
    // Check if we finished
    if(ampl_data.st_m.pulse == 38)
    {
        // We TXed 38 bits. All done!
        // mark process as ended and do not 
        // enable this timer again
        ampl_data.st_m.running = 0;
    }
    else
    {
        // some work still to do!
        // enable this timer again!
        //hal_timer_interrupt_ena(hal_timerT3);//T3CONbits.TON = 1;
        hal_timer_start(hal_timer3); 
    }

}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



