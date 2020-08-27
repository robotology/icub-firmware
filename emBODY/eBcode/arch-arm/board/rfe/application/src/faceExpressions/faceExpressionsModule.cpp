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

#include "faceExpressionsModule.h"
#include "stm32hal.h" // to init gpio 5 volt

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using namespace RfeApp;
using namespace std;
// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

bool PartProps::loadhex(uint8_t *stream, bool bigendianess)
{
    // format is cc|bb|mmmmmmmm    
    if(nullptr == stream)
    {
        return false;
    }
    char byte0[2+1] = {0};
    char byte1[2+1] = {0};
    char word0[8+1] = {0};
    std::memmove(byte0, &stream[0], 2);
    std::memmove(byte1, &stream[2], 2);
    if(bigendianess)
    {
        std::memmove(word0, &stream[4], 8);
    }
    else
    {
        for(int i=0; i<4; i++) { std::memmove(&word0[2*i], &stream[4+(3-i)*2], 2); }
    }
    
    long int b0 = strtol(byte0, nullptr, 16);
    long int b1 = strtol(byte1, nullptr, 16);
    long int w0 = strtol(word0, nullptr, 16);
    
    // at least one valid color or brigthness 
    
    Color c = tocolor(b0);
    Brightness b = tobrightness(b1);
    if((Color::none == c) || (Brightness::none == b))
    {
        return false;
    }
    
    bri = b;
    col = c;
    msk = w0;
       
    return true;    
}

    
FaceExpressions::FaceExpressions():
    leftEBrow_expr(FacePart_t::leftEBrow),
    rightEBrow_expr(FacePart_t::rightEBrow),
    mouth_expr(FacePart_t::mouth)
{;}    

bool FaceExpressions::initted() const
{
    return _initted;
}

bool FaceExpressions::init(Expression_t exp, Color col, Brightness bri)
{
    if(_initted)
    {
        return true;
    }
    
    HAL_GPIO_WritePin(EN_5V_GPIO_Port, EN_5V_Pin, GPIO_PIN_SET);      
    display(FacePart_t::all, exp, col, bri);    
    _initted = true;    
    return true;
} 


bool FaceExpressions::rotate(uint8_t cnt)
{
    Expression_t exp = static_cast<Expression_t>(cnt%expressionMaxNum);
    Color col = tocolor(cnt%12);
    Brightness bri = Brightness::medium;
    display(FacePart_t::all, exp, col, bri);//RfeApp::Color::red);   

    return true;    
}


bool FaceExpressions::display(Expression_t exp, Color col)
{
    leftEBrow_expr.load(exp);
    rightEBrow_expr.load(exp);
    mouth_expr.load(exp);    
    
    leftEBrow_expr.load(col);
    rightEBrow_expr.load(col);
    mouth_expr.load(col); 
    
    return displayExpression();   
}


bool FaceExpressions::display(Expression_t exp, const std::array<Color, 3> &cols, Brightness bri)
{
    leftEBrow_expr.load(exp);
    rightEBrow_expr.load(exp);
    mouth_expr.load(exp);  
    
    leftEBrow_expr.load(bri);
    rightEBrow_expr.load(bri);
    mouth_expr.load(bri);  

    leftEBrow_expr.load(cols[0]);
    rightEBrow_expr.load(cols[1]);
    mouth_expr.load(cols[2]);      

    return displayExpression();       
}


bool FaceExpressions::display(FacePart_t part, Expression_t exp, Color col, Brightness bri)
{
    switch(part)
    {
        default:
        case FacePart_t::all:
        {
            leftEBrow_expr.load(exp);
            rightEBrow_expr.load(exp);
            mouth_expr.load(exp);    
            
            leftEBrow_expr.load(col);
            rightEBrow_expr.load(col);
            mouth_expr.load(col);   

            leftEBrow_expr.load(bri);
            rightEBrow_expr.load(bri);
            mouth_expr.load(bri);             
        } break;
        
        case FacePart_t::mouth:
        {
            mouth_expr.load(exp);    
            mouth_expr.load(col);  
            mouth_expr.load(bri);             
        } break;        

        case FacePart_t::rightEBrow:
        {
            rightEBrow_expr.load(exp);
            rightEBrow_expr.load(col);         
            rightEBrow_expr.load(bri);
        } break;

        case FacePart_t::leftEBrow:
        {
            leftEBrow_expr.load(exp);
            leftEBrow_expr.load(col);         
            leftEBrow_expr.load(bri);  
        } break;
        
        case FacePart_t::none:
        {
            
        } break;
        
    }
       
    return displayExpression();   
}



