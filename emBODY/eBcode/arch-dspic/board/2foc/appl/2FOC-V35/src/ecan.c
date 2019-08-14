//   
//  CAN configuration related rouines:
//  masks, filters, DMA,  &co.
//


#include <p33FJ128MC802.h>
#include <string.h>

#include "ecan.h"
#include "system.h"
#include "faults.h"

volatile int CAN_BusOff = 0;

// Usando questa struttura il passaggio dei dati nella DMARAM diventa una memcpy dei prime 7 word
typedef struct
{
  // Message Buffer Word 0
  unsigned IDE:1;   // Identifier Extended
  unsigned SRR:1;
  unsigned SID:11;  // SID, 11 bit id max of 0x7FF
  unsigned :3;
  // Message Buffer Word 1
  unsigned EID6to17:12;  // EID[17..6] 
  unsigned :4;
  // Message Buffer Word 2
  unsigned DLC:4; // datalen
  unsigned RB0:1; // reserved
  unsigned :3;
  unsigned RB1:1; // reserved
  unsigned RTR:1; // remote trans. req.	
  unsigned EID0to5:6;  // EID[5..0]
  // Message Buffer Word 3,4,5,6

  unsigned char data[8];
    
  // Message Buffer Word 7
  unsigned :8;
  unsigned FILHIT:5;
  unsigned :3;

} __attribute__((__packed__)) tCANMessage;

/*
tCanErrors CanErrors = {

	.CAN_RTRFlagActive = 0,
	.CAN_BufferOverRun = 0,
	.CAN_BusOff = 0,
	.CAN_TXPasv = 0,
	.CAN_RXPasv = 0,
	.CAN_Warn = 0,
};
*/
/* DMA CAN Message Buffer Configuration */
typedef unsigned int ECAN1RXMSGBUF [8][ECAN1_MSG_BUF_LENGTH];
typedef unsigned int ECAN1TXMSGBUF [8][ECAN1_MSG_BUF_LENGTH];
// Define ECAN Message Buffer
ECAN1TXMSGBUF ecan1MsgBufTx __attribute__((space(dma),aligned(ECAN1_MSG_BUF_LENGTH*16)));
ECAN1RXMSGBUF ecan1MsgBufRx __attribute__((space(dma),aligned(ECAN1_MSG_BUF_LENGTH*16)));

void (*ECANRxCb)(unsigned long id, unsigned char len, tCanData *payload);

static void s_hal_can_rxFilter_set_mask( unsigned char mask_num, unsigned long mask_val, unsigned char extid);
static void s_hal_can_rxFilter_set_id(unsigned char id_num, unsigned long id_val, unsigned char extid, unsigned char mask_sel);

void ECANClkInit(void)
//
// CAN clocking and sampling parameter settings
// 
{
  // FCAN is selected to be DFCY, FCAN = DFCY = 40MHz
  C1CTRL1bits.CANCKS = 0x1;

  // Synchronization Jump Width set to 4 Tq
  C1CFG1bits.SJW = 0x3;

  // Bit Time = (Sync Segment + Propagation Delay + Phase Segment 1 + Phase Segment 2)=20*Tq
  // Phase Segment 1 = 8Tq
  // Phase Segment 2 = 6Tq
  // Propagation Delay = 5Tq
  // Sync Segment = 1Tq
  // CiCFG1<BRP> =(FCAN /(2 ×N×FBAUD))– 1
  // Bit rate of 1Mbps
  C1CFG1bits.BRP = BRP_VAL ;
  // Phase Segment 1 time is 8 Tq
  C1CFG2bits.SEG1PH=0x7;
  // Phase Segment 2 time is set to be programmable
  C1CFG2bits.SEG2PHTS = 0x1;
  // Phase Segment 2 time is 6 Tq
  C1CFG2bits.SEG2PH = 0x5;
  // Propagation Segment time is 5 Tq
  C1CFG2bits.PRSEG = 0x4;
  // Bus line is sampled three times at the sample point
  C1CFG2bits.SAM = 0x1;
}

