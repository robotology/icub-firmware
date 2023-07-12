
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Luca Tricerri
 * email:   luca.tricerri@iit.it
 */

// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theBATservice.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <array>

#include "EOVtheCallbackManager.h"
#include "EOtheCANmapping.h"
#include "EOtheCANservice.h"
#include "EOtheErrorManager.h"
#include "EOtheServices_hid.h"
#include "EoError.h"
#include "EoProtocolAS.h"
#include "embot_app_eth_CANmonitor.h"
#include "embot_core_binary.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective
// modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

namespace embot::app::eth {} // namespace embot::app::eth

namespace embot::app::eth {

constexpr eOservice_core_t dummy_service_core{
    .initted = eobool_false,
    .active = eobool_false,
    .activateafterverify = eobool_false,
    .started = eobool_false,
    .onverify = nullptr,
    .state = eomn_serv_state_notsupported,
    .tmpcfg = nullptr,
    .servconfig = {.type = eomn_serv_NONE}};

constexpr eOservice_diagnostics_t dummy_service_diagnostics{
    .reportTimer = nullptr,
    .reportPeriod =
        0, // 10*EOK_reltime1sec, // with 0 we dont periodically report
    .errorDescriptor = {0},
    .errorType = eo_errortype_info,
    .errorCallbackCount = 0,
    .repetitionOKcase =
        0 // 10 // with 0 we transmit report only once at succesful activation
};

constexpr eOservice_cantools_t dummy_service_cantools{
    .boardproperties = nullptr,
    .entitydescriptor = nullptr,
    .discoverytargets = nullptr,
    .ondiscoverystop = {0},
    .command = {0},
};

// finally the implementation struct
struct embot::app::eth::theBATservice::Impl {
  Config config{};
  bool initted{false};

  // data structures used in the embobj implementation
  // we can surely reuse them
  eOservice_core_t service{dummy_service_core};
  eOservice_diagnostics_t diagnostics{dummy_service_diagnostics};
  eOservice_cantools_t sharedcan{dummy_service_cantools};
  EOarray *id32ofregulars{nullptr};

  constexpr static const char s_eobj_ownname[] = "theBATservice";

  const eOmn_serv_configuration_t *tmpservcfg{nullptr};
  // theBATnetvariables[i] holds pointer to the network variable of the whole
  // eOas_battery_t w/ i index theFTboards[i] contains the relenat board type
  std::array<eOas_battery_t *, maxSensors> theBATnetvariables{nullptr};
  std::array<eObrd_cantype_t, maxSensors> theBATboards{eobrd_cantype_bms};

  // this object performs the monitoring of presence of CAN boards
  CANmonitor canmonitor{};
  static constexpr CANmonitor::Config defaultcanmonitorconfig{
      {}, // the map is left empty
      100 * embot::core::time1millisec,
      CANmonitor::Report::ALL,
      10 * embot::core::time1second,
      s_eobj_ownname,
      eomn_serv_category_battery};

  embot::core::Time debugtimeft{0};

  // methods used by theBATservice

  Impl() = default;

  eOresult_t initialise();
  eOresult_t SendReport();
  eOresult_t Verify(const eOmn_serv_configuration_t *servcfg,
                    eOservice_onendofoperation_fun_t onverify,
                    bool activateafterverify);
  eOresult_t Activate(const eOmn_serv_configuration_t *servcfg);
  eOresult_t Deactivate();
  eOmn_serv_state_t GetServiceState() const;
  eOresult_t SetRegulars(eOmn_serv_arrayof_id32_t *arrayofid32,
                         uint8_t *numberofthem);
  eOresult_t Start();
  eOresult_t Stop();
  eOresult_t Tick();
  eOresult_t AcceptCANframe(const canFrameDescriptor &canframedescriptor);
  eOresult_t process(const eOropdescriptor_t *rd, const EOnv *nv);

