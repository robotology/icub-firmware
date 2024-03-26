

/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Luca Tricerri
 * email:   luca.tricerri@iit.it
 */

// - include guard
// ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_ETH_theBATservice_H_
#define __EMBOT_APP_ETH_theBATservice_H_

#include "embot_core.h"
#include "embot_core_binary.h"

#include "EoCommon.h"
#include "EoProtocol.h"

#if defined(USE_EMBOT_theServices)  
#include "embot_app_eth_Service.h"
#else
#include "EOtheServices.h"
#endif

#include "embot_app_eth_theServiceTester.h"

#include <memory>
#include <vector>

namespace embot {
namespace app {
namespace eth {

class theBATservice {
public:
  struct Config {
    uint32_t tbd{0};
    constexpr Config() = default;
    constexpr Config(uint32_t t) : tbd(t) {}
    constexpr bool isvalid() const { return (666 == tbd) ? false : true; }
  };

  struct canFrameDescriptor {
    enum class Type : uint8_t {
      info,
      status_bms,
      status_bat,
      unspecified,
    };
    eOcanport_t port{eOcanport1};
    eOcanframe_t *frame{nullptr};
    Type type{Type::unspecified};
    canFrameDescriptor() = default;
    canFrameDescriptor(eOcanport_t p, eOcanframe_t *f, Type t)
        : port(p), frame(f), type(t){};
  };

  static theBATservice &getInstance();

  static constexpr size_t maxSensors{1};
  static constexpr size_t maxRegulars{maxSensors};

  eOresult_t initialise(const Config &config);

  eOmn_serv_state_t GetServiceState() const;
  eOresult_t Verify(const eOmn_serv_configuration_t *servcfg,
                    eOservice_onendofoperation_fun_t onverify,
                    bool activateafterverify);
  eOresult_t Activate(const eOmn_serv_configuration_t *servcfg);
  eOresult_t Deactivate();
  eOresult_t Start();
  eOresult_t Stop();
  eOresult_t SetRegulars(eOmn_serv_arrayof_id32_t *arrayofid32,
                         uint8_t *numberofthem);
  eOresult_t Tick();
  // processes a CAN frame coming from the sensor
  eOresult_t AcceptCANframe(const canFrameDescriptor &canframedescriptor);
  // it can be called by the ETH callbacks eoprot_fun_UPDT_as_bat_*(EOnv* nv,
  // eOropdescriptor_t* rd) in such a case use its nv and rd argument but it can
  // be called by any other module to emulate reception of a ROP. in such a
  // case, use nv = nullptr and the embot::app::eth::fill(eOropdescriptor_t ...)
  // function
  eOresult_t process(const eOropdescriptor_t *rd, const EOnv *nv = nullptr);
  eOresult_t SendReport();
  
//  // we can call them if _Activate() was called. they are used by the callbacks of eth protocol
//  eOresult_t eo_battery_GetFullScale(eOservice_onendofoperation_fun_t overrideonfullscaleready);
//  eOresult_t eo_battery_Set(eOas_battery_config_t *cfg);
//  eOresult_t eo_battery_SetDataRate(uint8_t datarate);
//  uint8_t eo_battery_GetDataRate();

  // default status of the BAT sensors
  static constexpr eOas_battery_status_t defaultBATstatus{
      .timedvalue = {.age = 0,
                     .temperature = 0,
                     .status = 0,
                     .voltage = 0,
                     .current = 0,
                     .charge = 0}};
  // default configuration of the BAT sensors
  static constexpr eOas_battery_config_t defaultBATconfig{
      .period = 100,
  };

  const theServiceTester::Config &servicetesterconfig() const;
  
private:
  // this one is called inside process() when the tag is
  // eoprot_tag_as_bat_config (or by theServiceTester)
  bool set(eOprotIndex_t index, const eOas_battery_config_t *ftc);
  // this one is called inside process() when the tag is
  // eoprot_tag_as_bat_cmmnds_enable (or by theServiceTester)
  bool enable(eOprotIndex_t index, const uint8_t *cmdenable);

  theBATservice();
  ~theBATservice();
  struct Impl;
  std::unique_ptr<Impl> pImpl;

  theBATservice(theBATservice const &) = delete;
  theBATservice &operator=(theBATservice const &) = delete;
};

} // namespace eth
} // namespace app
} // namespace embot

#endif // include-guard

// - end-of-file (leave a blank line
// after)----------------------------------------------------------------------------
