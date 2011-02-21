#include <uart.h>
#include "serial_interface.h"
#include "options.h"

/// Serial 
static int COMlTxBufferIndex;
static int COMlRxBufferIndex;
static int testcounter;
extern main();

extern unsigned int _board_ID;
extern unsigned char board_MODE;
unsigned int board_TIMER;
extern unsigned int PW_CONTROL; // 0x1B0 for 120 decim
extern unsigned int TIMER_VALUE;
extern unsigned int CONFIG_TYPE;
extern char _additional_info[32];
extern unsigned int ConValue[2];


void Serial_Init()
{
}


