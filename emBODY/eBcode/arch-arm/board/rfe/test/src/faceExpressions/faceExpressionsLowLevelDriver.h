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
    
    /*NOTE: The face is composed by three part: mouth, LeftEyebrown, rightEyebrown. 
            Each part is composed by 5 TLC drivers and each of them pilots 4 leds. */
    enum class HardwareConfig_numOf_t : uint8_t {
                faceParts=3,      //The face is composed by : mouth, LeftEyebrown, rightEyebrown
                TLCperPart=5,     //Each face part has 5 TLC drivers.
                ledsperTLC=4,     //Each TLC pilots 4 leds
                colorsPerLed=3,   //Each les is RGB (3 colors)
                bytesPerColor=2 }; //Each Color is encode in 2 bytes

    /* A TLC packet is composed of heder(4 byte) plus data for describing color of each led.
       So each TLC packet payload is 4(leds) * 3(colors-rgb) *2 (byte) = 24. Therefore each TLC packet is 28=(24+4) bytes. */
    enum class TLCPacketInfo_t : uint8_t { headersize=4, totalsize= (embot::core::tointegral(HardwareConfig_numOf_t::ledsperTLC))*
                                                         (embot::core::tointegral(HardwareConfig_numOf_t::colorsPerLed))*
                                                         (embot::core::tointegral(HardwareConfig_numOf_t::bytesPerColor)) +
                                                         (static_cast<std::uint8_t>(TLCPacketInfo_t::headersize)) };
}; //end namespace RfeApp


class RfeApp::TLCDriver
{
    
private:
    
    struct TCL_headerPacket_t
    {
        std::uint8_t WRITE_CMD;
        std::uint8_t BLANK;
        std::uint8_t DSPRPT;
        std::uint8_t TMGRST;
        std::uint8_t EXTGCK;
        std::uint8_t OUTTMG;
        
        TCL_headerPacket_t(): WRITE_CMD(0x25), BLANK(0), DSPRPT(1), TMGRST(1), EXTGCK(0), OUTTMG(1) {;}
    };

    const TCL_headerPacket_t tlc_headerPkt; //the header config never change
    static const std::uint8_t GlobalBrightness_Max = 0x7F; // maximum raw value allowed
    //static const std::uint8_t GlobalBrightness_Default = 114; // it is equalt to 90% of max Brightness

    static const std::uint8_t GlobalBrightness_Default = 127;
    
public:
    struct Led_t
    {
      std::uint16_t R;    // red
      std::uint16_t G;    // green
      std::uint16_t B;    // blue
      
      Led_t():R(0),G(0),B(0){};
      void reset(void) {B=0; R=0; G=0;};
    };

    struct BrightnessLed_t
    {
        std::uint8_t R {tovalueofbrightness(Brightness::high)};    // red
        std::uint8_t G {tovalueofbrightness(Brightness::high)};    // green
        std::uint8_t B {tovalueofbrightness(Brightness::high)};    // blue
        
        void set(Brightness b) 
        {
            R = G = B = tovalueofbrightness(b);
        }
      
        BrightnessLed_t() = default;
        
        //BrightnessLed_t():R(GlobalBrightness_Default), G(GlobalBrightness_Default), B(GlobalBrightness_Default){};
    };

    TLCDriver();
    void reset(void);
    void setBrightness(Brightness b) { brightness4AllLeds.set(b); }
    void setColor(std::uint8_t ledNum, Color c);
    void createDataPacket(std::uint8_t *packet);

    private:
    Led_t leds[static_cast<std::uint32_t>(HardwareConfig_numOf_t::ledsperTLC)];
    BrightnessLed_t brightness4AllLeds {};
};



class RfeApp::FaceExpressionsLLDriver
{
public:
    FaceExpressionsLLDriver() {}
    void preparePacket(FacePartExpr_t &facepartexpr);
    bool sendStream(RfeApp::Error &err, embot::core::relTime timeout = 3*embot::core::time1millisec, int spi = 1);
    
    static void onspitxcompleted();
    
private:

    struct HardwareConfig_t
    {
        struct numOf_t
        {
            std::uint8_t FaceParts;     //the face is composed by : mouth, LeftEyebrown, rightEyebrown
            std::uint8_t TLCperPart;    //Each face part has 5 TLC drivers.
            std::uint8_t LedsperTLC;    //Each TLC pilots 4 leds
            std::uint8_t ColorsPerLed;  //Each les is RGB (3 colors)
            std::uint8_t BytesPerColor; //Each Color is encode in 2 bytes
            numOf_t(): 
            FaceParts     (static_cast<std::uint8_t>(HardwareConfig_numOf_t::faceParts)),
            TLCperPart    (static_cast<std::uint8_t>(HardwareConfig_numOf_t::TLCperPart)), 
            LedsperTLC    (static_cast<std::uint8_t>(HardwareConfig_numOf_t::ledsperTLC)), 
            ColorsPerLed  (static_cast<std::uint8_t>(HardwareConfig_numOf_t::colorsPerLed)), 
            BytesPerColor (static_cast<std::uint8_t>(HardwareConfig_numOf_t::bytesPerColor)){};
        };
        struct tlcStartIndexImGlobalStream_t
        {
            std::uint8_t lbrown;
            std::uint8_t rbrown;
            std::uint8_t mouth;
            tlcStartIndexImGlobalStream_t(): lbrown(10), rbrown(5), mouth(0){};
        };
        
        numOf_t numOf;
        tlcStartIndexImGlobalStream_t tlcStartIndexImGlobalStream;
        HardwareConfig_t():numOf(), tlcStartIndexImGlobalStream(){};
    };
    
    
    const HardwareConfig_t hwConfig;
    static constexpr std::uint32_t globalDataPacketSize = (static_cast<std::uint8_t>(HardwareConfig_numOf_t::faceParts))*
                                                   (static_cast<std::uint8_t>(HardwareConfig_numOf_t::TLCperPart))*
                                                   (static_cast<std::uint8_t>(TLCPacketInfo_t::totalsize));
    /*The global data size is equal to sum of all tlcpacket for all tlc for all parts.
    It is not possible to send only a part of data.*/
    std::uint8_t globalDataPacket[ globalDataPacketSize ];
                                                   
    std::uint8_t spitxData[globalDataPacketSize];   
    static volatile bool spitxISactive;                                                   

    TLCDriver tlcdriver;
};




#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


