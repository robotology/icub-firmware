/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Jacopo Losi
 * email:   valentina.gaggero@iit.it, jacopo.losi@iit.it
*/

#ifndef __PROTOCOLMANAGER_H_
#define __PROTOCOLMANAGER_H_

// embot library includes
#include "embot_os_common.h"
#include "embot_hw_can.h"
#include "embot_prot_can.h"
#include "embot_core_binary.h"

#include "embot_os_theScheduler.h"
#include "embot_app_eth_theErrorManager.h"
#include "embot_app_theLEDmanager.h"

// make it singleton
class ProtocolManager
{
    private:
        // we can maybe create a parametrized constrcutor where we pass the number of CAN bus we wanna config with embot::hw::CAN::one or embot::hw::CAN::two
        ProtocolManager()
        {
            embot::core::print("Protocol Manager Instance Created");
        }
        
        ~ProtocolManager() = default;
        
        // Methods implemented for the callback related to the eventhread
        static void alertonrxframe(void *t);
        static void alertontxframe(void *t);
        static void emitONcan(embot::app::eth::theErrorManager::Severity sev, const embot::app::eth::theErrorManager::Caller &caller, const embot::app::eth::theErrorManager::Descriptor &des, const std::string &str);
        
        // private member objects
        static constexpr embot::hw::CAN canbus {embot::hw::CAN::one};
        static constexpr uint8_t addr {0};
        
    public:
        
    enum class TestCommand
    {
        testCAN         = 0,
        testFwVersion   = 1,
        testLedOff      = 2,
        testLedOn       = 3,
        testMicroId     = 4,
        testVin         = 5,
        testCin         = 6,
        testVauxOK      = 7,
        testPwrnFailOK  = 8,
        testFaultOff    = 9,
        testFaultOn     = 10,
        testI2C         = 11,
        testHallSensor  = 12,
        testQuadEncoder = 13,
        testUndefined   = 254,
        testDummy       = 255
    };
    
    // Delete copy constructor and assignment operator
    ProtocolManager(const ProtocolManager&) = delete;
    ProtocolManager& operator=(const ProtocolManager&) = delete;
    // Delete move constructor and move assignment operator
    ProtocolManager(ProtocolManager&&) = delete;
    ProtocolManager& operator=(ProtocolManager&&) = delete;
    
    // Static method to provide access to the single instance
    static ProtocolManager& getInstance() 
    {
        static ProtocolManager instance;  // Guaranteed to be created only once (C++11 thread-safe)
        return instance;
    }
    
    // Constexpr to be used in the tests
    static constexpr embot::os::Event evRXcanframe = embot::core::binary::mask::pos2mask<embot::os::Event>(1);
    static inline const std::uint8_t canRXcapacity = 32;
    static inline const std::uint8_t canTXcapacity = 32;
    
    embot::hw::can::Frame _inputCanFrame;
    embot::hw::can::Frame _outputCanFrame;
    
    void canProtocolInit(void *t);
    void parseTestCommand(ProtocolManager::TestCommand &cmd);
    void sendTestResult(uint8_t *outdata);

    void tick(size_t tt);
};

#endif