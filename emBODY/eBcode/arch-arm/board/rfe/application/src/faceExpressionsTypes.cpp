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
//extern const LedColor mouth_happy[5*4]; 
//extern const LedColor lBrow_happy[5*4];
//extern const LedColor rBrow_happy[5*4]; 
//    
//    
//extern const LedColor mouth_sad[5*4];
//extern const LedColor lBrow_sad[5*4];
//extern const LedColor rBrow_sad[5*4];

//extern const LedColor mouth_neutral[5*4];
//extern const LedColor lBrow_neutral[5*4];
//extern const LedColor rBrow_neutral[5*4];



//FACE1
const LedColor mouth_happy[5*4] = {green,green,white,green,black,green,white,green,green,white,white,green,green,green,white,green,black,green,white,green};
const LedColor lBrow_happy[5*4] = {black,green,black,black,black,green,black,black,black,green,black,black,black,green,black,black,black,green,black,black};
const LedColor rBrow_happy[5*4] = {black,green,black,black,black,green,black,black,black,green,black,black,black,green,black,black,black,green,black,black};
    
//FACE2    
const LedColor mouth_sad[5*4] = {black, red, red, red, black, red, black, red, red, black, black, red, black, red, red, red, black, red, black, red};
const LedColor lBrow_sad[5*4] = {black,black,black,purple,black,black,black,purple,black,black,black,purple,black,black,black,purple,black,black,black,purple};
const LedColor rBrow_sad[5*4] = {black,black,black,purple,black,black,black,purple,black,black,black,purple,black,black,black,purple,black,black,black,purple};

//FACE3
const LedColor mouth_neutral[5*4] =  {black, red, red, red, black, red, black, red, red, black, black, red, black, red, red, red, black, red, black, red};
const LedColor lBrow_neutral[5*4] =  {black,black,purple,black,black,black,purple,black,black,black,purple,black,black,black,purple,black,black,black,purple,black};
const LedColor rBrow_neutral[5*4] =  {black,black,purple,black,black,black,purple,black,black,black,purple,black,black,black,purple,black,black,black,purple,black};



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
        default:
            return nullptr;
    
    };
}