void ECANWriteRxAcptMask(
//
// This function configures Acceptance Filter Mask
//
  int m, // Mask number [0-2]
  long identifier,  // 11/29 bit identifier
    // Filter Mask Identifier (29-bits) : 0b000f ffff ffff ffff ffff ffff ffff ffff
    //                                         |____________|_____________________|
    //                                                SID10:0           EID17:0
    // Filter Mask Identifier (11-bits) : 0b0000 0000 0000 0000 0000 0fff ffff ffff
    //                                                                |___________|
    //                                                                    SID10:0 
  unsigned int mide,  // match both 11/29id
    // "0" Match either standard or extended address message if filters match 
    // "1" Match only message types that correpond to 'exide' bit in filter
  unsigned int exide ) // extended or standard id 
    // "0" for standard identifier 
		// "1" for Extended identifier
{
  unsigned long sid10_0=0, eid15_0=0, eid17_16=0;
  unsigned int *maskRegAddr;

  // change register window
  C1CTRL1bits.WIN=1;

  // Obtain the Address of CiRXMmSID register for given Mask number "m"
  maskRegAddr = (unsigned int *)(&C1RXM0SID + (m << 1));

  // Bit-filed manupulation to write to Filter Mask register
  if(1==exide) 
  { 	
    // Filter Extended Identifier
    eid15_0 = (identifier & 0xFFFF);
    eid17_16= (identifier>>16) & 0x3;
    sid10_0 = (identifier>>18) & 0x7FF;

    if(1==mide)
      *maskRegAddr=((sid10_0)<<5) + 0x0008 + eid17_16;  // Write to CiRXMnSID Register
    else
      *maskRegAddr=((sid10_0)<<5) + eid17_16;  // Write to CiRXMnSID Register
    *(maskRegAddr+1)= eid15_0;  // Write to CiRXMnEID Register
  }
  else
  {
    // Filter Standard Identifier
    sid10_0 = (identifier & 0x7FF);			
    if(1==mide)
      *maskRegAddr=((sid10_0)<<5) + 0x0008;  // Write to CiRXMnSID Register
    else
      *maskRegAddr=(sid10_0)<<5;  // Write to CiRXMnSID Register	
		
    *(maskRegAddr+1)=0;  // Write to CiRXMnEID Register
  }

  // lock mask changes
  C1CTRL1bits.WIN=0;	
}

void ECANWriteRxAcptFilter(
// This function configures Acceptance Filter  
  int n, // Filter number
  long identifier, // 11/29 bit Identifier
    // Filter Identifier (29-bits) : 0b000f ffff ffff ffff ffff ffff ffff ffff
    //                                    |____________|_____________________|
    //                                        SID10:0           EID17:0
    // Filter Identifier (11-bits) : 0b0000 0000 0000 0000 0000 0fff ffff ffff
    //                                                           |___________|
    //                                                              SID10:0
  unsigned int exide, // extended or standard id
    // "0" for standard identifier 
		// "1" for Extended identifier
  unsigned int bufPnt, // pointer to filtered message buffer [0..15]
  unsigned int maskSel ) // optional masking  of identifier bits [0-3]
{
  unsigned long sid10_0=0, eid15_0=0, eid17_16=0;
  unsigned int *sidRegAddr,*bufPntRegAddr,*maskSelRegAddr, *fltEnRegAddr;

  // unlock filter changes
  C1CTRL1bits.WIN=1;

  // Obtain the Address of CiRXFnSID, CiBUFPNTn, CiFMSKSELn and CiFEN register for a given filter number "n"
  sidRegAddr = (unsigned int *)(&C1RXF0SID + (n << 1));
  bufPntRegAddr = (unsigned int *)(&C1BUFPNT1 + (n >> 2));
  maskSelRegAddr = (unsigned int *)(&C1FMSKSEL1 + (n >> 3));
  fltEnRegAddr = (unsigned int *)(&C1FEN1);

  // Bit-filed manupulation to write to Filter identifier register
  if(1==exide) 
  {
    // Filter Extended Identifier
    eid15_0 = (identifier & 0xFFFF);
    eid17_16= (identifier>>16) & 0x3;
    sid10_0 = (identifier>>18) & 0x7FF;

    *sidRegAddr=(((sid10_0)<<5) + 0x8) + eid17_16;  // Write to CiRXFnSID Register
    *(sidRegAddr+1)= eid15_0;  // Write to CiRXFnEID Register
  }
  else
  {  
    // Filter Standard Identifier
    sid10_0 = (identifier & 0x7FF);			
    *sidRegAddr=(sid10_0)<<5;  // Write to CiRXFnSID Register
    *(sidRegAddr+1)=0;  // Write to CiRXFnEID Register
  }

  *bufPntRegAddr = (*bufPntRegAddr) & (0xFFFF - (0xF << (4 *(n & 3))));  // clear nibble
  *bufPntRegAddr = ((bufPnt << (4 *(n & 3))) | (*bufPntRegAddr));  // Write to C1BUFPNTn Register

  *maskSelRegAddr = (*maskSelRegAddr) & (0xFFFF - (0x3 << ((n & 7) * 2)));  // clear 2 bits
  *maskSelRegAddr = ((maskSel << (2 * (n & 7))) | (*maskSelRegAddr));  // Write to C1FMSKSELn Register

  *fltEnRegAddr = ((0x1 << n) | (*fltEnRegAddr));  // Write to C1FEN1 Register

  // lock filter changes
  C1CTRL1bits.WIN=0;

}