bool FaceExpressions::display(FacePart_t part, Color col, Brightness bri, uint32_t picture)
{
    switch(part)
    {
        default:
        case FacePart_t::all:
        {
            leftEBrow_expr.load(picture);
            rightEBrow_expr.load(picture);
            mouth_expr.load(picture);    
            
            leftEBrow_expr.load(col);
            rightEBrow_expr.load(col);
            mouth_expr.load(col);  

            leftEBrow_expr.load(bri);
            rightEBrow_expr.load(bri);
            mouth_expr.load(bri);            
        } break;
        
        case FacePart_t::mouth:
        {
            mouth_expr.load(picture);    
            mouth_expr.load(col);    
            mouth_expr.load(bri);             
        } break;        

        case FacePart_t::rightEBrow:
        {
            rightEBrow_expr.load(picture);
            rightEBrow_expr.load(col);         
            rightEBrow_expr.load(bri); 
        } break;

        case FacePart_t::leftEBrow:
        {
            leftEBrow_expr.load(picture);
            leftEBrow_expr.load(col); 
            leftEBrow_expr.load(bri);             
        } break;
        
        case FacePart_t::none:
        {
            
        } break;
        
    }
       
    return displayExpression();   
}

bool FaceExpressions::display(FacePart_t part, const PartProps &pp)
{   
    if(!pp.isvalid())
    {
        _error = Error::invalidFacepart;
        return false;
    }
    
    switch(part)
    {
        default:
        case FacePart_t::all:
        {
            leftEBrow_expr.load(pp);
            rightEBrow_expr.load(pp);
            mouth_expr.load(pp);             
        } break;
        
        case FacePart_t::mouth:
        {
            mouth_expr.load(pp);               
        } break;        

        case FacePart_t::rightEBrow:
        {
            rightEBrow_expr.load(pp);
        } break;

        case FacePart_t::leftEBrow:
        {
            leftEBrow_expr.load(pp);          
        } break;
        
        case FacePart_t::none:
        {
            
        } break;
        
    }    
    
    return displayExpression(); 
}


bool FaceExpressions::loadNewExpression(uint8_t *data, uint32_t size)
{
    FacePart_t part {FacePart_t::leftEBrow};
    Expression_t expression {Expression_t::neutral};
    
    bool res = parse(data, size, part, expression);
    if(!res)
        return res;
    
    switch(part)
    {
        case FacePart_t::leftEBrow: {res = leftEBrow_expr.load(expression); break;}
        case FacePart_t::rightEBrow: {res = rightEBrow_expr.load(expression); break;}
        case FacePart_t::mouth: {res = mouth_expr.load(expression); break;}
        default: 
        {
            res = false;
        } break;
    };
    
    return res;    
}



bool FaceExpressions::displayExpression(void)
{
    bool ret = true;
    
    bool somethingIsChanged = false;
    
    if(leftEBrow_expr.changed)
        driver.preparePacket(leftEBrow_expr);
    if(rightEBrow_expr.changed)
        driver.preparePacket(rightEBrow_expr);
    if(mouth_expr.changed)
        driver.preparePacket(mouth_expr);
    
    somethingIsChanged |= leftEBrow_expr.changed | rightEBrow_expr.changed | mouth_expr.changed;
    
    if(somethingIsChanged)
    {
        ret = driver.sendStream(_error);
        leftEBrow_expr.changed = false;
        rightEBrow_expr.changed = false;
        mouth_expr.changed = false;
    }
    
    return ret;
}

static uint32_t getHexVal(uint8_t *recMsg)
{
    char *end = nullptr;
    return strtol((char*)recMsg, &end, 16);
}

