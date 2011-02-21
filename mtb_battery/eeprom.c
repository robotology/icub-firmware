#include "eeprom.h"
//
// EEPROM definitions
// 

// EEPROM memorized data
// size: 8 + 8+8 + 6*8 + (8+8) + 6*(8+8) = 184 = (23*8)
typedef struct s_eeprom
{
  // configrazioni relative alla scheda
  unsigned EE_B_EEErased  :1;  // if 1 the ee has been erased (hopefully ;-)
  unsigned EE_B_EnableWD  :1;
  unsigned EE_B_EnableBOR :1;
  
  // configurazioni relative al CAN
  unsigned char EE_CAN_BoardAddress;  
  unsigned char EE_CAN_DestAddress;        // command destination address 
  unsigned char EE_CAN_MessageDataRate;    // framerate of outgoing messages
  unsigned char EE_CAN_Speed;
  
  // configurazioni relative ai canali analogici
  unsigned char EE_AN_ActiveChannels[13];  // sequenza di acquisizione canali 0 se canale non usato
  unsigned char EE_AN_SelectedChannel;     // canale attualmente attivo
  unsigned int  EE_AN_ChannelScanningTime; // tempo di scansione dei canali in 100aia di usec
  unsigned int  EE_AN_ChannelOffset[6];    // DAC generated offset
  unsigned int  EE_AN_ChannelValue[13];    // ADC values  

  // torque/force
  int  EE_TF_TorqueValue[3];      // Torque values
  int  EE_TF_ForceValue[3];       // Force values

  int           EE_TF_TMatrix[6][6];       // SG2TF Transformation Matrix
  unsigned int  EE_ChkSum;                 // data validation checksum
} s_eeprom;