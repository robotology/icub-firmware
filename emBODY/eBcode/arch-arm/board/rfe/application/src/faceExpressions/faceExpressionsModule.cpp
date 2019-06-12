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
FaceExpressions::FaceExpressions():
    leftEBrow_expr(FacePart_t::leftEBrow),
    rightEBrow_expr(FacePart_t::rightEBrow),
    mouth_expr(FacePart_t::mouth)
{;}    

bool FaceExpressions::init(void)
{
    HAL_GPIO_WritePin(EN_5V_GPIO_Port, EN_5V_Pin, GPIO_PIN_SET);
    leftEBrow_expr.loadGift(Expression_t::neutral);
    rightEBrow_expr.loadGift(Expression_t::neutral); 
    mouth_expr.loadGift(Expression_t::neutral); 
    displayExpression();
    return true;
} 
bool FaceExpressions::loadNewExpression(uint8_t *data, uint32_t size)
{
    FacePart_t part;
    Expression_t expression;
    
    bool res = parse(data, size, part, expression);
    if(!res)
        return res;
    
    switch(part)
    {
        case FacePart_t::leftEBrow: {res = leftEBrow_expr.loadGift(expression); break;}
        case FacePart_t::rightEBrow: {res = rightEBrow_expr.loadGift(expression); break;}
        case FacePart_t::mouth: {res = mouth_expr.loadGift(expression); break;}
        default: return false;
    };
    return res;
    
   // res = facePartExpr_array[part].loadGift(expression);
}
bool FaceExpressions::displayExpression(void)
{
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
        driver.sendStream();
        leftEBrow_expr.changed = false;
        rightEBrow_expr.changed = false;
        mouth_expr.changed = false;
    }
    

    return true;
}

static uint32_t getHexVal(uint8_t *recMsg)
{
    char *end;
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



    for (i=0;i<=(size);)
    {
      switch (data[i])
      {
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