  // methods used to manage ETH commands. called by process() but also by the
  // theBATservice
  bool set(eOprotIndex_t index, const eOas_battery_config_t *ftc);
  bool enable(eOprotIndex_t index, const uint8_t *cmdenable);

  const theServiceTester::Config &servicetesterconfig() const;

private:
  // service methods used to tx commands to CAN not used here
  void can_battery_Config(eOprotIndex_t index, uint8_t ratemillisec);
  void can_battery_TX(eOprotIndex_t index, bool on);
  void can_battery_TXstop();

  // service methods used to handle the verifying state
  // some must stay static because are used as callbacks
  // step 1: discovery of all required CAN boards
  // step 2: we start full scales query
  // step3: if query is OK we activate and send reply back to
  // yarprobotinterface. if KO just reply back.
  eOresult_t
  verifyingstate_step1_discovery(const eOmn_serv_configuration_t *servcfg,
                                 eOservice_onendofoperation_fun_t onverify,
                                 bool activateafterverify);
  static eOresult_t
  s_verifyingstate_step2_onstop_search_for_ft_boards_we_get_fullscale(
      void *par, EOtheCANdiscovery2 *cd2, eObool_t searchisok);

  // other static ones used for callbacks
  static void s_send_periodic_error_report(void *par);
  static constexpr eObool_t s_battery_isID32relevant(uint32_t id32);

  // debug funtions
  void debug_CANMAPPING(EOconstarray *carray);
};

eOresult_t embot::app::eth::theBATservice::Impl::initialise() {
  if (true == initted) {
    return eores_OK;
  }

  embot::core::print("embot::app::eth::theBATservice::Impl::initialise()");

  service.servconfig.type = eomn_serv_NONE;
  sharedcan.boardproperties =
      eo_vector_New(sizeof(eObrd_canproperties_t), theBATservice::maxSensors,
                    nullptr, 0, nullptr, nullptr);
  sharedcan.entitydescriptor =
      eo_vector_New(sizeof(eOcanmap_entitydescriptor_t),
                    theBATservice::maxSensors, nullptr, 0, nullptr, nullptr);
  sharedcan.discoverytargets = eo_array_New(
      theBATservice::maxSensors, sizeof(eOcandiscovery_target_t), nullptr);
  id32ofregulars = eo_array_New(embot::app::eth::theBATservice::maxRegulars,
                                sizeof(uint32_t), nullptr); // unico che serve

  diagnostics.reportTimer = eo_timer_New();
  diagnostics.errorType = eo_errortype_error;
  diagnostics.errorDescriptor.sourceaddress = eo_errman_sourcedevice_localboard;
  diagnostics.errorDescriptor.code = eoerror_code_get(
      eoerror_category_Config, eoerror_value_CFG_bat_not_verified_yet);

  service.initted = eobool_true;
  service.active = eobool_false;
  service.started = eobool_false;
  service.state = eomn_serv_state_idle;
  eo_service_hid_SynchServiceState(eo_services_GetHandle(),
                                   eomn_serv_category_battery, service.state);

  // clear data used by this class
  for (auto &item : theBATnetvariables)
    item = nullptr;
  for (auto &item : theBATboards)
    item = eobrd_cantype_bms;

  initted = true;
  return eores_OK;
}

eOresult_t embot::app::eth::theBATservice::Impl::Tick() {
  if (eobool_false ==
      service.active) { // nothing to do because object must be first activated
    return (eores_OK);
  }

  if (eobool_false == service.started) { // not running, thus we do nothing
    return (eores_OK);
  }

  // add in here a check vs correct arrival of can frames.
  canmonitor.tick();

  return eores_OK;
}

eOresult_t embot::app::eth::theBATservice::Impl::SendReport() {
  eo_errman_Error(eo_errman_GetHandle(), diagnostics.errorType, NULL,
                  s_eobj_ownname, &diagnostics.errorDescriptor);

  eOerror_value_t errorvalue =
      eoerror_code2value(diagnostics.errorDescriptor.code);

  switch (errorvalue) {
  case eoerror_value_CFG_bat_failed_candiscovery: {
    eo_candiscovery2_SendLatestSearchResults(eo_candiscovery2_GetHandle());
  } break;

  default: {
    // dont send any additional info
  } break;
  }

  return eores_OK;
}

eOresult_t embot::app::eth::theBATservice::Impl::Verify(
    const eOmn_serv_configuration_t *servcfg,
    eOservice_onendofoperation_fun_t onverify, bool activateafterverify) {
  eo_errman_Trace(eo_errman_GetHandle(), "::Verify()", s_eobj_ownname);

  if (NULL == servcfg) {
    service.state = eomn_serv_state_failureofverify;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(),
                                     eomn_serv_category_battery, service.state);
    if (NULL != onverify) {
      onverify(nullptr, eobool_false);
    }
    return (eores_NOK_nullpointer);
  }

