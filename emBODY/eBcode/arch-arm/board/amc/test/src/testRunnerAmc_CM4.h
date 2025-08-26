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
};

#endif