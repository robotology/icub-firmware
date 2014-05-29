#include<p30f4011.h>
#include<can.h>
//#include"Pin Definitions.h"
#include <iCubCanProtocol.h>
#include <iCubCanProto_types.h>

/* Frame formats */
#define STANDARD_FORMAT            0
#define EXTENDED_FORMAT            1

/* Frame types   */
#define DATA_FRAME                 1
#define REMOTE_FRAME               0

#define CAN_TX_SOFTWARE_BUFFER_SIZE 20

// CAN RX message buffer
#define CAN_RX_SOFTWARE_BUFFER_SIZE 5


// unsigned char can1TxEn;
//#define CAN1_TX_EN  can1TxEn=1;
//#define CAN1_TX_DIS can1TxEn=0;

/* can bus message structure */
typedef struct canmsg_tag 
{
	unsigned char	CAN_data[8];					// CAN bus message 
	unsigned int 	CAN_messID;						// message ID - arbitration 
	unsigned char 	CAN_frameType;					// data or remote frame 
	unsigned char 	CAN_frameFormat;				// standard or extended frame 
	unsigned char 	CAN_length;						// len of the data 
} canmsg_t;


typedef struct 
{
    uint8_t                    shift;
    uint8_t                    indexInSet; //triangle index in set
    uint8_t                    setNum; //equal to sda num
    uint8_t                    isToUpdate; //if =1 triangle needs to be recalibrated 
    uint16_t                   CDCoffset;
}triangle_cfg_t;

#define triangles_max_num   16 

void CAN_Init();
unsigned char CAN1_send(unsigned int MessageID,unsigned char FrameType,unsigned char Length,unsigned char *Data);
void CAN1_interruptTx (void);
void CAN1_interruptRx (void);
int  CAN1_getRxbufferIndex();
void CAN1_RxbufferIndex_dec(); //canRxBufferIndex --
int CAN1_handleRx    (unsigned int board_id);
void can_receive_additional_info(); 
void can_send_additional_info(); 
void can_enaDisa_transmission_messages(uint8_t enable);

