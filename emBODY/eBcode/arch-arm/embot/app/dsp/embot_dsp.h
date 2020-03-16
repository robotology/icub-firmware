
/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
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

#ifndef _EMBOT_DSP_H_
#define _EMBOT_DSP_H_

#include "embot_core.h"

namespace embot { namespace dsp {
        
    using Q15 = std::int16_t;
    using Q15result = std::int32_t;
   
} }  // namespace embot { namespace dsp { 

namespace embot { namespace dsp { namespace q15 {
 
    const Q15 negOne = 0x8000;
    const Q15 negOneNearly = 0x8001;    // -1+2^(-15) = -0.999969482421875
    const Q15 negOneHalf = 0xC000;      // -1+2^(-1) = -1.00+0.50 = -0.50
    const Q15 negOneFourth = 0xE000;    // -1+2^(-1)+2^(-2) = -1.00+0.50+0.25 = -0.25
    const Q15 negOneEigth = 0xF000;     // -1+2^(-1)+2^(-2)+2^(-3) = -1.00+0.50+0.25+0.125 = -0.125    
    const Q15 negEPSILON = 0xFFFF;      // -1+sum_(i=-1,..,-15)(2^i) = -0.000030517578125  
    const Q15 zero = 0;
    const Q15 posEPSILON = 0x0001;      // 2^(-15) = 0.000030517578125
    const Q15 posOneHalf = 0x4000;      // 2^(-1) = 0.5   
    const Q15 posOneFourth = 0x2000;    // 2^(-2) = 0.25
    const Q15 posOneEigth = 0x1000;     // 2^(-3) = 0.125
    const Q15 posOneNearly = 0x7FFF;    // sum_(i=-1,..,-15)(2^i) = 1-2^(-15) = 0.999969482421875
       
    Q15 convert(const double v, bool &saturated);
    double convert(const Q15 v);
    
    Q15 U16toQ15(const std::uint16_t valU16); // transforms a value in range [0, 64k-1] into a Q15
    std::uint16_t Q15toU16(const Q15 valQ15); // transforms a Q15 into range [0, 64k-1]    
    
    Q15 opposite(const Q15 v);  // opposite of negOne is posOneNearly
    Q15 saturate(const Q15result x, bool &saturated);
    Q15 add(const Q15 a, const Q15 b);
    Q15 add(const Q15 a, const Q15 b, bool &saturated); 
    Q15 sub(const Q15 a, const Q15 b);
    Q15 mul(const Q15 a, const Q15 b);
    Q15 mul(const Q15 a, const Q15 b, bool &saturated); 
    Q15 div(const Q15 a, const Q15 b, bool &saturated);
    

    struct matrix
    {
        std::uint8_t    nrows;
        std::uint8_t    ncols;
        Q15*            data;   // organised by row   
        
        void load(std::uint8_t r, std::uint8_t c, Q15* d) { nrows = r; ncols = c; data = d; }   
        matrix() { load(0, 0, nullptr); }  
        matrix(std::uint8_t r, std::uint8_t c, Q15* d) { load(r, c, d); }
        Q15 get(std::uint8_t r, std::uint8_t c) { if((r<nrows) && (c<ncols) && (nullptr != data)) { return data[c + r*ncols]; } else { return 0; } } 
        void set(std::uint8_t r, std::uint8_t c, Q15 v) { if((r<nrows) && (c<ncols) && (nullptr != data)) { data[c + r*ncols] = v; }  }
        void clear() {  if(nullptr != data) { std::memset(data, 0, sizeof(Q15)*ncols*nrows); }  } 
        void diagonal(Q15 v) { clear(); if(nullptr != data) { std::uint8_t min = (ncols<nrows) ? (ncols) : (nrows); for(int i=0; i<min; i++) data[i+i*ncols] = v; } }
        void fill(Q15 v) { clear(); if(nullptr != data) { for(int r=0; r<nrows; r++) for(int c=0; c<ncols; c++) data[c+r*ncols] = v; } }
    };
    
    bool multiply(const matrix &m1, const matrix &m2, matrix &res, bool &saturated);
    bool add(const matrix &m1, const matrix &m2, matrix &res, bool &saturated);
   
} } }  // namespace embot { namespace dsp { namespace q15 {

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