void ECANDma1Init(void)
// DMA Initialization for ECAN1 Transmission
{
  // clear the collission flags
  DMACS0 = 0;	
  // setup channel 1 for peripheral indirect addressing mode
  // normal operation, word operation and select as Tx to peripheral
  DMA1CON = 0x2020; 
  // setup the address of the peripheral ECAN1 (C1TXD)
  DMA1PAD=0x0442;
  // Set the data block transfer size of 8 
  DMA1CNT=0x0007;
  // automatic DMA Tx initiation by DMA request
  DMA1REQ=0x0046;	
  // DPSRAM TX start adddress offset value
  DMA1STA=__builtin_dmaoffset(ecan1MsgBufTx); 
  // enable the channel
  DMA1CONbits.CHEN=1;	 
}

void ECANDma2Init(void)
// DMA Initialization for ECAN1 Reception
{
  // clear the collission flags
  DMACS0=0;
  // setup channel 2 for peripheral indirect addressing mode 
  // normal operation, word operation and select as Rx to peripheral
  DMA2CON=0x0020;
  // setup the address of the peripheral ECAN1 (C1RXD)
  DMA2PAD=0x0440;
  // Set the data block transfer size of 8
  DMA2CNT=7;
  // automatic DMA Rx initiation by DMA request
  DMA2REQ=0x0022;
  // DPSRAM RX start adddress offset value
  DMA2STA=__builtin_dmaoffset(ecan1MsgBufRx);	
  // enable the channel
  DMA2CONbits.CHEN=1;
}

int ECANRxFilterAdd(unsigned long id, unsigned long mask)
{
  static unsigned long masks[3];
  static unsigned char masks_idx = 0;
  static unsigned int id_idx = 0;
  int mask_n;
  int i;
  int mide;

  //
  // Filter configuration
  //

  // if all filters are already used we can not
  // add another.
  // Return with error.
  if(id_idx > 15)
		return -1;

  // make sure unused bit are not floating
  // as we will do comparsions on mask values
  // and we must consider equal two masks with
  // different unused bits.
  // Pull-down all unused bits.
  mask = mask & 0x9fffffff;

  // do the same for ids (not required right now)
  id = id & 0x9fffffff;


  // if the mask is a void mask, then detect it
  // because the ECAN has a special void mask hardwired,
  // so there is no need to allocate a new mask.
  // Mask is void if it has:
  //  - all 0b1 in all CAN id/extid valid bits
  //  - 0b1 in the 29 bit flag
  // TODO: check ECAN documentation about void mask behaviour
  // about MIDE
  if(mask == 0x9fffffff){

    // mask #3 is the special magic value
    // for the HW void mask
    mask_n = 3;

  }else{

    // we need a custom mask, check if we have
    // already configured a custom mask that
    // is exactly equal to the required one.
    mask_n = -1;
	
    for(i=0;i<masks_idx;i++){
      // search in the allocated masks
	  if(masks[i] == mask){
		// found a suitable mask. Use it!
		mask_n = i;
		break;
	  }
	}
	
    // if we have not found any suitable mask, we 
    // finally need to allocate one HW mask
	if(mask_n == -1){
      // if we have no more spare HW mask, then return
      // with error.
	  if(masks_idx == 3)
        return -2;

      // allocate a new HW mask
	  masks[masks_idx] = mask;
	  mask_n = ++masks_idx;

		
      // Mask Configuration
      // ecan1WriteRxAcptMask(int m, long identifierMask, unsigned int mide, unsigned int exide)
      //  m = 0 to 2 -> Mask Number
      //  identifier -> SID <10:0> : EID <17:0> 
      //  mide = 0 -> Match either standard or extended address message if filters match 
      //  mide = 1 -> Match only message types that correpond to 'exide' bit in filter
      //  exide = 0 -> Match messages with standard identifier addresses 
      //  exide = 1 -> Match messages with extended identifier addresses
      //
      // definisce la maschera per i pacchetti a 29 bit
      // this mask checks all bits of the packet  

	  // mide is stored in bit 31 of user mask. Extract it
	  mide = !!(mask & 0x80000000);
      // the mask must not include mide
      mask = mask & 0x1fffffff;
	  ECANWriteRxAcptMask(mask_n, mask, mide, !!(id & ECAN_ID_29));
	    
    }
  }

  // Filter Configuration
  // ecan1WriteRxAcptFilter(int n, long identifier, unsigned int exide,unsigned int bufPnt,unsigned int maskSel)	
  //  n = 0 to 15 -> Filter number
  //  identifier -> SID <10:0> : EID <17:0> 
  //  exide = 0 -> Match messages with standard identifier addresses 
  //  exide = 1 -> Match messages with extended identifier addresses 
  //  bufPnt = 0 to 14  -> RX Buffer 0 to 14
  //  bufPnt = 15 -> RX FIFO Buffer
  //  maskSel = 0	->	Acceptance Mask 0 register contains mask
  //  maskSel = 1	->	Acceptance Mask 1 register contains mask
  //  maskSel = 2	->	Acceptance Mask 2 register contains mask
  //  maskSel = 3	->	No Mask Selection 
  // Enable buffers 1 for RX. Packets with matching id will 
  // be put in ecan1MsgBuf[1]. 
  
  ECANWriteRxAcptFilter(id_idx++,id,!!(id & ECAN_ID_29),0xf,mask_n);
  
  // 
  return 0;
}

