//
// Hall Effect sensor related routines
//

#include "DHES.h"


// the HES activation sequence is 1,3,2,6,4,5

// note: zero is init val for "old" dhes value
// this LUT gives 0 if no turn has been done
// 1 if one positive turn has to be counted
// -1 if one negative turn has to be counted
// -2 if an invalid transition happened

                                 //old new
signed char DHES_TRANSITION_LUT[7][7] = {
 
 //to 0  1  2  3  4  5  6 
	{ 0, 0, 0, 0, 0, 0, 0}, // from 0 all ok (init val)
	{-2, 0,-2, 0,-2,-1,-2}, // from 1 (ok 1 3 5)
	{-2,-2, 0, 0,-2,-2, 0}, // from 2 (ok 2 3 6)
	{-2, 0, 0, 0,-2,-2,-2}, // from 3 (ok 3 1 2)
	{-2,-2,-2,-2, 0, 0, 0}, // from 4 (ok 4 6 5)
	{-2, 1,-2,-2, 0, 0,-2}, // from 5 (ok 5 1 4)
	{-2,-2, 0,-2, 0,-2, 0}, // from 6 (ok 6 2 4)
};

unsigned int DHES_UNALIGNEDMECANGLE_LUT[7] = {
  0,
  0x0000U/ELECTRICCYCLESPERENCODERCYCLE, // 0°
  0x5554U/ELECTRICCYCLESPERENCODERCYCLE, // 120°
  0x2AAAU/ELECTRICCYCLESPERENCODERCYCLE, // 60°
  0xAAA8U/ELECTRICCYCLESPERENCODERCYCLE, // 240°
  0xD552U/ELECTRICCYCLESPERENCODERCYCLE, // 300°
  0x7FFFU/ELECTRICCYCLESPERENCODERCYCLE, // 180°
};

extern unsigned int hes_antibump_engaged;

static SFRAC16 hes_val = 0;
static int hes_round = 0; // ELECTRICAL hes_round conter inside a MECHANICAL hes_round

int g_hes_state = 0;

void DHESInit()
{
}

SFRAC16 DHESRead()
{
  unsigned char port_read;

  #ifdef DHES_CONNECTED_TO_P1_P6
  // HU==RP3==RB3, HV==RP8==RB8, HW==RP9==RB9
  port_read = (PORTBbits.RB3) | (PORTBbits.RB8<<2) | (PORTBbits.RB9<<1);
  #else
  // Read DHES vaue from the uC pins of choice
  // mask the port where the DHES are connected (RP5, RP6, RP7)
  port_read = (PORTB & 0b011100000) >> 5;
  #warning if_DHES_signal_is_OC_then_check_2FOC_board_PUPs_on_P2_and_P5
  #endif

  // NOTE: from the DHES you get an Electrical angle! 

  if((port_read == 0) || (port_read == 7))
  { 
     SysError.DHESInvalidValue=1;
     g_hes_state = 0; // next time do not make assumption
     FaultConditionsHandler();
     return hes_val;
  }

  #ifndef SIX_STEP

  if (g_hes_state == port_read) return hes_val;

  // if 0 no turn has been done
  // if 1 or -1 a CW or CCW turn has been done
  // if -2 error: invalid transition
  signed char transition = DHES_TRANSITION_LUT[g_hes_state][port_read];
               
  if(-2 == transition)
  {
    SysError.DHESInvalidSequence = 1;
    g_hes_state = 0; // next time do not make assumption
    FaultConditionsHandler();
    return hes_val;
  }

  hes_round += transition;

  hes_val = DHES_UNALIGNEDMECANGLE_LUT[port_read];

  // hes_round += DDHES_TURN_LUT[hes_state][encoder_buffer];

  if(hes_round == ELECTRICCYCLESPERENCODERCYCLE)
  {
    hes_round = 0;
  }
  else 
  if(hes_round == -1)
  {
    hes_round = ELECTRICCYCLESPERENCODERCYCLE-1;
  }

  // combine the electrical angle information with the 
  // electrical turn count to get mechanical angle
  hes_val += hes_round * (0xFFFFU/ELECTRICCYCLESPERENCODERCYCLE);
  #endif

  g_hes_state = port_read;

  return hes_val;
}
