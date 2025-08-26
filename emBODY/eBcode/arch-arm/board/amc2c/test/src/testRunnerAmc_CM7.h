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
};

#endif