void ECANStart()
{
    // put the module in normal mode
  C1CTRL1bits.REQOP=1;
  while(C1CTRL1bits.OPMODE != 1);

  // put the module in normal mode
  C1CTRL1bits.REQOP=0;
  while(C1CTRL1bits.OPMODE != 0);

  // ECAN transmit/receive message control

  // 4 CAN Message Buffers in DMA RAM
  // clear the buffer and overflow flags
  // C1RXFUL1=C1RXFUL2=C1RXOVF1=C1RXOVF2=0x0000;
  // ECAN1, Buffer 0 is a Transmit Buffer
  C1TR01CONbits.TXEN0=1;			
  // ECAN1, Buffer 1 is a Transmit Buffer
  C1TR01CONbits.TXEN1=1;	
  // ECAN1, Buffer 2 is a Transmit Buffer
  C1TR23CONbits.TXEN2=1;

//#ifdef ECAN_TX4BUF
	// ECAN1, Buffer 3 is a Transmit Buffer
  C1TR23CONbits.TXEN3=1;
//#else
  // ECAN1, Buffer 3 is a Receive Buffer
//  C1TR23CONbits.TXEN3=0;
//#endif
  // ECAN1, Buffer 4 is a Receive Buffer
  C1TR45CONbits.TXEN4=0;
  // ECAN1, Buffer 5 is a Receive Buffer
  C1TR45CONbits.TXEN5=0;
  // ECAN1, Buffer 6 is a Receive Buffer
  C1TR67CONbits.TXEN6=0;
  // ECAN1, Buffer 7 is a Receive Buffer
  C1TR67CONbits.TXEN7=0;

  // Message Buffer 0 Priority Level
  C1TR01CONbits.TX0PRI=0b0; 		
  C1TR01CONbits.TX1PRI=0b0;
  C1TR23CONbits.TX2PRI=0b0;

//#ifdef ECAN_TX4BUF
  C1TR23CONbits.TX3PRI=0b0;

  // Set up hw buffers and fifo.
  // buf 0 is for TX. buf 1 is not used.
  // bufs 2, 3, 4 and 5 are for RX,
  // used in FIFO fashion 
  C1FCTRLbits.FSA = 4; // FIFO starts from buf 4 - seems it needs a 16-bit aligned or 2-power number ?
//#else
///  C1FCTRLbits.FSA = 3; // FIFO starts from buf 3 - seems it needs a 16-bit aligned or 2-power number ?
//#endif
  C1FCTRLbits.DMABS = 0b10; // FIFO end. 8 buffers from the begin of DMA ram. INCLUDING TX and not used

  C1INTEbits.ERRIE = 1; // config ecan to produce interrupt on errors
  IEC2bits.C1IE = 1; // enables ecan interrupt

  // enable CAN ints. PHILOSOPY: move in more acconcing function to ENABLE can (here it is in Initialization
  IEC1bits.DMA2IE = 1;
  // init DMA channels 0 and 2 for CAN messaging
  // in order to get notified about CAN rx events.
  // We polls on mailboxes during the FOC core loop.
    // initialise the DMA channel 1 for ECAN Tx
  ECANDma1Init();

  // initialise the DMA channel 2 for ECAN Rx
  ECANDma2Init();

}

