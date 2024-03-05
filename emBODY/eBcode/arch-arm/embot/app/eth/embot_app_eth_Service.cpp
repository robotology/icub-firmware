
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_Service.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "EoManagement.h"
#include "embot_core.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

static_assert(embot::core::tointegral(embot::app::eth::Service::Category::mc) == eomn_serv_category_mc, "incorrect value");
static_assert(embot::core::tointegral(embot::app::eth::Service::Category::ft) == eomn_serv_category_ft, "incorrect value");
static_assert(embot::app::eth::Service::numberOfCategories == eomn_serv_categories_numberof, "incorrect value");


static_assert(embot::core::tointegral(embot::app::eth::Service::Type::MC_foc) == eomn_serv_MC_foc, "incorrect value");
static_assert(embot::core::tointegral(embot::app::eth::Service::Type::MC_mc4plus) == eomn_serv_MC_mc4plus, "incorrect value");
static_assert(embot::core::tointegral(embot::app::eth::Service::Type::MC_advfoc) == eomn_serv_MC_advfoc, "incorrect value");
static_assert(embot::core::tointegral(embot::app::eth::Service::Type::AS_ft) == eomn_serv_AS_ft, "incorrect value");
static_assert(embot::core::tointegral(embot::app::eth::Service::Type::unknown) == eomn_serv_UNKNOWN, "incorrect value");
static_assert(embot::core::tointegral(embot::app::eth::Service::Type::none) == eomn_serv_NONE, "incorrect value");
static_assert(embot::app::eth::Service::numberOfTypes == eomn_serv_types_numberof, "incorrect value");


static_assert(embot::core::tointegral(embot::app::eth::Service::State::notsupported) == eomn_serv_state_notsupported, "incorrect value");
static_assert(embot::core::tointegral(embot::app::eth::Service::State::idle) == eomn_serv_state_idle, "incorrect value");
static_assert(embot::core::tointegral(embot::app::eth::Service::State::verifying) == eomn_serv_state_verifying, "incorrect value");
static_assert(embot::core::tointegral(embot::app::eth::Service::State::verified) == eomn_serv_state_verified, "incorrect value");
static_assert(embot::core::tointegral(embot::app::eth::Service::State::activated) == eomn_serv_state_activated, "incorrect value");
static_assert(embot::core::tointegral(embot::app::eth::Service::State::failureofverify) == eomn_serv_state_failureofverify, "incorrect value");
static_assert(embot::core::tointegral(embot::app::eth::Service::State::started) == eomn_serv_state_started, "incorrect value");
static_assert(embot::core::tointegral(embot::app::eth::Service::State::fatalerror) == eomn_serv_state_system_in_fatalerror, "incorrect value");



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


