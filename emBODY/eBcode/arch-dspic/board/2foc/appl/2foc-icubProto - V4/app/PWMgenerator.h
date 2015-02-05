/* 
 * File:   PWMgenerator.h
 * Author: Alessandro Scalzo
 *
 * Created on 29 novembre 2014, 10.36
 */

#ifndef PWMGENERATOR_H
#define	PWMGENERATOR_H

#include <pwm12.h>

class PWMgenerator
{
public:
    PWMgenerator(){}
    ~PWMgenerator(){}

    void init(){}

    void turnOn(){}
    void turnOff(){}
    void zeroOut(){}
    void setOutput(short Va, short Vb, short Vc)
    {
    }
};

#endif	/* PWMGENERATOR_H */