  if (eomn_serv_AS_battery != servcfg->type) {
    service.state = eomn_serv_state_failureofverify;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(),
                                     eomn_serv_category_battery, service.state);
    if (NULL != onverify) {
      onverify(nullptr, eobool_false);
    }
    return (eores_NOK_generic);
  }

  if (eobool_true == service.active) {
    Deactivate();
  }

  service.state = eomn_serv_state_verifying;
  eo_service_hid_SynchServiceState(eo_services_GetHandle(),
                                   eomn_serv_category_battery, service.state);

  // make sure the timer is not running
  eo_timer_Stop(diagnostics.reportTimer);

  service.onverify = onverify;
  service.activateafterverify = activateafterverify;

  service.state = eomn_serv_state_verified;
  eo_service_hid_SynchServiceState(eo_services_GetHandle(),
                                   eomn_serv_category_battery, service.state);

  if (eobool_true == service.activateafterverify) {
    Activate(servcfg);
  }

  diagnostics.errorDescriptor.sourcedevice = eo_errman_sourcedevice_localboard;
  diagnostics.errorDescriptor.sourceaddress = 0;
  diagnostics.errorDescriptor.par16 = 0;
  diagnostics.errorDescriptor.par64 = 0;
  EOaction_strg astrg = {0};
  EOaction *act = (EOaction *)&astrg;
  eo_action_SetCallback(act, s_send_periodic_error_report, this,
                        eov_callbackman_GetTask(eov_callbackman_GetHandle()));

  diagnostics.errorType = eo_errortype_debug;
  diagnostics.errorDescriptor.code =
      eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_bat_ok);
  eo_errman_Error(eo_errman_GetHandle(), diagnostics.errorType, NULL,
                  s_eobj_ownname, &diagnostics.errorDescriptor);

  if ((0 != diagnostics.repetitionOKcase) && (0 != diagnostics.reportPeriod)) {
    diagnostics.errorCallbackCount = diagnostics.repetitionOKcase;
    eo_timer_Start(diagnostics.reportTimer, eok_abstimeNOW,
                   diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
  }

  if (NULL != service.onverify) {
    service.onverify(nullptr, eobool_true);
  }

  return eores_OK;
}