/*FaceExpressions::parse parses a single command and return true if it recognizes it else false.
TODO: this function can parses more commands and put the reult in an array of pair (part, expression).
The code to pare more commands already exists, I need only to pass an array as in-out parametr and manage them outside*/
bool FaceExpressions::parse(uint8_t *data, uint32_t size, FacePart_t &part, Expression_t &expression)
{ 
    uint8_t i=0;
//    char resp[80];
    uint32_t emotionId=0;
    static uint32_t count_cmd=0;
    bool res = false;
    if(size == 0)
        return false;

    uint32_t tmp = 0;

    for (i=0;i<=(size);)
    {
      switch (data[i])
      {
          case 'C':   // color
          {
            tmp = getHexVal(&data[i+1]);
            i=i+3;
            count_cmd++;
            res = true;
          }
          
          case 'L':   //Left EyeBrows
          {
            emotionId = getHexVal(&data[i+1]);
            expression = static_cast<Expression_t>(emotionId);  
            part=FacePart_t::leftEBrow;
            i=i+3;
            count_cmd++;
            res = true;
          }
          break;
          case 'I':   //SWITCH ON ALL THE LEDS
          {
            //TODO
            count_cmd++;
            res = false;
            i=i+1;
          }
          break;
          case 'M':   //Mouth
          {
            emotionId = getHexVal(&data[i+1]);
            expression = static_cast<Expression_t>(emotionId);  
            part=FacePart_t::mouth;
            count_cmd++;
            res = true;
            i=i+3;
          }
          break;
          case 'S':  //eyelids
          {
            //skip because we haven't eyelids
            res = false;
            count_cmd++;
            i=i+3;
            
          }
          break;
          case 'R':  //Right EyeBrows
          {
            emotionId = getHexVal(&data[i+1]);
            expression = static_cast<Expression_t>(emotionId);  
            part=FacePart_t::rightEBrow;
            count_cmd++;
            i=i+3;
            res = true;
          }
          break;
          case 'Z':   //SWITCH OFF ALL THE LEDS
          {
            // TODO SWITCH ON ALL THE LEDS
             count_cmd++;
           res = false;
            i=i+1;
          }
          break;
          case 'X':   //cancel current command string
          {
            // TODO SWITCH ON ALL THE LEDS
            count_cmd++;
            i=i+1;
            res=false;
          }
          break;
          default:
          {
            //count_cmd++;
              i=i+1;
          }
          break;
      }
    }
    //debig
//    if(RecMsgLen>0)
//    {
//        snprintf(resp, sizeof(resp), "total msg= %s Len=%d E=%d P=%d cnt=%d\n",(char*)data, msgsize, emotionId, partId, count_cmd);
//        while(USBD_BUSY==CDC_Transmit_FS((uint8_t*)resp, strlen(resp)));
//    }
    
//    for (i=0;i<(30);i++)
//    {
//      RecMsg[i]='\0'; //clean of the buffer
//    }


return res;

}


