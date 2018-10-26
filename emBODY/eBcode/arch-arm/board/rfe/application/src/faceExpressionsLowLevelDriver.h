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

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _FACEEXPRESSIONSLLDRIVER_H_
#define _FACEEXPRESSIONSLLDRIVER_H_

#include "faceexpressionsTypes.h"
#include <cstdint>


namespace RfeApp{
    class FaceExpressionsLLDriver;
    class TLCDriver;
};


class RfeApp::TLCDriver
{
    
private:
    
    struct TCL_headerPacket_t
    {
        std::uint8_t WRITE_CMD;       // 6 bits, 0x25
        std::uint8_t BLANK;
        std::uint8_t DSPRPT;
        std::uint8_t TMGRST;
        std::uint8_t EXTGCK;
        std::uint8_t OUTTMG;
        
        TCL_headerPacket_t(): WRITE_CMD(0x25), BLANK(0), DSPRPT(1), TMGRST(1), EXTGCK(0), OUTTMG(1) {;}
    };

    const TCL_headerPacket_t tlc_headerPkt;
    static const std::uint16_t GlobalBrightness_Max = 0x7F; // maximum raw value allowed
    static const std::uint16_t GlobalBrightness_Default = 114; // it is equalt to 90% of max Brightness

public:
    struct Led_t
    {
      std::uint16_t B;    // blue
      std::uint16_t G;    // green
      std::uint16_t R;    // red
      
      Led_t():B(0),G(0),R(0){};
      void reset(void) {B=0; R=0; G=0;};
    };

    struct  BrightnessLed_t
    {
      std::uint8_t B;    // blue
      std::uint8_t G;    // green
      std::uint8_t R;    // red
        
      BrightnessLed_t():B(GlobalBrightness_Default),G(GlobalBrightness_Default),R(GlobalBrightness_Default){};
    };
    
    TLCDriver();
    void reset(void);
    void setColor(std::uint8_t ledNum/*todo:enum???*/, LedColor c);
    void createDataPacket(std::uint8_t *packet);

    private:
    Led_t leds[static_cast<std::uint32_t>(FaceExpressionLL_numOf::LedsperTLC)];
    BrightnessLed_t brightness4AllLeds;

    
};



class RfeApp::FaceExpressionsLLDriver
{
    public:
//    const uint8_t numOfFaceParts=3;
//    const uint8_t numOfTLCperPart=5;
//    const uint8_t numOfLedsperTLC=4;
//    const uint8_t numOfColorPerLeds=3; //RGB
//    const uint8_t numOfBytePerColors=2; //RGB
    FaceExpressionsLLDriver(){;};
    void preparePacket(FacePartExpr_t &facepartexpr);
    void sendStream(void);
    
    
private:
    /*NOTE: each TLC is configured with a packet composed of heder (4 byte) plus data for describing color of each led.
      So each TLC packet payload is 4(leds) * 3(colors-rgb) *2 (byte) = 24. Therefore each TLC packet is 28=(24+4) bytes.
      Each facepart has 5 TLC and we have 3 face part so in total: 3(part) * 5(TLC) * 28(TLC-packet-size)*/
    std::uint8_t globalDataPacket[420]; //todo: prova a mettere enum
    std::uint8_t globalStreamData[420]; //where data are put in inverse order
    TLCDriver    tlcdriver;
};




#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