eOresult_t embot::app::eth::theBATservice::Impl::Activate(
    const eOmn_serv_configuration_t *servcfg) {
  eo_errman_Trace(eo_errman_GetHandle(), "::Activate()", s_eobj_ownname);

  if (NULL == servcfg) {
    return (eores_NOK_nullpointer);
  }

  if (eomn_serv_AS_battery != servcfg->type) {
    return (eores_NOK_generic);
  }

  if (eobool_true == service.active) {
    Deactivate();
  }

  memcpy(&service.servconfig, servcfg, sizeof(eOmn_serv_configuration_t));

  // now... use the servcfg
  EOconstarray *carray = eo_constarray_Load(reinterpret_cast<const EOarray *>(
      &servcfg->data.as.battery.arrayofsensors));
  uint8_t numofsensors = eo_constarray_Size(carray);

  // marco.accame on 25 feb 2022: for now i dont do any more verification on the
  // sanity of the servconfig. we already did whatever we could inside Verify()

  // in here we do:
  // - load boards in can mapping, and also in theFTboards
  // - load entities, one for board in strict order
  // - init the theBATnetvariables

  // we must load all the boards into can mapping, entities etc
  for (uint8_t s = 0; s < numofsensors; s++) {
    const eOas_battery_sensordescriptor_t *sd =
        reinterpret_cast<const eOas_battery_sensordescriptor_t *>(
            eo_constarray_At(carray, s));

    if ((nullptr != sd) &&
        (eobool_true ==
         eoas_battery_isboardvalid(static_cast<eObrd_cantype_t>(
             sd->boardinfo.type)))) { // even if it is a useless check because
                                      // we did it before inside Verify()

      // preparation for loading can boards into EOtheCANmapping
      eObrd_canproperties_t prop = {0};
      prop.type = sd->boardinfo.type;
      prop.location.port = sd->canloc.port;
      prop.location.addr = sd->canloc.addr;
      prop.location.insideindex = eobrd_caninsideindex_none;
      prop.requiredprotocol = sd->boardinfo.protocol;
      eo_vector_PushBack(sharedcan.boardproperties, &prop);

      // preparation for loading entities into EOtheCANmapping
      eOcanmap_entitydescriptor_t des = {0};
      des.location.port = sd->canloc.port;
      des.location.addr = sd->canloc.addr;
      des.location.insideindex = eobrd_caninsideindex_none;
      des.index = static_cast<eOcanmap_entityindex_t>(s);
      eo_vector_PushBack(sharedcan.entitydescriptor, &des);

      // data used by this class
      void *vpft = eoprot_entity_ramof_get(
          eoprot_board_localboard, eoprot_endpoint_analogsensors,
          eoprot_entity_as_battery, static_cast<eOprotIndex_t>(s));
      theBATnetvariables[s] = reinterpret_cast<eOas_battery_t *>(vpft);
      theBATboards[s] = static_cast<eObrd_cantype_t>(sd->boardinfo.type);
    }
  }

  eo_canmap_LoadBoards(eo_canmap_GetHandle(), sharedcan.boardproperties);
  eo_canmap_ConfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors,
                         eoprot_entity_as_battery, sharedcan.entitydescriptor);

  // ok, service is active now

  service.active = eobool_true;
  service.state = eomn_serv_state_activated;
  eo_service_hid_SynchServiceState(eo_services_GetHandle(),
                                   eomn_serv_category_battery, service.state);

  return (eores_OK);
}

eOresult_t embot::app::eth::theBATservice::Impl::Deactivate() {
  eo_errman_Trace(eo_errman_GetHandle(), "::Deactivate()", s_eobj_ownname);

  if (eobool_false == service.active) {
    // i force to eomn_serv_state_idle because it may be that state was
    // eomn_serv_state_verified or eomn_serv_state_failureofverify
    service.state = eomn_serv_state_idle;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(),
                                     eomn_serv_category_battery, service.state);
    return (eores_OK);
  }

  // send stop messages to the battery sensor board, unload the
  // entity-can-mapping and the board-can-mapping, reset all things inside this
  // object

  if (eobool_true == service.started) {
    Stop();
  }

  SetRegulars(nullptr, nullptr);

  // deconfig EOtheCANmapping
  eo_canmap_DeconfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors,
                           eoprot_entity_as_battery,
                           sharedcan.entitydescriptor);
  eo_canmap_UnloadBoards(eo_canmap_GetHandle(), sharedcan.boardproperties);

  // clear data used by this class
  for (auto &item : theBATnetvariables) {
    if (nullptr != item) {
      // i reset the status to its default value
      std::memmove(&item->status, &defaultBATstatus,
                   sizeof(eOas_battery_status_t));
      // i reset the config to its default value
      std::memmove(&item->config, &defaultFTconfig,
                   sizeof(eOas_battery_config_t));
    }

    item = nullptr;
  }
  for (auto &item : theBATboards) {
    item = eobrd_cantype_none;
  }

  memset(&service.servconfig, 0, sizeof(eOmn_serv_configuration_t));
  service.servconfig.type = eomn_serv_NONE;

  eo_vector_Clear(sharedcan.boardproperties);
  eo_vector_Clear(sharedcan.entitydescriptor);
  eo_array_Reset(sharedcan.discoverytargets);

  // make sure the timer is not running
  eo_timer_Stop(diagnostics.reportTimer);

  service.active = eobool_false;
  service.state = eomn_serv_state_idle;
  eo_service_hid_SynchServiceState(eo_services_GetHandle(),
                                   eomn_serv_category_battery, service.state);

  return (eores_OK);
}