void ECANInit (void (*RxCb)(unsigned long id, unsigned char len, tCanData *payload))

//
// Initialises the ECAN module
//                                                                            
{

  // put the module in configuration mode

  ECANRxCb = RxCb;
  C1CTRL1bits.REQOP=4;
  while(C1CTRL1bits.OPMODE != 4);

  // clocking and sampling init
  ECANClkInit(); 

  // reset error flag
  C1INTF = 0xf;
  C1CTRL1bits.WIN=0;

}



//
// DMA interrupt handlers
//

//TODO: This Irq handler is empty. Remove.

void __attribute__((interrupt, no_auto_psv)) _DMA1Interrupt(void)
// Channel 1 of the DMA is configured to Tx ECAN messages of ECAN module 1. 
{
   IFS0bits.DMA1IF = 0;          // Clear the DMA1 Interrupt Flag;
}


// when the CAN module enters in an error state
// an interrupt is triggered and the error state
// is saved in local vars. When the ECAN leaves
// the error state, no interrupt is generated, thus
// we have to poll for this
void ECANCheckState()
{
	//change reg window
	C1CTRL1bits.WIN=1;

	if(C1INTFbits.TXBP == 0){
		SysError.CAN_TXIsPasv = 0;
	}

	if(C1INTFbits.RXBP == 0){
		SysError.CAN_RXIsPasv = 0;
	}

	if(C1INTFbits.EWARN == 0){
		SysError.CAN_IsWarnTX = 0;
    SysError.CAN_IsWarnRX = 0;		
	}
	
	C1CTRL1bits.WIN=0;
}

void __attribute__((interrupt, no_auto_psv)) _C1Interrupt(void)
// CAN error interrupt service routine
{
	IFS2bits.C1IF = 0;
	int pasv;
	
	pasv = 0;

	// change register window
	C1CTRL1bits.WIN=1;

	if(C1INTFbits.TXBO){
		CAN_BusOff = 1;
		//SysError.CANFailure = 1;
       
	}

	if(C1INTFbits.TXBP){
		SysError.CAN_TXIsPasv = 1;
		SysError.CAN_TXWasPasv = 1;
		pasv = 1;
	//	SysError.CANFailure = 1;
	}

	if(C1INTFbits.RXBP){
		SysError.CAN_RXIsPasv = 1;
		SysError.CAN_RXWasPasv = 1;
		//SysError.CANFailure = 1;
	}

	if(C1INTFbits.EWARN){
		SysError.CAN_WasWarn = 1;	
		SysError.CAN_IsWarnTX = 1;	
    SysError.CAN_IsWarnRX = 1;	
		//SysError.CANFailure = 1;
	}

	C1INTF = 0xf;

	C1CTRL1bits.WIN=0;

	if(CAN_BusOff == 1){
		// CAN bus off is the most serious error that can happen
	    // on the CAN wire. We have to fire a fault in this condition.
	    // FYI this happen for example if we short CANL and CANH but not
	    // necessarly when the cable is cut (see below)
		FaultConditionsHandler();
	}
	if(pasv){
		// we trust this condition happens when the CAN cable is cutted.
	    // we also think this condition will not happens unless a seriuos
	    // problem affect CAN line (maybe not cutted, but very bad anyway)
	    // for this reason we fire a fault
		FaultConditionsHandler();
	}

}

void ECANPrepareTxBuffer(tCANMessage *CANTxMessage, unsigned long id, unsigned char len, tCanData *payload)
{
  

  if(id & ECAN_ID_29){
  
	// 29 bit
  	CANTxMessage->EID0to5 = id & 0x3f;
  	CANTxMessage->EID6to17 = (id >> 6)& 0xfff;
  	CANTxMessage->SID = (id >> 18) & 0x7ff;
  
	CANTxMessage->IDE = CANTxMessage->SRR = 1;
  }else{

    // 11 bit
    CANTxMessage->SID = id & 0x7ff;

    CANTxMessage->EID0to5 = 0;
  	CANTxMessage->EID6to17 = 0;

	  CANTxMessage->IDE = CANTxMessage->SRR = 0;
  }
  
  CANTxMessage->RTR=0x0; //not remote
//  CANTxMessage->RB0=CANTxMessage.RB1=0x0; // reserved

  CANTxMessage->DLC=len;

  memcpy(CANTxMessage->data,payload->b,len);
}


