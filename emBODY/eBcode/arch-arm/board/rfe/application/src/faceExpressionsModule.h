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

#ifndef _EMOTIONSMODULE_H_
#define _EMOTIONSMODULE_H_

#include "faceExpressionsTypes.h"
#include "faceExpressionsLowLevelDriver.h"

namespace RfeApp{
    class FaceExpressions;
};

class RfeApp::FaceExpressions
{
    public:
//    enum class FacePart_t {leftEBrow=0, rightEBrow=1, mouth=2, facePartMaxNum=3} ;
//    enum class Expression_t {neutral=0, happy=0, sad=1, surprised=2, angry=3, evil=4, shy=5, cunning=6, expressionMaxNum=7} ;
//    
//    struct FacePartExpr_t
//    {
//        FacePart_t facepart;
//        Expression_t expression;
//        uint32_t *expressionMask_ptr;
//        bool changed;
//        //todo: how to initialize??? to null or default??    
//    };
    
    FaceExpressions();
    //note: embot::hw::result_t instead of bool 
    bool init(void); //init hw and default face
    bool loadNewExpression(std::uint8_t *data, std::uint32_t size);
    bool displayExpression(void);
    
private:
    FacePartExpr_t leftEBrow_expr;
    FacePartExpr_t rightEBrow_expr;
    FacePartExpr_t mouth_expr;
    //FacePartExpr_t facePartExpr_array[static_cast<std::uint32_t>(FacePart_t::facePartMaxNum)];
    FaceExpressionsLLDriver driver;
    
    bool parse(std::uint8_t *data, std::uint32_t size, FacePart_t &part, Expression_t &expression);
    void getFace(void); //??utile        
};



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


