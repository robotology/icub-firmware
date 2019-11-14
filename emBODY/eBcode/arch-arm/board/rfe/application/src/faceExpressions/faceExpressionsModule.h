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

#include <array>

namespace RfeApp{
    class FaceExpressions;
};

class RfeApp::FaceExpressions
{
    public:
    
    FaceExpressions();
    bool init(Expression_t exp = Expression_t::neutral, Color col = Color::white, Brightness bri = Brightness::medium); //init hw and default face
    bool initted() const;
    bool loadNewExpression(std::uint8_t *data, std::uint32_t size);
    bool processcommands(std::uint8_t *data, std::uint32_t size, bool bigendianess = true);
    bool displayExpression(void);
    bool display(Expression_t exp, Color col);
    bool display(FacePart_t part, Expression_t exp, Color col, Brightness bri);   
    bool rotate(uint8_t cnt);    
    bool display(FacePart_t part, Color col, Brightness bri, uint32_t picture);
    bool display(FacePart_t part, const PartProps &pp);
    bool display(Expression_t exp, const std::array<Color, 3> &cols, Brightness bri);
    
private:
    FacePartExpr_t leftEBrow_expr {};
    FacePartExpr_t rightEBrow_expr {};
    FacePartExpr_t mouth_expr {};
    FaceExpressionsLLDriver driver {};
    bool _initted {false};
    bool parse(std::uint8_t *data, std::uint32_t size, FacePart_t &part, Expression_t &expression);
};



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


