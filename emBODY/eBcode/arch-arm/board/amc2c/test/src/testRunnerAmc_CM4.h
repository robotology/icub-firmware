/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Jacopo Losi
 * email:   valentina.gaggero@iit.it, jacopo.losi@iit.it
*/

#ifndef __TESTRUNNERAMCCM4_H_
#define __TESTRUNNERAMCCM4_H_

#include "testRunnerBase.h"

// embot library includes
#include "embot_core.h"


class TestRunnerAmc_CM4 : public TestRunnerBase 
{
    public:
        TestRunnerAmc_CM4() {embot::core::print("Built TestRunnerAmc_CM4");}
        ~TestRunnerAmc_CM4() = default;
        
        bool testCanComm(uint8_t *data) override;
        bool testVin(uint8_t *data) override;
        bool testCin(uint8_t *data) override;
        bool testFault(uint8_t on, uint8_t *data) override;
        bool testHallSensor(uint8_t *data) override;
        bool testQuadEncoder(uint8_t *data) override;
        bool testPWMChannelx(const uint8_t channel, uint8_t *data) override;
};

#endif