eOresult_t embot::app::eth::theBATservice::Impl::Start() {
  eo_errman_Trace(eo_errman_GetHandle(), "::Start()", s_eobj_ownname);

  if (eobool_false ==
      service.active) { // nothing to do because object must be first activated
    return (eores_OK);
  }

  if (eobool_true == service.started) { // it is already running
    return (eores_OK);
  }

  service.started = eobool_true;
  service.state = eomn_serv_state_started;
  eo_service_hid_SynchServiceState(eo_services_GetHandle(),
                                   eomn_serv_category_battery, service.state);

  // Start() does not force the tx from any CAN sensor boards.
  // the activation of transmission is done at reception of command
  // set<eOas_battery_commands_t::enable, 1> managed inside process(const EOnv*
  // nv, const eOropdescriptor_t* rd)

  // we configure it w/ an empty target. and we start it. we shall add boards
  // when the tx is enabled
  CANmonitor::Config cfg = defaultcanmonitorconfig;
  // use ...
  cfg.target.clear();
  cfg.periodofcheck =
      embot::core::time1millisec *
      service.servconfig.data.as.battery.canmonitorconfig.periodofcheck;
  cfg.periodofreport =
      embot::core::time1millisec *
      service.servconfig.data.as.battery.canmonitorconfig.periodofreport;
  cfg.reportmode = static_cast<CANmonitor::Report>(
      service.servconfig.data.as.battery.canmonitorconfig.reportmode);
  canmonitor.configure(cfg);
  canmonitor.start();

  return eores_OK;
}

eOresult_t embot::app::eth::theBATservice::Impl::Stop() {
  eo_errman_Trace(eo_errman_GetHandle(), "::Stop()", s_eobj_ownname);

  if (eobool_false ==
      service.active) { // nothing to do because object must be first activated
    return (eores_OK);
  }

  if (eobool_false == service.started) { // it is already stopped
    return (eores_OK);
  }

  // Stop() forces the end of the tx from all the CAN sensor boards
  // and of the activities of the CAN monitor
  canmonitor.stop();
  can_battery_TXstop();

  service.started = eobool_false;
  service.state = eomn_serv_state_activated;
  eo_service_hid_SynchServiceState(eo_services_GetHandle(),
                                   eomn_serv_category_battery, service.state);

  // we dont remove all regulars related to ft entity because Deactivate() does
  // that.

  return eores_OK;
}

eOresult_t embot::app::eth::theBATservice::Impl::SetRegulars(
    eOmn_serv_arrayof_id32_t *arrayofid32, uint8_t *numberofthem) {
  eo_errman_Trace(eo_errman_GetHandle(), "::SetRegulars()", s_eobj_ownname);

  if (eobool_false ==
      service.active) { // nothing to do because object must be first activated
    return (eores_OK);
  }

  return (eo_service_hid_SetRegulars(id32ofregulars, arrayofid32,
                                     s_battery_isID32relevant, numberofthem));
}

