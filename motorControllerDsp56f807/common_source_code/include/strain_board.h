#ifndef __strain_board_h__
#define __strain_board_h__

#include "controller.h"

#define CAN_ID_STRAIN_01		01
#define CAN_ID_STRAIN_02		02
#define CAN_ID_STRAIN_03		03
#define CAN_ID_STRAIN_04		04
#define CAN_ID_STRAIN_05		05
#define CAN_ID_STRAIN_06 		06
#define CAN_ID_STRAIN_07		07
#define CAN_ID_STRAIN_08		08
#define CAN_ID_STRAIN_09		09
#define CAN_ID_STRAIN_10 		10
#define CAN_ID_STRAIN_11		11
#define CAN_ID_STRAIN_12		12
#define CAN_ID_STRAIN_13		13
#define CAN_ID_STRAIN_14 		14

#define STRAIN_MAX             15

extern byte  _selected_strain_id[JN];
extern byte  _selected_strain_chan[JN];

extern Int16 _strain[STRAIN_MAX][6];
extern Int16 _strain_init[STRAIN_MAX][6];
extern Int16 _strain_old[STRAIN_MAX][6];
extern byte  _strain_wtd[STRAIN_MAX];  		//watchdog timer: disables pwm if strain doesn't communicate
#define STRAIN_SAFE 200    					//number of ms for the watchdog timer

void init_strain ();
word read_strain(byte jnt, bool sign);
void start_strain(word can_address);
void stop_strain(word can_address);
bool read_force_data (byte jnt, byte strain_num, byte strain_chan);
#endif