int ECANSend(unsigned long id, unsigned char len, tCanData *payload)
// sends the message on CAN bus  
// Parameters: 
{

  static int ecanprio[4] = {0,0,0,0};

  // If TX is still pending on both buffers, give up!
  if( C1TR01CONbits.TXREQ0 && C1TR01CONbits.TXREQ1 && C1TR23CONbits.TXREQ2 &&  C1TR23CONbits.TXREQ3)
  {
    // TODO: lanciare un acconccio log di errore
    return -1;
  }

  // If Buf0 is free then use Buf0
  if(!C1TR01CONbits.TXREQ0){
    
	ECANPrepareTxBuffer((tCANMessage*) ecan1MsgBufTx[0], id, len, payload);
    // set the message for transmission
 
//	C1TR01CONbits.TX0PRI=0b1;

	    ecanprio[0] = 0;
//#ifdef ECAN_TX4BUF
	if((ecanprio[1] < 0b11) && (ecanprio[2] < 0b11) && (ecanprio[3]< 0b11)){
		ecanprio[1]++;
		ecanprio[2]++;
		ecanprio[3]++;
	}
//#else
//	if((ecanprio[1] < 0b11) && (ecanprio[2] < 0b11)){
//		ecanprio[1]++;
//		ecanprio[2]++;
//	}

//#endif
	C1TR01CONbits.TX0PRI = ecanprio[0];
	C1TR01CONbits.TX1PRI = ecanprio[1];
	C1TR23CONbits.TX2PRI = ecanprio[2];

//#ifdef ECAN_TX4BUF
	C1TR23CONbits.TX3PRI = ecanprio[3];
//#endif

    C1TR01CONbits.TXREQ0=1;
  
  
  }else if(!C1TR01CONbits.TXREQ1){
      // Buf 0 was not free. If buf1 is free use buf 1 
      //
      ECANPrepareTxBuffer((tCANMessage*) ecan1MsgBufTx[1], id, len, payload);

	  ecanprio[1] = 0;

//#ifdef ECAN_TX4BUF
	 if((ecanprio[0] < 0b11) && (ecanprio[2] < 0b11) && (ecanprio[3]< 0b11)){
		ecanprio[0]++;
		ecanprio[2]++;
		ecanprio[3]++;
	 }
//#else
//	 if((ecanprio[0] < 0b11) && (ecanprio[2] < 0b11)){
//		ecanprio[0]++;
//		ecanprio[2]++;
//	 }
//#endif

	  C1TR01CONbits.TX0PRI = ecanprio[0];
	  C1TR01CONbits.TX1PRI = ecanprio[1];
	  C1TR23CONbits.TX2PRI = ecanprio[2];

//#ifdef ECAN_TX4BUF
	  C1TR23CONbits.TX3PRI = ecanprio[3];
//#endif
      // set the message for transmission
      C1TR01CONbits.TXREQ1=1;
    
  }else if(!C1TR23CONbits.TXREQ2){

      // use buf 2 
      
	  ECANPrepareTxBuffer((tCANMessage*) ecan1MsgBufTx[2], id, len, payload);
      // set the message for transmission
	  ecanprio[2] = 0;

//#ifdef ECAN_TX4BUF
	 if((ecanprio[0] < 0b11) && (ecanprio[1] < 0b11) && (ecanprio[3]< 0b11)){
		ecanprio[0]++;
		ecanprio[1]++;
		ecanprio[3]++;
	 }
//#else
//	 if((ecanprio[0] < 0b11) && (ecanprio[1] < 0b11)){
//		ecanprio[0]++;
//		ecanprio[1]++;
//	 }
//#endif

	  C1TR01CONbits.TX0PRI = ecanprio[0];
	  C1TR01CONbits.TX1PRI = ecanprio[1];
	  C1TR23CONbits.TX2PRI = ecanprio[2];

//#ifdef ECAN_TX4BUF
	  C1TR23CONbits.TX3PRI = ecanprio[3];
//#endif
			
      C1TR23CONbits.TXREQ2=1;
    }
//#ifdef ECAN_TX4BUF
	else{
 // use buf 3 
      
	  ECANPrepareTxBuffer((tCANMessage*) ecan1MsgBufTx[3], id, len, payload);
      // set the message for transmission
	  ecanprio[3] = 0;


	 if((ecanprio[0] < 0b11) && (ecanprio[1] < 0b11) && (ecanprio[2]< 0b11)){
		ecanprio[0]++;
		ecanprio[1]++;
		ecanprio[2]++;
	 }

	  C1TR01CONbits.TX0PRI = ecanprio[0];
	  C1TR01CONbits.TX1PRI = ecanprio[1];
	  C1TR23CONbits.TX2PRI = ecanprio[2];
	  C1TR23CONbits.TX3PRI = ecanprio[3];
			
      C1TR23CONbits.TXREQ3=1;
	}

//#endif
 return 0; 
}


