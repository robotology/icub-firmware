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

#ifndef _FACEEXPRESSIONSTYPES_H_
#define _FACEEXPRESSIONSTYPES_H_

#include <cstdint>

//#define _FaceExpression_numOfFaceParts  3
//#define _FaceExpression_numOfTLCPerPart 5
//#define _FaceExpression_numOfLedsPerTLC 3
//#define _FaceExpression_numOfColorPerLeds  3
namespace RfeApp{
    
    enum class FacePart_t {leftEBrow=0, rightEBrow=1, mouth=2, facePartMaxNum=3} ;
    enum class Expression_t {neutral=0, happy=1, sad=2, surprised=3, angry=4, evil=5, shy=6, cunning=7, expressionMaxNum=8} ;
    enum class FaceExpressionLL_numOf {faceParts=3, TLCperPart=5, LedsperTLC=4, ColorsPerLed=3, BytesPerColor=2} ;
    enum /*class*/ LedColor {   dontChange=0x000000, //I removed class in order semplify gift definition even if I lose strong type (see faceExpressionsGifts.cpp) 
                            black  =  0x000000,
                            white  =  0xFFFFFF,
                            red    =  0xFF0000,
                            lime   =  0x00FF00,
                            blue   =  0x0000FF,
                            yellow =  0xFFFF00,
                            cyan   =  0x00FFFF,
                            magenta=  0xFF00FF,
                            silver =  0xC0C0C0,
                            gray   =  0x808080,
                            maroon =  0x800000,
                            olive  =  0x808000,
                            green  =  0x008000,
                            purple =  0x800080,
                            teal   =  0x008080,
                            navy   =  0x000080,
                            test   =  0xFF0100};
    
    struct FacePartExpr_t
    {
        FacePart_t facepart;
        Expression_t expression;
        LedColor const *expressionMask_ptr;
        bool changed;
        std::uint8_t tlc_start;
        
        FacePartExpr_t(FacePart_t part);
        bool loadGift(Expression_t expression);   
    };

//struct FaceExpressionLL_numOf {// struct o define o le lascio in ll??
//    const uint8_t numOfFaceParts=3;
//    const uint8_t numOfTLCperPart=5;
//    const uint8_t numOfLedsperTLC=4;
//    const uint8_t numOfColorPerLeds=3; //RGB
//    const uint8_t numOfBytePerColors=2; //RGB
//};
//    const FaceExpressionLL_numOf pippo; 

};
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


