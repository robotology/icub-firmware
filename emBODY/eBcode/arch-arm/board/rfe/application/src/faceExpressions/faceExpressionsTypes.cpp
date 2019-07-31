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

////FACE1
//const LedColor mouth_happy[5*4] = {green,green,white,green,black,green,white,green,green,white,white,green,green,green,white,green,black,green,white,green};
//const LedColor lBrow_happy[5*4] = {black,green,black,black,black,green,black,black,black,green,black,black,black,green,black,black,black,green,black,black};
//const LedColor rBrow_happy[5*4] = {black,green,black,black,black,green,black,black,black,green,black,black,black,green,black,black,black,green,black,black};
//    
////FACE2    
//const LedColor mouth_sad[5*4] = {black, red, red, red, black, red, black, red, red, black, black, red, black, red, red, red, black, red, black, red};
//const LedColor lBrow_sad[5*4] = {black,black,black,purple,black,black,black,purple,black,black,black,purple,black,black,black,purple,black,black,black,purple};
//const LedColor rBrow_sad[5*4] = {black,black,black,purple,black,black,black,purple,black,black,black,purple,black,black,black,purple,black,black,black,purple};

////FACE3
//const LedColor mouth_neutral[5*4] =  {black, red, red, red, black, red, black, red, red, black, black, red, black, red, red, red, black, red, black, red};
//const LedColor lBrow_neutral[5*4] =  {black,black,purple,black,black,black,purple,black,black,black,purple,black,black,black,purple,black,black,black,purple,black};
//const LedColor rBrow_neutral[5*4] =  {black,black,purple,black,black,black,purple,black,black,black,purple,black,black,black,purple,black,black,black,purple,black};


constexpr LedColor col2use = navy;







//test with colors
constexpr std::uint32_t numOfLedInApart = (static_cast<std::uint8_t>(HardwareConfig_numOf_t::ledsperTLC))*
                                          (static_cast<std::uint8_t>(HardwareConfig_numOf_t::TLCperPart));


//const LedColor mouth__[numOfLedInApart] = {DL01,DL02,DL03,DL04,DL05,DL06,DL07,DL08,DL09,DL10,DL11,DL12,DL13,DL14,DL15,DL16,DL17,DL18,DL19,DL20};
//const LedColor lBrow__[numOfLedInApart] = {DL01,DL02,DL03,DL04,DL05,DL06,DL07,DL08,DL09,DL10,DL11,DL12,DL13,DL14,DL15,DL16,DL17,DL18,DL19,DL20};
//const LedColor rBrow__[numOfLedInApart] = {DL01,DL02,DL03,DL04,DL05,DL06,DL07,DL08,DL09,DL10,DL11,DL12,DL13,DL14,DL15,DL16,DL17,DL18,DL19,DL20};

//eyebrwos
const LedColor eyebrow_1[numOfLedInApart] = {col2use,black,black,black,col2use,black,black,black,col2use,black,black,black,col2use,black,black,black,col2use,black,black,black};
const LedColor eyebrow_2[numOfLedInApart] = {black,col2use,black,black,black,col2use,black,black,black,col2use,black,black,black,col2use,black,black,black,col2use,black,black};
const LedColor eyebrow_3[numOfLedInApart] = {black,black,col2use,black,black,black,col2use,black,black,black,col2use,black,black,black,col2use,black,black,black,col2use,black};
const LedColor eyebrow_4[numOfLedInApart] = {black,black,black,col2use,black,black,black,col2use,black,black,black,col2use,black,black,black,col2use,black,black,black,col2use};

    
const LedColor mouth_happy[numOfLedInApart] = {col2use,black,col2use,black,black,black,col2use,black,black,col2use,col2use,black,col2use,black,col2use,black,black,black,col2use,black};
//const LedColor lBrow_happy[numOfLedInApart] = {black,black,col2use,black,black,black,col2use,black,black,black,col2use,black,black,black,col2use,black,black,black,col2use,black};
//const LedColor rBrow_happy[numOfLedInApart] = {black,black,col2use,black,black,black,col2use,black,black,black,col2use,black,black,black,col2use,black,black,black,col2use,black};
const LedColor *lBrow_happy = eyebrow_2;
const LedColor *rBrow_happy = eyebrow_2;

    
    
const LedColor mouth_sad[numOfLedInApart] = {black,black,black,black,col2use,col2use,col2use,black,black,col2use,col2use,black,black,black,black,black,col2use,col2use,col2use,black};
//const LedColor lBrow_sad[numOfLedInApart] = {black,black,col2use,black,black,black,col2use,black,black,black,col2use,black,black,black,col2use,black,black,black,col2use,black};
//const LedColor rBrow_sad[numOfLedInApart] = {black,black,col2use,black,black,black,col2use,black,black,black,col2use,black,black,black,col2use,black,black,black,col2use,black};
const LedColor *lBrow_sad = eyebrow_3;
const LedColor *rBrow_sad = eyebrow_3;   

    


