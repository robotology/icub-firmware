/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Jacopo Losi
 * email:   valentina.gaggero@iit.it, jacopo.losi@iit.it
*/

#ifndef __FACTORYTESTRUNNER_H_
#define __FACTORYTESTRUNNER_H_

#include "testRunnerBase.h"
#include "testRunnerAmc_CM7.h"
#include "testRunnerAmc_CM4.h"

// std system includes
#include <memory>


class FactoryTestRunner
{
    public:
        enum class BoardRunnerType
        {
            amc_cm7 = 0,
            amc_cm4 = 1,
            amc_bldc = 2,
            unknown = 244,
            dummy = 255
        };
        // generic base class creator made using the factory method design pattern
        std::unique_ptr<TestRunnerBase> createTestRunner(FactoryTestRunner::BoardRunnerType boardType)
        {
            switch(boardType)
            {
                case BoardRunnerType::amc_cm7:
                {
                    embot::core::print("Build unique_ptr type TestRunnerAmc_CM7");
                    return std::make_unique<TestRunnerAmc_CM7>();
                } break;
                case BoardRunnerType::amc_cm4:
                {
                    embot::core::print("Build unique_ptr type TestRunnerAmc_CM4");
                    return std::make_unique<TestRunnerAmc_CM4>();
                } break;
                case BoardRunnerType::unknown:
                case BoardRunnerType::dummy:
                default:
                {
                    embot::core::print("Build unique_ptr type TestRunnerBase");
                    return std::make_unique<TestRunnerBase>();
                } break;
            }
            
        }
        
        ~FactoryTestRunner() = default;
};

#endif