bool FaceExpressions::processcommands(uint8_t *data, uint32_t size, bool bigendianess)
{
    if((nullptr == data) || (0 == size))
    {
        return false;
    }
    
    //FacePart_t p {FacePart_t::none};
    Expression_t e {Expression_t::none};
    Color c {Color::none};
    Brightness b {Brightness::none};
    bool res {false};
    uint32_t tmp = 0;

    // it was: for (i=0;i<=(size);) .... why <= ???
    for(uint32_t i=0; i<size; /*removed*/)
    {
        uint8_t cmd = data[i];
        switch(cmd)
        {            
            case 'E':   // all parts have this expression E {e}
            {
                uint8_t byte[3] = {0};
                byte[0] = data[i+1];
                byte[1] = data[i+2];                
                tmp = getHexVal(byte);
                i=i+3; 
                e = toexpression(tmp);
                
                if(Expression_t::none != e)
                {   // this expression is for every facepart ...
                    leftEBrow_expr.load(e);  
                    rightEBrow_expr.load(e);
                    mouth_expr.load(e);
                    res = true;            
                }               
            } break;   
            
            case 'C':   // color: C {c}
            {
                uint8_t byte[3] = {0};
                byte[0] = data[i+1];
                byte[1] = data[i+2];                
                tmp = getHexVal(byte);
                i=i+3;                                               
                c = tocolor(tmp);
                
                if(Color::none != c)
                {   // the color command is for every facepart ...
                    leftEBrow_expr.load(c);
                    rightEBrow_expr.load(c);
                    mouth_expr.load(c);
                    res = true;  
                }   
            } break;
            
            case 'B':   // brightness: B {b}
            {
                uint8_t byte[3] = {0};
                byte[0] = data[i+1];
                byte[1] = data[i+2];                
                tmp = getHexVal(byte);
                i=i+3;                                             
                b = tobrightness(tmp);
                
                if(Brightness::none != b)
                {   // the brigthness command is for every facepart ...
                    leftEBrow_expr.load(b);
                    rightEBrow_expr.load(b);
                    mouth_expr.load(b);
                    res = true;  
                }   
            } break;            
                     
          
            case 'L':   // expression for Left EyeBrow: L {e}
            {
                uint8_t byte[3] = {0};
                byte[0] = data[i+1];
                byte[1] = data[i+2];                
                tmp = getHexVal(byte);
                i=i+3; 
                e = toexpression(tmp);
                
                if(Expression_t::none != e)
                {   // this expression is only for the left eyebrow
                    leftEBrow_expr.load(e);  
                    res = true;            
                }               
            } break;

            case 'R':   // expression for right EyeBrow: R {e}
            {
                uint8_t byte[3] = {0};
                byte[0] = data[i+1];
                byte[1] = data[i+2];                
                tmp = getHexVal(byte);
                i=i+3; 
                e = toexpression(tmp);
                
                if(Expression_t::none != e)
                {   // this expression is only for the right eyebrow
                    rightEBrow_expr.load(e);  
                    res = true;            
                }               
            } break;
            
            case 'M':   // expression for mouth: M {e}
            {
                uint8_t byte[3] = {0};
                byte[0] = data[i+1];
                byte[1] = data[i+2];                
                tmp = getHexVal(byte);
                i=i+3; 
                e = toexpression(tmp);
                
                if(Expression_t::none != e)
                {   // this expression is only for the mouth
                    mouth_expr.load(e);  
                    res = true;            
                }               
            } break;
            
            
            case 'l':   // multiple for left: l {e, c, b} 
            case 'm':   // multiple for mouth: m {e, c, b} 
            case 'r':   // multiple for right: r {e, c, b} 
            case 'a':   // multiple for all: a {e, c, b} 
            {
                uint8_t byte[3] = {0};
                byte[0] = data[i+1];
                byte[1] = data[i+2];                
                tmp = getHexVal(byte); 
                e = toexpression(tmp);
                
                byte[0] = data[i+3];
                byte[1] = data[i+4];                
                tmp = getHexVal(byte); 
                c = tocolor(tmp);
                
                byte[0] = data[i+5];
                byte[1] = data[i+6];                
                tmp = getHexVal(byte); 
                b = tobrightness(tmp);
                
                i=i+7;
                
                res =  true;
                
                if(('a' == cmd) || ('l' == cmd))
                {
                    leftEBrow_expr.load(e); 
                    leftEBrow_expr.load(c);  
                    leftEBrow_expr.load(b);                    
                }
                if(('a' == cmd) || ('r' == cmd))
                {
                    rightEBrow_expr.load(e); 
                    rightEBrow_expr.load(c);  
                    rightEBrow_expr.load(b);                    
                }
                if(('a' == cmd) || ('m' == cmd))
                {
                    mouth_expr.load(e); 
                    mouth_expr.load(c);  
                    mouth_expr.load(b);                    
                }                                
                
            } break;  
            
            case 'Z':   // the ultimate config by all three parts: Z, {c, b, mask}, {c, b, mask},  {c, b, mask}
            {
                PartProps pr {};
                pr.loadhex(&data[i+1], bigendianess);
                if(pr.isvalid())
                {
                    leftEBrow_expr.load(pr);  
                }
                pr.clear();
                pr.loadhex(&data[i+1+12], bigendianess);
                if(pr.isvalid())
                {
                    rightEBrow_expr.load(pr);
                }                
                pr.clear();
                pr.loadhex(&data[i+1+12+12], bigendianess);
                if(pr.isvalid())
                {
                    mouth_expr.load(pr);
                }
                i = i+1+12+12+12;
        
            } break;            
            
            default: // i dont have a valid character, hence i advance one by one ... 
            {
                i=i+1;
            } break;
      }
    }
    
    return res;    
}


RfeApp::Error FaceExpressions::getError(bool clear_after_read)
{
    Error e = _error;
    if(true == clear_after_read)
    {
        _error = Error::none;
    }
    return e;
}    
   
// eof

