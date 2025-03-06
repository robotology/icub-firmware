/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Jacopo Losi
 * email:   valentina.gaggero@iit.it, jacopo.losi@iit.it
*/

#ifndef __TESTRUNNERAMCCM7_H_
#define __TESTRUNNERAMCCM7_H_

#include "testRunnerBase.h"

// embot library includes
#include "embot_core.h"

class TestRunnerAmc_CM7 : public TestRunnerBase
{
    public:
        TestRunnerAmc_CM7(){embot::core::print("Built TestRunnerAmc_CM7");}
        ~TestRunnerAmc_CM7() = default;    
    
        bool testCanComm(uint8_t *data) override;
        bool testFwVersion() override;
        bool testLed(uint8_t on) override;
        bool testMicroId(uint8_t *data) override;
        bool testVin(uint8_t *data) override;
        bool testCin(uint8_t *data) override;
        bool testVauxOK() override;
        bool testPwrnFailOK() override;
        bool testFault(uint8_t on, uint8_t *data) override;
};

#endif