//const LedColor mouth_surprised[numOfLedInApart] = {black,col2use,col2use,col2use,black,col2use,white,col2use,col2use,white,white,col2use,black,col2use,col2use,col2use,black,col2use,black,col2use};
const LedColor mouth_surprised[numOfLedInApart] = {black,col2use,col2use,col2use,black,col2use,black,col2use,col2use,black,black,col2use,black,col2use,col2use,col2use,black,col2use,black,col2use};
const LedColor *lBrow_surprised = eyebrow_1;
const LedColor *rBrow_surprised = eyebrow_1;

    
const LedColor mouth_angry[numOfLedInApart] = {black,black,black,black,col2use,col2use,col2use,black,black,col2use,col2use,black,black,black,black,black,col2use,col2use,col2use,black};
const LedColor *lBrow_angry = eyebrow_4;
const LedColor *rBrow_angry = eyebrow_4;

const LedColor mouth_neutral[numOfLedInApart] = {black,black,black,black,black,black,col2use,black,black,col2use,col2use,black,black,black,black,black,black,black,col2use,black};
const LedColor *lBrow_neutral = eyebrow_2;
const LedColor *rBrow_neutral = eyebrow_2;

const LedColor *mouth_evil = mouth_happy;
const LedColor *lBrow_evil = eyebrow_4;
const LedColor *rBrow_evil = eyebrow_4;

    
const LedColor *mouth_cunning = mouth_sad;
const LedColor *lBrow_cunning = eyebrow_2;
const LedColor *rBrow_cunning = eyebrow_4;
    
const LedColor *mouth_shy = mouth_sad;
const LedColor *lBrow_shy = eyebrow_2;
const LedColor *rBrow_shy = eyebrow_2;

//FACE hap green
    
//const LedColor mouth_happy[numOfLedInApart] = {green,green,white,green,black,green,white,green,green,white,white,green,green,green,white,green,black,green,white,green};
//const LedColor lBrow_happy[numOfLedInApart] = {black,green,black,black,black,green,black,black,black,green,black,black,black,green,black,black,black,green,black,black};
//const LedColor rBrow_happy[numOfLedInApart] = {black,green,black,black,black,green,black,black,black,green,black,black,black,green,black,black,black,green,black,black};
    
    
////red
//const LedColor mouth_angry[numOfLedInApart] = {red,red,white,red,black,red,white,red,red,white,white,red,red,red,white,red,black,red,white,red};
//const LedColor lBrow_angry[numOfLedInApart] = {black,red,black,black,black,red,black,black,black,red,black,black,black,red,black,black,black,red,black,black};
//const LedColor rBrow_angry[numOfLedInApart] = {black,red,black,black,black,red,black,black,black,red,black,black,black,red,black,black,black,red,black,black};

////blue
//const LedColor mouth_neutral[numOfLedInApart] = {blue,blue,white,blue,black,blue,white,blue,blue,white,white,blue,blue,blue,white,blue,black,blue,white,blue};
//const LedColor lBrow_neutral[numOfLedInApart] = {black,blue,black,black,black,blue,black,black,black,blue,black,black,black,blue,black,black,black,blue,black,black};
//const LedColor rBrow_neutral[numOfLedInApart] = {black,blue,black,black,black,blue,black,black,black,blue,black,black,black,blue,black,black,black,blue,black,black};

////yellow
//const LedColor mouth_surprised[numOfLedInApart] = {yellow,yellow,white,yellow,black,yellow,white,yellow,yellow,white,white,yellow,yellow,yellow,white,yellow,black,yellow,white,yellow};
//const LedColor lBrow_surprised[numOfLedInApart] = {black,yellow,black,black,black,yellow,black,black,black,yellow,black,black,black,yellow,black,black,black,yellow,black,black};
//const LedColor rBrow_surprised[numOfLedInApart] = {black,yellow,black,black,black,yellow,black,black,black,yellow,black,black,black,yellow,black,black,black,yellow,black,black};

////maroon
//const LedColor mouth_sad[numOfLedInApart] = {maroon,maroon,white,maroon,black,maroon,white,maroon,maroon,white,white,maroon,maroon,maroon,white,maroon,black,maroon,white,maroon};
//const LedColor lBrow_sad[numOfLedInApart] = {black,maroon,black,black,black,maroon,black,black,black,maroon,black,black,black,maroon,black,black,black,maroon,black,black};
//const LedColor rBrow_sad[numOfLedInApart] = {black,maroon,black,black,black,maroon,black,black,black,maroon,black,black,black,maroon,black,black,black,maroon,black,black};

