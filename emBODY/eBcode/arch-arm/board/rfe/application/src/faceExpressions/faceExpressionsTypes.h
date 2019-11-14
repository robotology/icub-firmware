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

#include <bitset>

#include "embot_common.h"

namespace RfeApp {
    
    // use consecutive values 
    enum class FacePart_t : uint8_t { leftEBrow = 0, rightEBrow = 1, mouth = 2, last = mouth, all = 254, none = 255 };
    constexpr uint8_t facePartMaxNum = embot::common::tointegral(FacePart_t::last) + 1;
    
    // use consecutive values 
    enum class Expression_t : uint8_t { neutral = 0, happy = 1, sad = 2, surprised = 3, angry = 4, evil = 5, shy = 6, cunning = 7, last = cunning, none = 255 };
    constexpr uint8_t expressionMaxNum = embot::common::tointegral(Expression_t::last) + 1; 
    
    // use consecutive values        
    enum class Color : uint8_t { black = 0, white, red, lime,  blue, yellow, cyan, magenta, silver, gray, maroon, olive, green, purple, teal, navy, last = navy, none = 255 };
    constexpr uint8_t colormaxNum = embot::common::tointegral(Color::last) + 1;
        
    // use consecutive values 
    enum class Brightness : uint8_t { dark = 0, minimum, low, medium, high, maximum, last = maximum, none = 255 };
    constexpr uint8_t brightnessMaxNum = embot::common::tointegral(Brightness::last) + 1; 
        
    constexpr Expression_t toexpression(const uint32_t e)
    {
        return (e>embot::common::tointegral(Expression_t::last)) ? Expression_t::none : static_cast<Expression_t>(e);        
    }
                            
    constexpr Color tocolor(const uint32_t c)
    {
        return (c>embot::common::tointegral(Color::last)) ? Color::none : static_cast<Color>(c);
    }
    
    constexpr uint32_t tovalueofcolor(Color c)
    {
        constexpr uint32_t _map2value[] = { 
            0x000000, 0xFFFFFF, 0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00, 0x00FFFF, 0xFF00FF, 
            0xC0C0C0, 0x808080, 0x800000, 0x808000, 0x008000, 0x800080, 0x008080, 0x000080           
        };
        return (c == Color::none) ? 0x000000 : _map2value[embot::common::tointegral(c)];        
    }
    

    constexpr Brightness tobrightness(const uint32_t b)
    {
        return (b>embot::common::tointegral(Brightness::last)) ? Brightness::none : static_cast<Brightness>(b);
    }
    
    constexpr uint8_t tovalueofbrightness(Brightness b)
    {
        constexpr uint8_t _map2v[] = { 0, 4, 16, 64, 96, 127 };
        return (b == Brightness::none) ? 0 : _map2v[embot::common::tointegral(b)];        
    }    
    
    
    struct PartProps
    {
        Color col {Color::none};
        Brightness bri {Brightness::none};
        uint32_t msk {0};
        bool loadhex(uint8_t *stream, bool bigendianess); // in 12 characters: cc|bb|mmmmmmmm
        bool clear() { col = Color::none; bri = Brightness::none; return true; }
        bool isvalid() const { return ((Color::none != col) || (Brightness::none != bri)); }
        PartProps() = default;     
        PartProps(Color c, Brightness b, uint32_t m) : col(c), bri(b), msk(m) {}        
    };
    
    struct FacePartExpr_t
    {
        constexpr static uint8_t numofleds = 20;

        bool changed {true};
        std::uint8_t tlc_start {0};
        
        FacePartExpr_t() = default;
        
        FacePartExpr_t(FacePart_t p);

        bool load(Expression_t e); 
        bool load(Color c); 
        bool load(Brightness b); 
        bool load(uint32_t m); 
        bool load(const PartProps &pr);
        
        Color getcolor(uint8_t tlc, uint8_t led) const
        {
            return mask[led+4*tlc] ? color : Color::black;
        }  

        Brightness getbrightness() const { return brightness; }

    private:
        std::bitset<numofleds> mask {0};
        Color color {Color::white};
        Brightness brightness {Brightness::medium};
        FacePart_t facepart {FacePart_t::leftEBrow};
        Expression_t expression {Expression_t::neutral};            
    };
};
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


