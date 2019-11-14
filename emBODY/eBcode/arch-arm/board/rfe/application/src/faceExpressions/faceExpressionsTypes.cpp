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
#include "faceExpressionsTypes.h"

using namespace RfeApp;
using namespace std;
// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "faceExpressionsLowLevelDriver.h"

#include <array>
#include <type_traits>
#include <bitset>
#include "embot_binary.h"


constexpr std::uint32_t numOfLedInApart = (static_cast<std::uint8_t>(HardwareConfig_numOf_t::ledsperTLC))*
                                          (static_cast<std::uint8_t>(HardwareConfig_numOf_t::TLCperPart));

static_assert(numOfLedInApart == FacePartExpr_t::numofleds, "ahhhh ... 21st Century Schizoid Man");

//constexpr uint32_t mskGenerate(const std::initializer_list<uint8_t> &on)
//{
//    uint32_t ret = 0;
//    for(auto a : on)
//    {
//        embot::binary::bit::set(ret, a);
//    }
//    return ret;    
//}

//constexpr uint32_t a = mskGenerate({1, 5, 9, 13, 17});
//constexpr uint32_t b = embot::binary::mask::generate<uint32_t>({1, 5, 9, 13, 17});
//static_assert(a == b, "ahhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh");

constexpr std::bitset<FacePartExpr_t::numofleds> mskEyeBrowNeutral = embot::binary::mask::generate<uint32_t>({1, 5, 9, 13, 17});
constexpr std::bitset<FacePartExpr_t::numofleds> mskEyeBrow01 = embot::binary::mask::generate<uint32_t>({0, 4, 8, 12, 16});                           // surprised
constexpr std::bitset<FacePartExpr_t::numofleds> mskEyeBrow02 = embot::binary::mask::generate<uint32_t>({1, 5, 9, 13, 17});                           // neutral, happy, shy
constexpr std::bitset<FacePartExpr_t::numofleds> mskEyeBrow03 = embot::binary::mask::generate<uint32_t>({2, 6, 10, 14, 18});                          // sad
constexpr std::bitset<FacePartExpr_t::numofleds> mskEyeBrow04 = embot::binary::mask::generate<uint32_t>({3, 7, 11, 15, 19});                          // angry

constexpr std::bitset<FacePartExpr_t::numofleds> mskMouth00 = embot::binary::mask::generate<uint32_t>({6, 9, 10, 18});                                // neutral
constexpr std::bitset<FacePartExpr_t::numofleds> mskMouth01 = embot::binary::mask::generate<uint32_t>({0, 2, 6, 9, 10, 12, 14, 18});                  // happy, evil
constexpr std::bitset<FacePartExpr_t::numofleds> mskMouth02 = embot::binary::mask::generate<uint32_t>({4, 5, 6, 9, 10, 16, 17, 18});                  // sad, cunning, shy
constexpr std::bitset<FacePartExpr_t::numofleds> mskMouth03 = embot::binary::mask::generate<uint32_t>({1, 2, 3, 5, 7, 8, 11, 13, 14, 15, 17, 19});    // surprised
constexpr std::bitset<FacePartExpr_t::numofleds> mskMouth04 = embot::binary::mask::generate<uint32_t>({4, 5, 6, 9, 10, 16, 17, 18});                  // angry
    

constexpr std::array<std::bitset<FacePartExpr_t::numofleds>, facePartMaxNum> mskFACEneutral { mskEyeBrow02, mskEyeBrow02, mskMouth00 };
constexpr std::array<std::bitset<FacePartExpr_t::numofleds>, facePartMaxNum> mskFACEhappy { mskEyeBrow02, mskEyeBrow02, mskMouth01 };
constexpr std::array<std::bitset<FacePartExpr_t::numofleds>, facePartMaxNum> mskFACEsad { mskEyeBrow03, mskEyeBrow03, mskMouth02 };
constexpr std::array<std::bitset<FacePartExpr_t::numofleds>, facePartMaxNum> mskFACEsurprised { mskEyeBrow01, mskEyeBrow01, mskMouth03 };
constexpr std::array<std::bitset<FacePartExpr_t::numofleds>, facePartMaxNum> mskFACEangry { mskEyeBrow04, mskEyeBrow04, mskMouth04 };
constexpr std::array<std::bitset<FacePartExpr_t::numofleds>, facePartMaxNum> mskFACEevil { mskEyeBrow04, mskEyeBrow04, mskMouth01 };
constexpr std::array<std::bitset<FacePartExpr_t::numofleds>, facePartMaxNum> mskFACEshy { mskEyeBrow02, mskEyeBrow02, mskMouth02 };
constexpr std::array<std::bitset<FacePartExpr_t::numofleds>, facePartMaxNum> mskFACEcunning { mskEyeBrow02, mskEyeBrow04, mskMouth02 };

// this is THE container of pre-formed bitmaps for face expressions 
constexpr std::array<std::array<std::bitset<FacePartExpr_t::numofleds>, facePartMaxNum>, expressionMaxNum> faceMSKs 
{
   mskFACEneutral,
   mskFACEhappy,
   mskFACEsad,
   mskFACEsurprised,
   mskFACEangry,
   mskFACEevil,
   mskFACEshy,
   mskFACEcunning        
};

// and this is the method used to retrieve a mask

constexpr const std::bitset<FacePartExpr_t::numofleds> getMSK(FacePart_t part, Expression_t expr)
{
    if(Expression_t::none == expr)
    {
        return 0;
    }
    return faceMSKs[embot::common::tointegral(expr)][embot::common::tointegral(part)];
}


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


FacePartExpr_t::FacePartExpr_t(FacePart_t part):
                facepart(part), expression(Expression_t::neutral), changed(true), tlc_start(0)
{
    
    if((FacePart_t::none == part) || (FacePart_t::all == part))
    {
        part = FacePart_t::mouth;
    }
    if(Expression_t::none == expression)
    {
        expression = Expression_t::neutral;
    }
    
    load(expression);
    
    switch(part)
    {
        case FacePart_t::leftEBrow: 
        { 
            tlc_start=10; 
        } break;
        
        case FacePart_t::rightEBrow: 
        { 
            tlc_start=5; 
        } break;
        
        default:
        case FacePart_t::mouth: 
        { 
            tlc_start=0; 
        } break;
    };
};

bool FacePartExpr_t::load(Expression_t e)
{ 
    if(Expression_t::none == e)
    {
        return false;
    }
    
    expression = e;        
    //mask = faceMSKs[embot::common::tointegral(expression)][embot::common::tointegral(facepart)];    
    mask = getMSK(facepart, expression);
    changed = true;    
    return true;
}


bool FacePartExpr_t::load(uint32_t m)
{            
    mask = m;
    changed = true;    
    return true;
}

bool FacePartExpr_t::load(Color c)
{
    if(Color::none == c)
    {
        return false;
    }
        
    color = c;
    changed = true;    
    return true;    
}

bool FacePartExpr_t::load(Brightness b)
{
    if(Brightness::none == b)
    {
        return false;
    }
        
    brightness = b;
    changed = true;    
    return true;    
}

bool FacePartExpr_t::load(const PartProps &pr)
{
    if(!pr.isvalid())
    {
        return false;
    }
    load(pr.bri);
    load(pr.col);
    load(pr.msk);
    
    return true;
}
        




