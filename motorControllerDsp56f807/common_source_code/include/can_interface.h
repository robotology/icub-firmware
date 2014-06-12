
#ifndef __can_interfaceh__
#define __can_interfaceh__
#include "iCubCanProtocol.h"

/******************************************************/
// can interface prototypes 
/******************************************************/

void can_interface_init (byte channels_num);
byte can_interface (void);
void can_send_broadcast(void);
void can_send_broadcast_identification(byte j);
void can_send_broadcast_debug(byte j, int rate);
void set_can_masks();
void create_F_M(UInt32 *filter,UInt32 *mask,byte _class1, byte _source1, byte _dest1, byte _class2, byte _source2, byte _dest2);
void setmask(UInt32 filter1, UInt32 filter2, UInt32 mask1,UInt32 mask2);
void can_send_additional_info();
void can_receive_additional_info();


#define MAIS_CAN_ID                 0xE 
#define MAIS_8bit_D_MSG             0x3ED //8 bits value
#define MAIS_8bit_C_MSG             0x3EC //8 bits value

#if ( VERSION == 0x0157 || VERSION == 0x0147 || VERSION==0x0257)
	#define CAN_ID_COUPLED_BOARD        1
#elif VERSION == 0x0150 || VERSION == 0x0140 || VERSION==0x0250
	#define CAN_ID_COUPLED_BOARD        2
#endif




/******************************************************/
// macro
/******************************************************/
#define ASK_PARM(msg, var1) \
	AS1_printStringEx (msg); \
	AS1_printStringEx (" ["); \
	AS1_printWord16AsChars (*var1); \
	AS1_printStringEx ("] : "); \
	AS1_getStringEx (buffer, SMALL_BUFFER_SIZE, true); \
	iretval = AS1_atoi (buffer, AS1_strlen(buffer, SMALL_BUFFER_SIZE)); \
	*var1 = iretval;
	
//----------------------------------------
#define ASK_PARM_32(msg, var1) \
	AS1_printStringEx (msg); \
	AS1_printStringEx (" ["); \
	AS1_printDWordAsChars (*var1); \
	AS1_printStringEx ("] : "); \
	AS1_getStringEx (buffer, SMALL_BUFFER_SIZE, true); \
	iretval = AS1_atoi (buffer, AS1_strlen(buffer, SMALL_BUFFER_SIZE)); \
	*var1 = (dword)iretval;

//----------------------------------------
#define BEGIN_SPECIAL_MSG_TABLE(x) \
	switch (x & 0x7F) \
	{ \
		default: \
			break;

//----------------------------------------
#define END_SPECIAL_MSG_TABLE \
	}

//----------------------------------------	
// message table macros 
// 0x7f = 0111 1111b
#define BEGIN_MSG_TABLE(x) \
	CAN_TEMP16 = (word)extract_l(x); \
	switch (CAN_TEMP16 & 0x7f) \
	{ \
		default: \
			return ERR_OK; \
			break;
			
//----------------------------------------
#define HANDLE_MSG(x, y) \
		case x: \
			y(CAN_DATA) ; \
			break;

//----------------------------------------	
#define END_MSG_TABLE \
	}


#endif 
