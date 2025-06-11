/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Jacopo Losi
 * email:   valentina.gaggero@iit.it, jacopo.losi@iit.it
*/

#ifndef __TESTMANAGERSINGLETON_H_
#define __TESTMANAGERSINGLETON_H_

// embot library includes
#include "embot_core.h"

// local includes
#include "protocolManager.h"
#include "factoryTestRunner.h"

class TestManagerSingleton 
{
private:

    // instantiate the derived factory creator
    std::unique_ptr<FactoryTestRunner> _factoryTestRunner_ptr;
    std::unique_ptr<TestRunnerBase> _testRunner_ptr;
    

    // Private constructor to prevent direct instantiation
    TestManagerSingleton() 
    {
        embot::core::print("TestManager Singleton Instance Created");
        
        // create test runner when singleton is initialized to prevent errors in following calls
        _factoryTestRunner_ptr = std::make_unique<FactoryTestRunner>();
        _testRunner_ptr = _factoryTestRunner_ptr->createTestRunner(FactoryTestRunner::BoardRunnerType::amc_cm7);  
        embot::core::print("_testRunner_ptr Instance Created");
    }
    
     ~TestManagerSingleton() = default;
    
public:

    // Delete copy constructor and assignment operator
    TestManagerSingleton(const TestManagerSingleton&) = delete;
    TestManagerSingleton& operator=(const TestManagerSingleton&) = delete;
    // Delete move constructor and move assignment operator --> since it is singleton I do not need to move any resource 
    // If defaulted we could have -> Singleton mySingleton = std::move(Singleton::getInstance()); leading to singleton pattern violation 
    TestManagerSingleton(TestManagerSingleton&&) = delete;
    TestManagerSingleton& operator=(TestManagerSingleton&&) = delete;
    
    // Static method to provide access to the single instance
    static TestManagerSingleton& getInstance() 
    {
        static TestManagerSingleton instance;  // Guaranteed to be created only once (C++11 thread-safe)
        return instance;
    }

    
    std::unique_ptr<TestRunnerBase>& getTesterRunnerInstance() // superfluous
    {
        return _testRunner_ptr;
    }
    
    void showMessage() {
        embot::core::print("Hello from Test Manager Singleton");
    }
    
    void testManagerSystemInit(void* p);
    void testManagerTick();
    void testManagerRun();


};

#endif


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
