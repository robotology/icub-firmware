/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Jacopo Losi
 * email:   valentina.gaggero@iit.it, jacopo.losi@iit.it
*/

// APIs header
#include "testRunnerBase.h"

// embot library includes
#include "embot_core.h"

bool TestRunnerBase::testCanComm(uint8_t *data)
{ 
    embot::core::print("Called method from base");
    return true; 
}
bool TestRunnerBase::testEthComm(uint8_t *data)
{
    embot::core::print("Called method from base");
    return true;
}
bool TestRunnerBase::testFwVersion(uint8_t *data){ return true; }
bool TestRunnerBase::testLed(uint8_t on){ return true; }
bool TestRunnerBase::testMicroId(uint8_t *data){ return true; }
bool TestRunnerBase::testVin(uint8_t *data){ return true; }
bool TestRunnerBase::testCin(uint8_t *data){ return true; }
bool TestRunnerBase::testVauxOK(uint8_t *data){ return true; }
bool TestRunnerBase::testPwrnFailOK(){ return true; }
bool TestRunnerBase::testFault(uint8_t on, uint8_t *data){ return true; }
bool TestRunnerBase::testI2C(){ return true; }
bool TestRunnerBase::testHallSensor(){ return true; }
bool TestRunnerBase::testQuadEncoder(){ return true; }
bool TestRunnerBase::testSpiAeaEncoder(uint8_t *data){ return true; }


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