////purple
//const LedColor mouth_evil[numOfLedInApart] = {purple,purple,white,purple,black,purple,white,purple,purple,white,white,purple,purple,purple,white,purple,black,purple,white,purple};
//const LedColor lBrow_evil[numOfLedInApart] = {black,purple,black,black,black,purple,black,black,black,purple,black,black,black,purple,black,black,black,purple,black,black};
//const LedColor rBrow_evil[numOfLedInApart] = {black,purple,black,black,black,purple,black,black,black,purple,black,black,black,purple,black,black,black,purple,black,black};

////col2use
//const LedColor mouth_shy[numOfLedInApart] = {col2use,col2use,white,col2use,black,col2use,white,col2use,col2use,white,white,col2use,col2use,col2use,white,col2use,black,col2use,white,col2use};
//const LedColor lBrow_shy[numOfLedInApart] = {black,col2use,black,black,black,col2use,black,black,black,col2use,black,black,black,col2use,black,black,black,col2use,black,black};
//const LedColor rBrow_shy[numOfLedInApart] = {black,col2use,black,black,black,col2use,black,black,black,col2use,black,black,black,col2use,black,black,black,col2use,black,black};

////olive
//const LedColor mouth_cunning[numOfLedInApart] = {olive,olive,white,olive,black,olive,white,olive,olive,white,white,olive,olive,olive,white,olive,black,olive,white,olive};
//const LedColor lBrow_cunning[numOfLedInApart] = {black,olive,black,black,black,olive,black,black,black,olive,black,black,black,olive,black,black,black,olive,black,black};
//const LedColor rBrow_cunning[numOfLedInApart] = {black,olive,black,black,black,olive,black,black,black,olive,black,black,black,olive,black,black,black,olive,black,black};
    
// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------
static LedColor const* loadMouthGift(Expression_t expression);
static LedColor const* loadLBrowGift(Expression_t expression);
static LedColor const* loadRBrowGift(Expression_t expression);

FacePartExpr_t::FacePartExpr_t(FacePart_t part):
                facepart(part), expression(Expression_t::neutral), changed(true), tlc_start(0)
{
    loadGift(Expression_t::neutral);
    switch (part)
    {
        case FacePart_t::leftEBrow: {tlc_start=10; break;}
        case FacePart_t::rightEBrow: {tlc_start=5; break;}
        case FacePart_t::mouth: {tlc_start=0; break;}
    };
};


        
bool FacePartExpr_t::loadGift(Expression_t expression)
{
    switch(facepart)
    {
        case FacePart_t::mouth: {expressionMask_ptr = loadMouthGift(expression); }break;
        
        case FacePart_t::leftEBrow: {expressionMask_ptr = loadLBrowGift(expression); }break;
        
        case FacePart_t::rightEBrow:{expressionMask_ptr = loadRBrowGift(expression); }break;

        default:
            expressionMask_ptr =  nullptr;
    
    }

    if(nullptr == expressionMask_ptr)
        return false;
    else
    {
        changed = true;
        return true;
    }    
}

static LedColor const * loadMouthGift(Expression_t expression)
{
    switch(expression)
    {
        case Expression_t::neutral:
            return &mouth_neutral[0];
        
        case Expression_t::happy:
            return &mouth_happy[0];
        
        case Expression_t::sad:
            return &mouth_sad[0];
        
        case Expression_t::surprised:
            return &mouth_surprised[0];
        
        case Expression_t::angry:
            return &mouth_angry[0];
        
        case Expression_t::evil:
            return &mouth_evil[0];

        case Expression_t::shy:
            return &mouth_shy[0];
        
        case Expression_t::cunning:
            return &mouth_cunning[0];

        default:
            return nullptr;
    
    };
}


static LedColor const * loadLBrowGift(Expression_t expression)
{
    switch(expression)
    {
        case Expression_t::neutral:
            return &lBrow_neutral[0];
        
        case Expression_t::happy:
            return &lBrow_happy[0];
        
        case Expression_t::sad:
            return &lBrow_sad[0];
        
        case Expression_t::surprised:
            return &lBrow_surprised[0];
        
        case Expression_t::angry:
            return &lBrow_angry[0];
        
        case Expression_t::evil:
            return &lBrow_evil[0];

        case Expression_t::shy:
            return &lBrow_shy[0];        

        case Expression_t::cunning:
            return &lBrow_cunning[0];

        default:
            return nullptr;
    
    };
}


static LedColor const * loadRBrowGift(Expression_t expression)
{
    switch(expression)
    {
        case Expression_t::neutral:
            return &rBrow_neutral[0];
        
        case Expression_t::happy:
            return &rBrow_happy[0];
        
        case Expression_t::sad:
            return &rBrow_sad[0];
        
        case Expression_t::surprised:
            return &rBrow_surprised[0];
        
        case Expression_t::angry:
            return &rBrow_angry[0];
        
        case Expression_t::evil:
            return &rBrow_evil[0];

        case Expression_t::shy:
            return &rBrow_shy[0];        

        case Expression_t::cunning:
            return &rBrow_cunning[0];

        default:
            return nullptr;
    
    };
}