eOresult_t embot::app::eth::theBATservice::Impl::AcceptCANframe(
    const canFrameDescriptor &cfd) {
  // object must be first activated otherwise we cannot use EOtheCANmapping,
  // theBATnetvariables, theFTboards
  if (eobool_false == service.active) {
    return (eores_OK);
  }

  eObrd_canlocation_t loc = {};
  loc.port = cfd.port;
  loc.addr = EOCANPROT_FRAME_GET_SOURCE(cfd.frame);
  loc.insideindex = eobrd_caninsideindex_none;

  eOprotIndex_t index = 0;

  canmonitor.touch(loc);

  eOas_battery_t *bat = theBATnetvariables[index];
  if (nullptr == bat) { // something is wrong
    return eores_NOK_generic;
  }

  switch (cfd.type) {
  case canFrameDescriptor::Type::unspecified: {
    bat->status.timedvalue.temperature =
        10 * ((static_cast<uint16_t>(cfd.frame->data[7]) << 8) +
        static_cast<uint16_t>(cfd.frame->data[6]));
    bat->status.timedvalue.charge = static_cast<float32_t>(
        (static_cast<uint16_t>(cfd.frame->data[5]) << 8) +
        static_cast<uint16_t>(cfd.frame->data[4]));
    int16_t curr = (static_cast<int16_t>(cfd.frame->data[3]) << 8) +
                  static_cast<int16_t>(cfd.frame->data[2]);
    bat->status.timedvalue.current =
            0.1 * static_cast<float32_t>(curr);
    bat->status.timedvalue.voltage =
        0.1 * static_cast<float32_t>(
                  (static_cast<uint16_t>(cfd.frame->data[1]) << 8) +
                  static_cast<uint16_t>(cfd.frame->data[0]));
    break;
  }
  default:
    return eores_NOK_unsupported;
  }

  return eores_OK;
}

eOmn_serv_state_t
embot::app::eth::theBATservice::Impl::GetServiceState() const {
  return service.state;
}

eOresult_t
embot::app::eth::theBATservice::Impl::process(const eOropdescriptor_t *rd,
                                              const EOnv *nv) {
  if ((nullptr == rd) || (nullptr == rd->data)) {
    return eores_NOK_nullpointer;
  }

  eOprotIndex_t index = eoprot_ID2index(rd->id32);
  eOprot_tag_as_battery_t tag =
      static_cast<eOprot_tag_as_battery_t>(eoprot_ID2tag(rd->id32));

  switch (tag) {
  case eoprot_tag_as_battery_config: {
    // in here we need to get the configuration and assign it
    eOas_battery_config_t *cfg =
        reinterpret_cast<eOas_battery_config_t *>(rd->data);
    set(index, cfg);
  } break;

  case eoprot_tag_as_battery_cmmnds_enable: {
    // in here we either start or stop depending on value 1 or 0 of ...
    // eOas_battery_commands_t::enable
    uint8_t *cmdenable = reinterpret_cast<uint8_t *>(rd->data);
    enable(index, cmdenable);
  } break;

  default: {

  } break;
  }

  return eores_OK;
}

bool embot::app::eth::theBATservice::Impl::set(
    eOprotIndex_t index, const eOas_battery_config_t *ftc) {
  // NOTE-x:
  // ... the ETH protocol mechanism already writes inside the relevant memory
  // the new values inside ftc we just have have them, which are also inside
  // theBATnetvariables[index]->config however, ... if the method process()
  // whuich calls set() is called by theServiceValidator and not by the ETH
  // callbacks, we also must copy what inside ftc but ... beware of nullptr

  if (nullptr == ftc) {
    return false;
  }

  if (eobool_false == service.active) {
    return false;
  }

  eOas_battery_config_t *theNVptr = (nullptr != theBATnetvariables[index])
                                        ? &theBATnetvariables[index]->config
                                        : nullptr;

  if (nullptr == theNVptr) {
    return false;
  }

  if (ftc != theNVptr) { // in this case the caller was not the ETH handler, so
                         // we must update the internal memory
    std::memmove(theNVptr, ftc, sizeof(eOas_battery_config_t));
  }

  // impose datarate to the BAT sensor board
  can_battery_Config(index, ftc->period);

  return true;
}

