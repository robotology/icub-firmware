// stub for the host tests of TETHER (test/host): in place of the real theTETHERservice (sockets, threads), it only
// gives the SharedVariables that the operations of the Dictionary use. the test builds them and calls set()
#pragma once
#include "embot_svc_tether.h"
#include "embot_svc_tether_SharedVariables.h"
namespace embot::svc {
    class theTETHERservice
    {
    public:
        static theTETHERservice &getInstance() { static theTETHERservice s {}; return s; }
        void set(tether::SharedVariables *v) { _vars = v; }
        tether::SharedVariables &sharedvariables() { return *_vars; }
    private:
        tether::SharedVariables *_vars {nullptr};
    };
}
