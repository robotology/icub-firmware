/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Jacopo Losi
 * email:   valentina.gaggero@iit.it, jacopo.losi@iit.it
*/

// APIs header
#include "testManagerSingleton.h"

// std includes
#include <string>

void TestManagerSingleton::testManagerSystemInit(void* p) 
{
    ProtocolManager& _ptcMgrInstance = ProtocolManager::getInstance();
    _ptcMgrInstance.canProtocolInit(p);
}

void TestManagerSingleton::testManagerTick()
{
    static size_t tt {0};
    tt++;
    ProtocolManager& _ptcMgrInstance = ProtocolManager::getInstance();
    _ptcMgrInstance.tick(tt);
}


void TestManagerSingleton::testManagerRun()
{
    ProtocolManager& _ptcMgrInstance = ProtocolManager::getInstance();
    
    ProtocolManager::TestCommand cmd = ProtocolManager::TestCommand::testUndefined;
    _ptcMgrInstance.parseTestCommand(cmd);
    uint8_t data[8] {0};
    embot::core::print("In TestManagerSingleton::testManagerRun() received cmd: " + std::to_string((uint8_t)cmd));
    // switch case based on cmd
    switch(cmd)
    {
        case ProtocolManager::TestCommand::testCAN:
        {
           _testRunner_ptr->testCanComm(data);
        } break;
        case ProtocolManager::TestCommand::testETH:
        {
            _testRunner_ptr->testEthComm(data);
        } break;
        case ProtocolManager::TestCommand::testFwVersion:
        {
            _testRunner_ptr->testFwVersion(data);
        } break;
        case ProtocolManager::TestCommand::testLedOff:
        {
            _testRunner_ptr->testLed(0);
        } break;
        case ProtocolManager::TestCommand::testLedOn:
        {
            _testRunner_ptr->testLed(1);
        } break;
        case ProtocolManager::TestCommand::testMicroId:
        {
            _testRunner_ptr->testMicroId(data);
        } break;
        case ProtocolManager::TestCommand::testVin:
        {
            _testRunner_ptr->testVin(data);
        } break;
        case ProtocolManager::TestCommand::testCin:
        {
            _testRunner_ptr->testCin(data);
        } break;
        case ProtocolManager::TestCommand::testVauxOK:
        {
            _testRunner_ptr->testVauxOK(data);
        } break;
        case ProtocolManager::TestCommand::testPwrnFailOK:
        {
            _testRunner_ptr->testPwrnFailOK(data);
        } break;
        case ProtocolManager::TestCommand::testFaultOff:
        {
            _testRunner_ptr->testFault(0, data);
        } break;
        case ProtocolManager::TestCommand::testFaultOn:
        {
            _testRunner_ptr->testFault(1, data);
        } break;
        case ProtocolManager::TestCommand::testI2C:
        {
             _testRunner_ptr->testI2C(data);
        } break;
        case ProtocolManager::TestCommand::testHallSensor:
        {
             _testRunner_ptr->testHallSensor();
        } break;
        case ProtocolManager::TestCommand::testQuadEncoder:
        {
             _testRunner_ptr->testQuadEncoder();
        } break;
        case ProtocolManager::TestCommand::testSPIEncoder:
        {
            _testRunner_ptr->testSpiAeaEncoder(data);
        } break;
        case ProtocolManager::TestCommand::testPWM1:
        {
            _testRunner_ptr->testPWMChannelx(1, data);
        } break;
        case ProtocolManager::TestCommand::testPWM2:
        {
            _testRunner_ptr->testPWMChannelx(2, data);
        } break;
        case ProtocolManager::TestCommand::testPWM3:
        {
            _testRunner_ptr->testPWMChannelx(3, data);
        } break;
        case ProtocolManager::TestCommand::testEEPROM:
        {
            _testRunner_ptr->testEEPROM(data);
        } break;
        default: break;
    }
    _ptcMgrInstance.sendTestResult(data);
}
    

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
