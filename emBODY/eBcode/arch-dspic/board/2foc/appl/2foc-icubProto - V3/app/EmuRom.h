/* Emulated EEPROM on Flash on DSP33 header file 				*
 * can be used to set-up a data block in flash that				*
 *  a) can be read by the FOC bootloader 						*
 *  b) can be used by the program application as config storage *
 * NOTE: make sure you are using the right gld !
 */

//
//  EMUROM USAGE
// 
// Declare a variable placed in the EmuEEPROM flash area, named as indicaterd in the paramenter DATANAME 
// the variable cannot be used to access directly data in EmuEEPROM. Thew acces is provided by a regular RAM variable 
// declared using the DECLARE_EMU_ROM_READVAR macro and the copied in init phase
// using the INIT_EMU_ROM_READVAR
// The RAM shadow can be saved back in EMUROM using REFLASH_EMU_ROM
// initialization at declare time (defaults given when reprogramming via emulator) of the EMUROM variables 
// can be performed using PLACE_EMU_ROM_DATA
//

/* usage example:
 *
 * typedef struct MyRom{
 *		tEepromHeader hdr; 		// read by bootloader also
 *		int mydata1;			// some application data
 *		int mydata2;			// some other application data
 *	}__attribute__((__packed__)) tMyRom;
 *
 * PLACE_EMU_ROM_DATA(MyRomSpace, tMyRom) = { .hdr.EE_CAN_BoardAddress = 0x3 };
 * DECLARE_EMU_ROM_READVAR(MyRomVar, tMyRom)
 * 
 * int main(){
 *
 *		INIT_EMU_ROM_READVAR(MyRomSpace, MyRomVar); // now erom data can be read by application code using MyRomVar
 * 
 * }
 *
 */
#ifndef __EMUROM_H__
#define __EMUROM_H__


//#define START__ (0x13e00 - 0x600)
#include <libpic30.h>
#include <string.h>

#include "userparms.h"

// permits to declare a variable in the EmuEEPROM area
#define PLACE_EMU_ROM_DATA(DATANAME, TYPE) const TYPE  __attribute__((space(prog))) __attribute__((aligned(_FLASH_PAGE*2))) __attribute__((__section__(".edata"))) DATANAME

// permits to declare a read only variable for getting the EmuEEprom data
#define DECLARE_EMU_ROM_READVAR(VARNAME, TYPE) TYPE VARNAME;

// used to links the variable 
#define INIT_EMU_ROM_READVAR(DATANAME, VARNAME) {_prog_addressT EE_addr; _init_prog_address(EE_addr, DATANAME); \
												 _memcpy_p2d16(&VARNAME, EE_addr, sizeof(VARNAME));}

// permette di salvare su EmuEEPROM il contenuto della variabile locale  
#define REFLASH_EMU_ROM(DATANAME,VARNAME) {  int buf[_FLASH_PAGE]; \
  int i;\
  _prog_addressT EE_addr; _init_prog_address(EE_addr, DATANAME);\
  _erase_flash(EE_addr); \
  memcpy((char*)buf,(char*)(&VARNAME),sizeof(VARNAME)); \
  for(i=0;i<sizeof(VARNAME);i+=_FLASH_ROW){ \
  _write_flash16(EE_addr+i, (( int*)(&buf))+i/2);} \
 }

// Mandatory Data structure of EmuROM data 
typedef struct 
{
  // configrazioni relative alla scheda
  unsigned EE_B_EEErased  :1;  // if 1 the ee has been erased (hopefully ;-)
  unsigned EE_B_EnableWD  :1;
  unsigned EE_B_EnableBOR :1;  

  // configurazioni relative al CAN
  unsigned char EE_CAN_BoardAddress;  
  unsigned char EE_CAN_MessageDataRate;    // framerate of outgoing messages
  unsigned char EE_CAN_Speed;


} __attribute__((__packed__)) tEepromHeader;


#define ReadFromEmuROM() INIT_EMU_ROM_READVAR(EMURomSpace, ApplicationData)

#endif

//
// Eeprom CRC calculator
//
// The firmware checks the EEPROM CRC value at init time.
// The EEPROM default values are initialized using the
// PLACE_EMU_ROM_DATA macro in globals.h
// The EEPROM default values includes values from some
// defines in this file, as well as the CRC.
// This means that modifying some of the following
// #defines requires also the EEPROM default CRC to be
// adjusted.
// For this purpose the following #define may be uncommented:
// doing this causes the CRC to be calculated and sent over 
// CAN bus.
// In this way one can adjust the default CRC easily.
//
// Note. Using this option disables FOC control and normal
// CAN operation
//
// This can be used only for FW development,
// otherwise it must be disabled.
//
//#define EEPROM_CRC_CALCULATE_AND_TX_ON_CAN

#if defined( EEPROM_CRC_CALCULATE_AND_TX_ON_CAN) && defined (FIRMWARE_SAFE_COMMAND_SET)
  #error EEPROM_CRC_CALCULATE_AND_TX_ON_CAN_must_not_be_defined_on_safe_firmware
#endif
