/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Jacopo Losi
 * email:   valentina.gaggero@iit.it, jacopo.losi@iit.it
*/

#ifndef __TESTRUNNERBASE_H_
#define __TESTRUNNERBASE_H_

#include "stm32hal.h"

class TestRunnerBase
{
    public:
        ~TestRunnerBase() = default;
    
        virtual bool testCanComm(uint8_t *data);
        virtual bool testEthComm(uint8_t *data);
        virtual bool testFwVersion(uint8_t *data);
        virtual bool testLed(uint8_t on);
        virtual bool testMicroId(uint8_t *data);
        virtual bool testVin(uint8_t *data);
        virtual bool testCin(uint8_t *data);
        virtual bool testVauxOK(uint8_t *data);
        virtual bool testPwrnFailOK(uint8_t *data);
        virtual bool testFault(uint8_t on, uint8_t *data);
        virtual bool testI2C(uint8_t *data);
        virtual bool testHallSensor();
        virtual bool testQuadEncoder();
        virtual bool testSpiAeaEncoder(uint8_t *data);
        virtual bool testPWMChannelx(const uint8_t channel, uint8_t *data);
        virtual bool testEEPROM(uint8_t *data);
};

#endif