// CAN RX ISR
void __attribute__((interrupt, no_auto_psv)) _DMA2Interrupt(void)
// Calls CAN message parser when DMA for incoming data completed
{
  // Clear the DMA2 Interrupt Flag;

// get data from CAN mailboxes and parse incoming commands

  int buffer;
  int count = 0;
  tCANMessage *msg;
  unsigned long id;
  unsigned long tmp;
  int len;
  tCanData data;

  IFS1bits.DMA2IF = 0;

  // ensure that if for any reason the DMA buffer never empties
  // the loop sooner or later ends.
  while(MAX_RX_INT != count++)
  {
    // get read pointer int the fifo ring. Hw updates it for us
    buffer = C1FIFObits.FNRB;
 
    // if the read location pointer points to an empty element,
    // then no RX packet is available
    if(!(C1RXFUL1 & (1<<buffer)))
    {
      break;
    }
    // get the message! len is not full msg struct as it contains 
    // also SW stuffs
    msg = (tCANMessage*) ecan1MsgBufRx[buffer]; 
    
	len = msg->DLC;
	
	if(len > 8){
		 len = 8;
		 SysError.CAN_DLCError = 1;
		 //SysError.CANFailure = 1;
	}

	if(msg->IDE){

		id = ECAN_ID_29; 
		id |= msg->EID0to5;

		tmp = msg->EID6to17;
		id |= tmp<<6;

		tmp = msg->SID;
		id |= tmp<<18;

	}else{

		id = msg->SID;

	}



	memcpy(data.b, msg->data, len);

    // C1RXFUL bits are lukly readable or clearable but not settable,
    // so writing it with all ones, except for the bit associated to
    // the buffer we just read, ensures no HW/SW race can happen
	C1RXFUL1 = ~(1<<buffer);


    // if message is RTR ..
    if(1 == msg->RTR)
    {
      // .. throw away it 
      SysError.CAN_RTRFlagActive=1;
	  //SysError.CANFailure = 1;
  	  continue;
    }

    // got one good msg :-) call the parser
    ECANRxCb(id, len, &data);
  }
  
  if(MAX_RX_INT == count)
  {
    // CAN Rx buffer overrun
    SysError.CAN_BufferOverRun=1;
    //SysError.CANFailure = 1;
  }
}


//VALE

typedef     char                    int8_t;
typedef     unsigned char           uint8_t;
typedef     signed int              int16_t;
typedef     unsigned int            uint16_t;
typedef     signed long             int32_t;
typedef     unsigned long           uint32_t;    
typedef     signed long long        int64_t;
typedef     unsigned long long      uint64_t; 

    
extern void hal_can_receptionfilter_set(unsigned char mask_num, unsigned long mask_val, unsigned char identifier_num,
                                                unsigned long identifier_val, unsigned char extid)
{
    s_hal_can_rxFilter_set_mask(mask_num, mask_val, extid);

    s_hal_can_rxFilter_set_id(identifier_num, identifier_val, extid, mask_num);
}

static void s_hal_can_rxFilter_set_mask( unsigned char mask_num, unsigned long mask_val, unsigned char extid)
{
    uint32_t sid = 0,eid15_0=0, eid17_16=0;
    uint16_t *maskRegAddr, *maskExtRegAddr; 

    // unlock mask changes
    C1CTRL1bits.WIN=1;
    
    // Obtain the Address of CiRXMmSID register for given Mask number "m"
    maskRegAddr = (uint16_t *)(&C1RXM0SID + (mask_num << 1));
    maskExtRegAddr = maskRegAddr + 1;

    // Bit-filed manupulation to write to Filter Mask register
    if(extid) 
    {     
        // Filter Extended Identifier
        eid15_0 = (mask_val & 0xFFFF);
        eid17_16= (mask_val>>16) & 0x3;
        sid = (mask_val>>18) & 0x7FF;
        
        // 0x008 for set mide bit: so filter maches only eid or only sid.
        *maskRegAddr=((sid)<<5) + 0x0008 + eid17_16;  // Write to CiRXMnSID Register
        *maskExtRegAddr= eid15_0;  // Write to CiRXMnEID Register
    }
    else
    {
        // Filter Standard Identifier
        sid = (mask_val & 0x7FF);
        
        // 0x008 for set mide bit: so filter maches only eid or only sid.
        *maskRegAddr = ((sid)<<5) + 0x0008;  // Write to CiRXMnSID Register    
        *maskExtRegAddr = 0;  // Write to CiRXMnEID Register
    }

    // lock mask changes
    C1CTRL1bits.WIN=0;
    
}