bool embot::app::eth::theBATservice::Impl::enable(eOprotIndex_t index,
                                                  const uint8_t *cmdenable) {
  // see NOTE-x above.

  if (nullptr == cmdenable) {
    return false;
  }

  if (eobool_false == service.active) {
    return false;
  }

  uint8_t *theNVptr = (nullptr != theBATnetvariables[index])
                          ? &theBATnetvariables[index]->cmmnds.enable
                          : nullptr;

  if (nullptr == theNVptr) {
    return false;
  }

  if (cmdenable != theNVptr) { // in this case the caller was not the ETH
                               // handler, so we must update the internal memory
    std::memmove(theNVptr, cmdenable, sizeof(uint8_t));
  }

  // in here i will start / stop tx of FT data

  bool on = (1 == *cmdenable) ? true : false;

  can_battery_TX(index, on);

  // i also need to add / rem the can location to the canmonitor.
  // NOTE: i do that only for FT, not for temperature because it is FT che
  // comanda
  eObrd_canlocation_t loc{};
  eOprotID32_t id32 =
      eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_battery,
                    index, eoprot_tag_none);
  if (eores_OK == eo_canmap_GetEntityLocation(eo_canmap_GetHandle(), id32, &loc,
                                              NULL, NULL)) {
    if (true == on) {
      canmonitor.add(loc);
      //            // i adjust rate according to actual rate w/ a safety factor
      //            constexpr uint16_t percentgain = 100;
      //            embot::core::relTime periodofcheck = 10*(100+percentgain) *
      //            theBATnetvariables[index]->config.ftperiod;
      //            canmonitor.setcheckrate(periodofcheck);
    } else {
      canmonitor.rem(loc);
    }
  }

  return true;
}

void embot::app::eth::theBATservice::Impl::s_send_periodic_error_report(
    void *par) {
  embot::app::eth::theBATservice::Impl *p =
      reinterpret_cast<embot::app::eth::theBATservice::Impl *>(par);

  eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL,
                  s_eobj_ownname, &p->diagnostics.errorDescriptor);

  if (eoerror_value_CFG_bat_failed_candiscovery ==
      eoerror_code2value(
          p->diagnostics.errorDescriptor
              .code)) { // if i dont find the can boards, i keep on sending the
                        // discovery results up. it is a temporary diagnostics
                        // tricks until we use the verification of services at
                        // bootstrap
    eo_candiscovery2_SendLatestSearchResults(eo_candiscovery2_GetHandle());
  }

  if (EOK_int08dummy != p->diagnostics.errorCallbackCount) {
    p->diagnostics.errorCallbackCount--;
  }
  if (0 == p->diagnostics.errorCallbackCount) {
    eo_timer_Stop(p->diagnostics.reportTimer);
  }
}

constexpr eObool_t
embot::app::eth::theBATservice::Impl::s_battery_isID32relevant(uint32_t id32) {
  constexpr uint32_t mask0 = (((uint32_t)eoprot_endpoint_analogsensors) << 24) |
                             (((uint32_t)eoprot_entity_as_battery) << 16);

  if ((id32 & mask0) == mask0) {
    return (eobool_true);
  }

  return (eobool_false);
}

void embot::app::eth::theBATservice::Impl::can_battery_Config(
    eOprotIndex_t index, uint8_t ratemillisec) {
  // For now do nothing for BMS
}

void embot::app::eth::theBATservice::Impl::can_battery_TX(eOprotIndex_t index,
                                                          bool on) {
  // For now do nothing for BMS
}

void embot::app::eth::theBATservice::Impl::can_battery_TXstop() {
  // For now do nothing for BMS
}

void embot::app::eth::theBATservice::Impl::debug_CANMAPPING(
    EOconstarray *carray) {
  // Notihing to do for now
}

const theServiceTester::Config &
embot::app::eth::theBATservice::Impl::servicetesterconfig() const {
  static constexpr embot::app::eth::theServiceTester::Config FTservtestconfig{};
  return FTservtestconfig;
}

} // namespace embot::app::eth

// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::eth::theBATservice &embot::app::eth::theBATservice::getInstance() {
  static theBATservice *p = new theBATservice();
  return *p;
}

embot::app::eth::theBATservice::theBATservice() {
  pImpl = std::make_unique<Impl>();
}

embot::app::eth::theBATservice::~theBATservice() {}

eOresult_t embot::app::eth::theBATservice::initialise(const Config &config) {
  pImpl->config = config;
  return pImpl->initialise();
}

eOresult_t embot::app::eth::theBATservice::Tick() { return pImpl->Tick(); }

eOresult_t embot::app::eth::theBATservice::SendReport() {
  return pImpl->SendReport();
}

eOresult_t embot::app::eth::theBATservice::Verify(
    const eOmn_serv_configuration_t *servcfg,
    eOservice_onendofoperation_fun_t onverify, bool activateafterverify) {
  return pImpl->Verify(servcfg, onverify, activateafterverify);
}

eOmn_serv_state_t embot::app::eth::theBATservice::GetServiceState() const {
  return pImpl->GetServiceState();
}

eOresult_t embot::app::eth::theBATservice::Activate(
    const eOmn_serv_configuration_t *servcfg) {
  return pImpl->Activate(servcfg);
}

eOresult_t embot::app::eth::theBATservice::Deactivate() {
  return pImpl->Deactivate();
}

eOresult_t embot::app::eth::theBATservice::Start() { return pImpl->Start(); }

eOresult_t embot::app::eth::theBATservice::Stop() { return pImpl->Stop(); }

eOresult_t embot::app::eth::theBATservice::SetRegulars(
    eOmn_serv_arrayof_id32_t *arrayofid32, uint8_t *numberofthem) {
  return pImpl->SetRegulars(arrayofid32, numberofthem);
}

eOresult_t embot::app::eth::theBATservice::AcceptCANframe(
    const canFrameDescriptor &canframedescriptor) {
  return pImpl->AcceptCANframe(canframedescriptor);
}

eOresult_t embot::app::eth::theBATservice::process(const eOropdescriptor_t *rd,
                                                   const EOnv *nv) {
  return pImpl->process(rd, nv);
}

bool embot::app::eth::theBATservice::set(eOprotIndex_t index,
                                         const eOas_battery_config_t *ftc) {
  return pImpl->set(index, ftc);
}

bool embot::app::eth::theBATservice::enable(eOprotIndex_t index,
                                            const uint8_t *cmdenable) {
  return pImpl->enable(index, cmdenable);
}

const embot::app::eth::theServiceTester::Config &
embot::app::eth::theBATservice::servicetesterconfig() const {
  return pImpl->servicetesterconfig();
}

// other objects

// - extern C functions ....

extern "C" {

extern void eoprot_fun_UPDT_as_battery_config(const EOnv *nv,
                                              const eOropdescriptor_t *rd) {
  embot::app::eth::theBATservice::getInstance().process(rd, nv);
}

extern void
eoprot_fun_UPDT_as_battery_cmmnds_enable(const EOnv *nv,
                                         const eOropdescriptor_t *rd) {
  embot::app::eth::theBATservice::getInstance().process(rd, nv);
}

extern void eoprot_fun_INIT_as_battery_config(const EOnv *nv) {
  eOas_battery_config_t *config =
      reinterpret_cast<eOas_battery_config_t *>(eo_nv_RAM(nv));
  std::memmove(config, &embot::app::eth::theBATservice::defaultFTconfig,
               sizeof(eOas_battery_config_t));
}

extern void eoprot_fun_INIT_as_battery_status(const EOnv *nv) {
  eOas_battery_status_t *status =
      reinterpret_cast<eOas_battery_status_t *>(eo_nv_RAM(nv));
  std::memmove(status, &embot::app::eth::theBATservice::defaultBATstatus,
               sizeof(eOas_battery_status_t));
}

} // extern "C"

// - end-of-file (leave a blank line
// after)----------------------------------------------------------------------------
