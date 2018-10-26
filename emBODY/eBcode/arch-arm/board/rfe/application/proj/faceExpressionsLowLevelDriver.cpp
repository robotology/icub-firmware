/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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
// - public interface
// --------------------------------------------------------------------------------------------------------------------
#include "faceExpressionsLowLevelDriver.h"

using namespace RfeApp;
using namespace std;
// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "stm32l4xx_hal.h"
#include "spi.h"

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------
//FaceExpressionsLLDriver::FaceExpressionsLLDriver(void)
//{;}    

void FaceExpressionsLLDriver::preparePacket(FacePartExpr_t &facepartexpr)
{
    for(uint8_t nr_tlc=0; nr_tlc<5/*TODO*/; nr_tlc++)
    {
        tlcdriver.reset();
        for(uint8_t nr_led=0; nr_led<4; nr_led++) // single TLC scan (4 LEDs)
        {                          
            tlcdriver.setColor(nr_led,facepartexpr.expressionMask_ptr[nr_led+4*(nr_tlc)]);
        }
        volatile uint16_t tlc_start_index = (facepartexpr.tlc_start+5/*num of tlc per part*/-1-nr_tlc)*28/*tlc packet size*/;
        tlcdriver.createDataPacket(&globalDataPacket[tlc_start_index]);
        
    }

}
void FaceExpressionsLLDriver::sendStream(void)
{
    HAL_SPI_Transmit_DMA(&hspi1, globalDataPacket, 420/*todo*/);
}

TLCDriver::TLCDriver()
{;}

void TLCDriver::reset(void)
{
    for(uint8_t i=0; i<static_cast<uint8_t>(FaceExpressionLL_numOf::LedsperTLC); i++)
        leds[i].reset();
}

void TLCDriver::setColor(uint8_t ledNum, LedColor c)
{
    uint8_t ledNum_rev = 3-ledNum; //I need to revert the order of led. //todo: improve comment and use enum
    uint32_t RED, GREEN, BLUE;
    RED   = ((c & 0xFF0000)>>0x10)*0x101;
    GREEN = ((c & 0x00FF00)>>0x08)*0x101;
    BLUE  = ((c & 0x0000FF))*0x101;

    leds[ledNum_rev].R = (uint16_t)RED;
    leds[ledNum_rev].G = (uint16_t)GREEN;
    leds[ledNum_rev].B = (uint16_t)BLUE;
}

void TLCDriver::createDataPacket(uint8_t *packet)
{
  uint8_t FUNCTION_CTRL=(tlc_headerPkt.OUTTMG<<4) + (tlc_headerPkt.EXTGCK<<3) + (tlc_headerPkt.TMGRST<<2) + (tlc_headerPkt.DSPRPT<<1) + tlc_headerPkt.BLANK;
  
  packet[ 0] = (tlc_headerPkt.WRITE_CMD<<2)  + (FUNCTION_CTRL>>3);
  packet[ 1] = (FUNCTION_CTRL<<5) + (brightness4AllLeds.B>>2);
  packet[ 2] = (brightness4AllLeds.B<<6)  + (brightness4AllLeds.G>>1);
  packet[ 3] = (brightness4AllLeds.G<<7)  + (brightness4AllLeds.R);
  
  packet[ 4] = leds[3].B>>8;
  packet[ 5] = leds[3].B;
  packet[ 6] = leds[3].G>>8;
  packet[ 7] = leds[3].G;
  packet[ 8] = leds[3].R>>8;
  packet[ 9] = leds[3].R;
  
  packet[10] = leds[2].B>>8;
  packet[11] = leds[2].B;
  packet[12] = leds[2].G>>8;
  packet[13] = leds[2].G;
  packet[14] = leds[2].R>>8;
  packet[15] = leds[2].R;
  
  packet[16] = leds[1].B>>8;
  packet[17] = leds[1].B;
  packet[18] = leds[1].G>>8;
  packet[19] = leds[1].G;
  packet[20] = leds[1].R>>8;
  packet[21] = leds[1].R;
  
  packet[22] = leds[0].B>>8;
  packet[23] = leds[0].B;
  packet[24] = leds[0].G>>8;
  packet[25] = leds[0].G;
  packet[26] = leds[0].R>>8;
  packet[27] = leds[0].R;
    
}