static void s_hal_can_rxFilter_set_id(unsigned char id_num, unsigned long id_val, unsigned char extid, unsigned char mask_sel) 
{
  
    uint32_t sid = 0, eid15_0 = 0, eid17_16 = 0;
    uint16_t *sidRegAddr, *exidRegAddr, *bufPntRegAddr=NULL, *maskSelRegAddr=NULL, *fltEnRegAddr;
    uint16_t aux;
        
    // unlock filter changes
    C1CTRL1bits.WIN=1;
    
    // Obtain the Address of CiRXFnSID, CiBUFPNTn, CiFMSKSELn and CiFEN register for a given filter number "n"
    sidRegAddr = (uint16_t *)(&C1RXF0SID + (id_num << 1)); // I need moltiply for 2 (<< 1), beause after C1RXF0SID there is C1RXF0EID register. regiters memory map
    exidRegAddr = sidRegAddr + 1; // C1RXF0EID is next to C1RXF0SID. see regiters memory map.

    fltEnRegAddr = (uint16_t *)(&C1FEN1);

    // Bit-field manupulation to write to Filter identifier register
    if(extid) 
    {
        // Filter Extended Identifier
        eid15_0 = (id_val & 0xFFFF);
        eid17_16= (id_val>>16) & 0x3;
        sid = (id_val>>18) & 0x7FF;
        
        // 0x8 ==> set EXIDE bit so , the filter maches only EID.
        *sidRegAddr =(((sid)<<5) + 0x8) + eid17_16;  // Write to CiRXFnSID Register
        *exidRegAddr = eid15_0;  // Write to CiRXFnEID Register
    }
    else
    {  
        // Filter Standard Identifier
        sid = (id_val & 0x7FF);            
        *sidRegAddr=(sid)<<5;  // Write to CiRXFnSID Register in bit from 5 to 15
        *exidRegAddr = 0;  // Write to CiRXFnEID Register
    }

    //select the correct buffer and mask register
    if( (id_num <=3) )
    {
        bufPntRegAddr = (uint16_t *)(&C1BUFPNT1);
        maskSelRegAddr = (uint16_t *)(&C1FMSKSEL1);
    }
    else if( (id_num >= 4) && (id_num <=7) ) 
    {
        bufPntRegAddr = (uint16_t *)(&C1BUFPNT2);
        maskSelRegAddr = (uint16_t *)(&C1FMSKSEL1);
    }
    else if( (id_num >= 8) && (id_num <=11) ) 
    {
        bufPntRegAddr = (uint16_t *)(&C1BUFPNT3);
        maskSelRegAddr = (uint16_t *)(&C1FMSKSEL2);
    }
    else if( (id_num >= 12) && (id_num <=15) ) 
    {
        bufPntRegAddr = (uint16_t *)(&C1BUFPNT4);
        maskSelRegAddr = (uint16_t *)(&C1FMSKSEL2);
    }
     
    //set the buffer where can messages will be inserted if they mach filter
    // I do filter_num%4 because in each register 4 buffer pointer.
    // I moltiply for 4, because buffer pointer is codifies in 4 bits.
    aux = 0xFFFF - (0xF <<(4* (id_num%4) ));          
    *bufPntRegAddr &= aux; //clear bits
    *bufPntRegAddr |= 0xF << (4*(id_num%4));  // set 0xF buffer, that is FIFO queue.

    if(mask_sel < 3 )
    {    
        aux = 0xFFFF - ( 0x3 <<(2*(id_num%8)) );
        *maskSelRegAddr &= aux; //clear bits
        *maskSelRegAddr |=(  mask_sel <<(2*(id_num%8)) );
    }

    *fltEnRegAddr |= (0x1 << id_num);  // Write to C1FEN1 Register
    
    // lock filter changes
    C1CTRL1bits.WIN=0;
}
