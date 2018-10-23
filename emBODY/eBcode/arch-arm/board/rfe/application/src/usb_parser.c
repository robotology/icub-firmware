 /****************************************************
 *  						     *
 *                                                   *
 *                                                   *
 *  Written by Marco Maggiali                        *
 *  <marco.maggiali@iit.it>                          *
 ****************************************************/
 
#include <stdio.h>
#include <string.h>
#include "stm32l4xx_hal.h"
#include "usbd_cdc_if.h"
#include "usbd_def.h"
#include "usb_parser.h"
#include <stdlib.h>

//#include "provaH.h"


//using namespace std;
//#include "embot_hw_usb.h"
//static void getMessage(uint8_t **ptr, uint16_t *size)
//{
//    embot::hw::usb::Message msg;
//    std::uint8_t numofmsg;
//    embot::hw::result_t res = embot::hw::usb::get(embot::hw::usb::Port::one, msg, numofmsg);
//    
//    if(res == embot::hw::resNOK)
//    {
//        *ptr = NULL;
//        *size = 0;
//        return;
//    }
//    *ptr = msg.data;
//    *size = msg.size;
//    return;
//    
//}





typedef enum 
{
    NONE = 0,
    LEB = 1,
    REB = 2, 
    MOU = 3,
    ELI = 4
} partId_t;

extern void DisplayExpression(uint16_t period);

static uint32_t getHexVal(uint8_t *recMsg)
{
    char *end;
    return strtol((char*)recMsg, &end, 16);
}



static uint32_t count_cmd=0;
uint32_t usbParser(uint8_t * RecMsg)
{
    uint8_t RecMsgLen=strlen((char*)RecMsg);
    uint8_t i=0;
    uint8_t value=0;
    char resp[80];
    uint32_t emotionId=0;
    partId_t partId=NONE;
    
    uint8_t *msgdata_ptr;
    uint16_t msgsize;
//    getMessage(&msgdata_ptr, &msgsize);
    msgdata_ptr = RecMsg;
    msgsize = RecMsgLen;
    if(msgsize == 0)
        return emotionId;



    for (i=0;i<=(msgsize);)
    {
      switch (msgdata_ptr[i])
      {
          case 'L':   //Left EyeBrows
          {
            emotionId = getHexVal(&msgdata_ptr[i+1]);
            partId=LEB;
            i=i+3;
            count_cmd++;
          }
          break;
          case 'I':   //SWITCH ON ALL THE LEDS
          {
            //TODO
            count_cmd++;
           
            i=i+1;
          }
          break;
          case 'M':   //Mouth
          {
            emotionId = getHexVal(&msgdata_ptr[i+1]);
            count_cmd++;
            partId=MOU;
            i=i+3;
          }
          break;
          case 'S':  //eyelids
          {
            emotionId = getHexVal(&msgdata_ptr[i+1]);
            partId=ELI;
            count_cmd++;
            i=i+3;
          }
          break;
          case 'R':  //Right EyeBrows
          {
            emotionId = getHexVal(&msgdata_ptr[i+1]);
            partId=REB;
            count_cmd++;
            i=i+3;
          }
          break;
          case 'Z':   //SWITCH OFF ALL THE LEDS
          {
            // TODO SWITCH ON ALL THE LEDS
             count_cmd++;
           
            i=i+1;
          }
          break;
          case 'X':   //cancel current command string
          {
            // TODO SWITCH ON ALL THE LEDS
            count_cmd++;
            i=i+1;
          }
          break;
          default:
          {
            //count_cmd++;
              i=i+1;
          }
          break;
      }
    }
    //debig
//    if(RecMsgLen>0)
//    {
//        snprintf(resp, sizeof(resp), "total msg= %s Len=%d E=%d P=%d cnt=%d\n",(char*)msgdata_ptr, msgsize, emotionId, partId, count_cmd);
//        while(USBD_BUSY==CDC_Transmit_FS((uint8_t*)resp, strlen(resp)));
//    }
    
//    for (i=0;i<(30);i++)
//    {
//      RecMsg[i]='\0'; //clean of the buffer
//    }
    
    return emotionId;
}






