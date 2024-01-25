
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theServiceMC.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_Service_impl.h"

#include "embot_app_eth_theServiceMC_impl.h"

//#include "embot_prot_can_motor_periodic.h"

//#include "EOtheCANprotocol.h"
//#include "EOtheCANmapping.h"

//#include "EOtheMotionController.h"

#include "embot_app_eth_theErrorManager.h"
#include "embot_os_theScheduler.h"

//#include "EoProtocolMC.h"
//#include "EOtheEntities.h"
//#include "Controller.h"
//#include "embot_app_eth_theEncoderReader.h"
//#include "embot_app_eth_theICCmapping.h"
#include "embot_app_eth_theServices.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


namespace embot::app::eth {


struct embot::app::eth::theServiceMC::Impl
{ 
        
    theServiceMC *_owner {nullptr};   
    
    theServiceMC::Config _config {}; // so far empty
        
    // it contains a great deal of useful code
    embot::app::eth::service::impl::mc::MCservice _mcservice {};
        
    // methods used by theServiceMC 
    
    Impl(theServiceMC *o) : _owner(o) {};   
 
        
    bool initialise();
                
    // interface to Service  
        
    Category category() const { return embot::app::eth::Service::Category::mc; }
    Type type() const { return _mcservice.type(); }
    State state() const { return _mcservice.state(); }
    void set(State s) { _mcservice.state(s); }
    
    bool verifyactivate(const embot::app::eth::Service::Config * servcfg, OnEndOfOperation onendofverifyactivate);
    bool deactivate();
    bool start();
    bool stop();
    bool set(const eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t& numberofthem);
    bool tick(bool resetstatus);
    bool report();
    bool process(const DescriptorCANframe &canframedescriptor);
    bool process(const DescriptorROP &ropdescriptor);
    
    // others

private: 
   
    static constexpr char p_objname[] = "theServiceMC";

    void p_traceprint(const char *str)
    {
        embot::app::eth::theErrorManager::getInstance().trace(str, {p_objname, embot::os::theScheduler::getInstance().scheduled()}); 
    }  
    
//    bool p_applysetpoint(eOprotIndex_t jointindex, eOmc_setpoint_t *setpoint);

//    bool p_encodersAcquire();
//    bool p_encodersApplyValue(uint8_t index, embot::app::eth::encoder::v1::valueInfo *valueinfo);
//    void p_JOMOupdatestatus();
    
//    static uint8_t p_getentityindex(eOprot_entity_t entity, const DescriptorCANframe &canframedescriptor);
    static bool p_isID32relevant(uint32_t id32);    
};


      
bool embot::app::eth::theServiceMC::Impl::initialise()
{
    p_traceprint("initialise()");

    if(true == _mcservice.initted())
    {
        return true;
    }
    
    constexpr size_t maxregulars {maxREGULARs};
    _mcservice.initialise(maxregulars);

//    // note: we could move the controller and encoder reader inside MCservice
//    
//    // controller
//    constexpr size_t eo_motcon_standardENCOs {6}; // as in legacy code.
//    MController_new(maxJOMOs, eo_motcon_standardENCOs);
//    
//    // encoder reader
//    embot::app::eth::theEncoderReader::getInstance().initialise();
           
    // synch service to theServices. at this stage it will impose embot::app::eth::Service::State::idle 
    _mcservice.synch(embot::app::eth::Service::State::idle);
    
#if 0
    state()     idle
    active()    false
    started()   false
#endif    

    return true;
}

bool embot::app::eth::theServiceMC::Impl::stop()
{
    p_traceprint("stop()");

#if 0
    to preceed we must be in started state
    state   started
    active  true
    started true
#endif      

    if(false == _mcservice.active())
    {   // nothing to do because object must be activated
        return true;
    } 
        
    if(false == _mcservice.started())
    {   // it is already stopped
        return true;
    }
    
//    // i just idle the controller
//    MController_go_idle();
    
    _mcservice.stop();

    // and then i revert the state to activated 
    _mcservice.synch(embot::app::eth::Service::State::activated);

#if 0
    we have
    state   activated
    active  true
    started false
#endif
    
    return true;
}

bool embot::app::eth::theServiceMC::Impl::deactivate()
{
    p_traceprint("deactivate()");
    
        
    // if the  service is not activated (so states: idle, failureofverify, verified or verifying) i just clear and synch to idle
    // actually the state verifying may give some problems but that should never happen
    if(false == _mcservice.active())
    {
        _mcservice.clear(); 
        _mcservice.synch(embot::app::eth::Service::State::idle);
        return true;        
    } 
    
#if 0
    now we surely are in
    state   started / activated
    active  true
    started true / false
#endif    
        
    
    // if the service is in state started at first we stop it so that it goes into state activated
    if(true == _mcservice.started())
    {
        Impl::stop();   
    }    

#if 0
    now we surely are in
    state   activated
    active  true
    started false
#endif 
    
    // we just need to make sure the regulars are cleared
    uint8_t n {0};
    Impl::set(nullptr, n);  
    
    // then we call the proper deactivate of the mcservice that sends into idle state
    _mcservice.deactivate();
    
    // i may also force gain the active flag to false and the state idle 
    _mcservice.synch(embot::app::eth::Service::State::idle);
    
    return true;
}


bool embot::app::eth::theServiceMC::Impl::verifyactivate(const embot::app::eth::Service::Config * servcfg, OnEndOfOperation onendofverifyactivate)
{
    p_traceprint("verifyactivate()");
    
#if 0
    state()     any, but normally: idle, activated, started, failureofverify. hopefully never verifying or verified
    active()    any
    started()   any
#endif 
    
    // if we already are in states activated or started we need to go back to idle. we call deactivate() that also calls stop() if needed.
    // but it would be much better to call deactivate() in any case because we want to go to idle in any case.
    // only thing: if we are in verifying state we should just drop the request.

#if 0
    // as it was     
    if(true == _mcservice.active())
    {
        deactivate();        
    }         
#else
    // as it is better to have
    if(embot::app::eth::Service::State::verifying == _mcservice.state())
    {
        // damn: the board is busy in verifying what YRI has asked, YRI is waiting for a reply and so we should never receive this request. 
        // nevertheless we dont continue and ...        
        _mcservice.forcestopofverify();        
        _mcservice.synch(embot::app::eth::Service::State::failureofverify);
        
        if(nullptr != onendofverifyactivate)
        {
            onendofverifyactivate(_owner, false); 
        }        
        return false;                
    }
    
    // in all other cases it is better to deactivate() first
    
    deactivate();

#endif
    
 
#if 0
    now we surely are in
    state     idle
    active    false
    started   false
#endif     
    
    if((nullptr == servcfg) || (embot::app::eth::Service::Config::Type::eOmn_serv != servcfg->type()))
    { 
        _mcservice.synch(embot::app::eth::Service::State::failureofverify);        
        if(nullptr != onendofverifyactivate)
        {
            onendofverifyactivate(_owner, false); 
        }        
        return false;
    }
    
    
    // now we check if service configuration is OK

    const eOmn_serv_configuration_t * svc =  reinterpret_cast<const eOmn_serv_configuration_t*>(servcfg->memory());      
    const embot::app::eth::Service::Type t {static_cast<const embot::app::eth::Service::Type>(svc->type)};
    
    if(false == _mcservice.supported(t))  
    {
        p_traceprint("verifyactivate() has unrecognised type");   
        _mcservice.synch(embot::app::eth::Service::State::failureofverify);        
        if(nullptr != onendofverifyactivate)
        {
            onendofverifyactivate(_owner, false); 
        }        
        return false;
    }
    

    // ok, i can verify
    
 
    // load and copy inside the service configuration
    if(false == _mcservice.load(_owner, svc))
    {
        _mcservice.synch(embot::app::eth::Service::State::failureofverify);        
        if(nullptr != onendofverifyactivate)
        {
            onendofverifyactivate(_owner, false); 
        }        
        return false;        
    }
    
    // assign the proper state   
    _mcservice.synch(embot::app::eth::Service::State::verifying);    


#if 0
    we are now
    state   verifying
    active  true
    started false
#endif      
    
    // start the process. at end of verification we may go to states:
    // - failureofverify, or
    // - verified and then into activated   
    if(false == _mcservice.verifyactivate(onendofverifyactivate))
    {
        _mcservice.synch(embot::app::eth::Service::State::failureofverify);        
        if(nullptr != onendofverifyactivate)
        {
            onendofverifyactivate(_owner, false); 
        }        
        return false;         
    }
    
    return true;
}


bool embot::app::eth::theServiceMC::Impl::start()
{
    p_traceprint("start()");
    
#if 0
    to proceed we must have
    state   activated
    active  true
    started false
#endif      
       
    if(false == _mcservice.active())
    {   // nothing to do because object must be first activated
        return true;
    } 
        
    if(true == _mcservice.started())
    {   // it is already started
        return true;
    }
                       
//    // we just need to start a read of the encoder, nothing else 
//    embot::app::eth::theEncoderReader::getInstance().StartReading();
    
    _mcservice.start();

    
    // now i synch to state started 
    _mcservice.synch(embot::app::eth::Service::State::started);

    
#if 0
    and now we have
    state   started
    active  true
    started true
#endif   
    
    return true;
}



bool embot::app::eth::theServiceMC::Impl::set(const eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t& numberofthem)
{
    p_traceprint("set(ID32)");
    
#if 0
    must be
    state   activated / started
    active  true
    started false / true
#endif      
    
    if(false == _mcservice.active())
    {   // nothing to do because object must be activated
        return true;
    } 

    bool r = embot::app::eth::theServices::getInstance().setREGULARS(_mcservice.core.id32ofRegulars, arrayofid32, p_isID32relevant, &numberofthem);
    
    return r;  
}

bool embot::app::eth::theServiceMC::Impl::tick(bool resetstatus)
{
    p_traceprint("tick()");
    
#if 0
    must be
    state   started
    active  true
    started true
#endif  
    
        
    if(false == _mcservice.started())
    {   // not yet started
        return true;
    }
     
    _mcservice.tick();
    
//    // 1) get the encoders
//    p_encodersAcquire();

//    // 2) starts another reading
//    embot::app::eth::theEncoderReader::getInstance().StartReading();
//    
//    // 3) perform motor control  
//    MController_do();
//    
//    // 4) update status of motors and joints
//    p_JOMOupdatestatus();  
    
    return true;
}

bool embot::app::eth::theServiceMC::Impl::report()
{
    p_traceprint("report()");
    //#warning TODO: add embot::app::eth::theServiceMC::Impl::report()
    return true;
}

bool embot::app::eth::theServiceMC::Impl::process(const DescriptorCANframe &canframedescriptor)
{
    p_traceprint("process(DescriptorCANframe)");    
    bool r {false};

    // i can proceed only if service is active .... 
    if(false == _mcservice.active())
    {
        return r;
    }
    
    if(nullptr == canframedescriptor.frame)
    {
        return r;
    }
    
    return _mcservice.process(canframedescriptor);
    
//    embot::prot::can::Frame frame {canframedescriptor.frame->id, canframedescriptor.frame->size, canframedescriptor.frame->data};    
//    embot::prot::can::Clas cls = embot::prot::can::frame2clas(frame);
//    
//    // we only accept commands of class periodicMotorControl   
//    if(embot::prot::can::Clas::periodicMotorControl != cls)
//    {
//        return r;
//    }
//    
//    embot::prot::can::motor::periodic::CMD cmd = static_cast<embot::prot::can::motor::periodic::CMD>(embot::prot::can::frame2cmd(frame));

////    eOmc_motor_t *motor {nullptr};
////    eOprotIndex_t motorindex {EOK_uint08dummy};
////    if(nullptr == (motor = (eOmc_motor_t*) p_eocanprotMCperiodic_get_entity(eoprot_entity_mc_motor, canframedescriptor.frame, canframedescriptor.port, eobrd_caninsideindex_first, &motorindex)))
////    { 
////        return r;
////    }

//        
//    switch(cmd)
//    {
//        case embot::prot::can::motor::periodic::CMD::FOC:
//        {
//            eOprotIndex_t motorindex = p_getentityindex(eoprot_entity_mc_motor, canframedescriptor);
//            eOmc_motor_t *motor = _mcservice.motor(motorindex);
//            if(nullptr != motor)
//            {    
//                r = true;
//                
//                // note of marco.accame: the following code is ok as long as the 2foc has been configured to send up in its periodic message 
//                // current, velocity, and position. if so, frame->data contains: [current:2bytes, velocity:2bytes, position:4bytes]. 
//                // the following code extract these values. 
//                motor->status.basic.mot_current  = ((int16_t*)canframedescriptor.frame->data)[0];
//                motor->status.basic.mot_velocity = ((int16_t*)canframedescriptor.frame->data)[1];
//                motor->status.basic.mot_position = ((int32_t*)canframedescriptor.frame->data)[1];

//                MController_update_motor_odometry_fbk_can(motorindex, canframedescriptor.frame->data); 
//            }                
//        } break;

//        case embot::prot::can::motor::periodic::CMD::STATUS:
//        {
//            eOprotIndex_t jointindex = p_getentityindex(eoprot_entity_mc_joint, canframedescriptor);          
//            eOmc_joint_t *joint = _mcservice.joint(jointindex);
//            if(nullptr != joint)
//            {    
//                r = true;
//                MController_update_motor_state_fbk(jointindex, canframedescriptor.frame->data); 
//            }                
//        } break;
//        
//        default:
//        {
//        } break;
//    }
//    
//    
//    return true;
}

bool embot::app::eth::theServiceMC::Impl::process(const DescriptorROP &ropdescriptor)
{
    p_traceprint("process(DescriptorROP)");
    
    bool r {false};
    
    // i can proceed only if service is active .... 
    if(false == _mcservice.active())
    {
        return r;
    }
    
    // and if we have a valid descriptor
    if((nullptr == ropdescriptor.rd) || (nullptr == ropdescriptor.nv))
    {
        return r;
    }
    
    return _mcservice.process(ropdescriptor);
    
//    eOprotEntity_t entity = eoprot_ID2entity(ropdescriptor.rd->id32);
//    eOprotIndex_t index = eoprot_ID2index(ropdescriptor.rd->id32);
//    eOprotTag_t tag = eoprot_ID2tag(ropdescriptor.rd->id32);
//    
//    
//    if(eomc_entity_joint == entity)
//    {
//        // process the joint tags
//        switch(static_cast<eOprot_tag_mc_joint_t>(tag))
//        {
//            // .config section
//            // almost every tag is managed BUT ... I believe that YRI sends ony set<eoprot_tag_mc_joint_config, data>
//            // so, we may prune the code
//            
//            case eoprot_tag_mc_joint_config: 
//            {   // 1
//                eOmc_joint_config_t *jcfg = reinterpret_cast<eOmc_joint_config_t*>(ropdescriptor.rd->data);
//                MController_config_joint(index, jcfg);   
//                r = true;
//            } break;

//            case eoprot_tag_mc_joint_config_pidposition:
//            {   // 2
//                eOmc_PID_t *pid = reinterpret_cast<eOmc_PID_t*>(ropdescriptor.rd->data);
//                MController_config_minjerk_pid(index, pid);   
//                r = true;
//            } break;

//            case eoprot_tag_mc_joint_config_pidvelocity:
//            {   // 3
//                eOmc_PID_t *pid = reinterpret_cast<eOmc_PID_t*>(ropdescriptor.rd->data);
//                // apparently eoprot_fun_UPDT_mc_joint_config_pidvelocity() originally does not call it
//                // MController_config_direct_pid(index, pid);   
//                r = true;
//            } break;

//            case eoprot_tag_mc_joint_config_pidtorque:
//            {   // 4
//                eOmc_PID_t *pid = reinterpret_cast<eOmc_PID_t*>(ropdescriptor.rd->data);
//                MController_motor_config_torque_PID(index, pid);   
//                r = true;
//            } break;

//            case eoprot_tag_mc_joint_config_userlimits:
//            {   // 5
//                eOmeas_position_limits_t *limitsofjoint = reinterpret_cast<eOmeas_position_limits_t*>(ropdescriptor.rd->data);
//                MController_config_joint_pos_limits(index, limitsofjoint->min, limitsofjoint->max);   
//                r = true;
//            } break;

//            case eoprot_tag_mc_joint_config_impedance:
//            {   // 6
//                eOmc_impedance_t *impedance = reinterpret_cast<eOmc_impedance_t*>(ropdescriptor.rd->data);
//                MController_config_joint_impedance(index, impedance);
//                r = true;
//            } break;
//            
//            case eoprot_tag_mc_joint_config_motor_params:
//            {   // 7
//                eOmc_motor_params_t *mparams = reinterpret_cast<eOmc_motor_params_t*>(ropdescriptor.rd->data);
//                MController_config_motor_friction(index, mparams);
//                r = true;
//            } break;
//            
//            // .status section
//            // nothing is managed in here because:
//            // 1. status is signalled by the board to YRI and never asked
//            // 2. the exception was case eoprot_tag_mc_joint_status_core_modes_ismotiondone
//            //    that was used only for eo_motcon_mode_mc4 that is not managed in here

//            // .input section
//            //  only eoprot_tag_mc_joint_inputs_externallymeasuredtorque
//            
//            case eoprot_tag_mc_joint_inputs_externallymeasuredtorque:
//            {   // 18
//                eOmeas_torque_t *torque = reinterpret_cast<eOmeas_torque_t*>(ropdescriptor.rd->data);
//                MController_update_joint_torque_fbk(index, *torque);
//                r = true;                
//            } break;
//            
//            // .cmmnds section
//            // 
//            
//            case eoprot_tag_mc_joint_cmmnds_calibration:
//            {   // 19
//                eOmc_calibrator_t *calibrator = reinterpret_cast<eOmc_calibrator_t*>(ropdescriptor.rd->data);
//                MController_calibrate(index, calibrator);
//                r = true;                
//            } break;

//            case eoprot_tag_mc_joint_cmmnds_setpoint:
//            {   // 20
//                eOmc_setpoint_t *setpoint = reinterpret_cast<eOmc_setpoint_t*>(ropdescriptor.rd->data);
//                r = p_applysetpoint(index, setpoint);               
//            } break;

//            case eoprot_tag_mc_joint_cmmnds_stoptrajectory:
//            {   // 21
//                MController_stop_joint(index);      
//                r = true;                
//            } break;
//            
//            case eoprot_tag_mc_joint_cmmnds_controlmode:
//            {   // 22
//                eOmc_controlmode_command_t *controlmode = reinterpret_cast<eOmc_controlmode_command_t*>(ropdescriptor.rd->data);
//                MController_set_control_mode(index, *controlmode);      
//                r = true;                
//            } break;
//            
//            case eoprot_tag_mc_joint_cmmnds_interactionmode:
//            {   // 23
//                eOmc_interactionmode_t* interactionmode = reinterpret_cast<eOmc_interactionmode_t*>(ropdescriptor.rd->data);
//                MController_set_interaction_mode(index, *interactionmode);      
//                r = true;                
//            } break;
//            
//            
//            // the unmanaged tags
//            
//            case eoprot_tag_mc_joint_wholeitem: // = 0 and never used
//                
//            #warning: some tags in eoprot_tag_mc_joint_config are duplicated ... why is it?
//            // case eoprot_tag_mc_joint_config_pidtrajectory: // = 2 as position ????
//            // case eoprot_tag_mc_joint_config_piddirect: // = 3 as velocity ???
//            case eoprot_tag_mc_joint_config_tcfiltertype: //= 8
//                
//            case eoprot_tag_mc_joint_status: // = 9
//            case eoprot_tag_mc_joint_status_core: // 10
//            case eoprot_tag_mc_joint_status_target:
//            case eoprot_tag_mc_joint_status_core_modes_controlmodestatus:
//            case eoprot_tag_mc_joint_status_core_modes_interactionmodestatus: 
//            case eoprot_tag_mc_joint_status_core_modes_ismotiondone: 
//            case eoprot_tag_mc_joint_status_addinfo_multienc:   
//            case eoprot_tag_mc_joint_status_debug:                
//            
//            case eoprot_tag_mc_joint_inputs:
//                
//            default:
//            {
//                // add a print that tells that a command is not managed
//            } break;
//        }
//        
//    }
//    else if(eomc_entity_motor == entity)
//    {
//        // process the motor tags
//        switch(static_cast<eOprot_tag_mc_motor_t>(tag))
//        {
//            
//            case eoprot_tag_mc_motor_config:
//            {   // 1
//                eOmc_motor_config_t* motorcfg = reinterpret_cast<eOmc_motor_config_t*>(ropdescriptor.rd->data);
//                MController_config_motor(index, motorcfg);                
//                r = true;                
//            } break;            

//            case eoprot_tag_mc_motor_config_currentlimits:
//            {
//                eOmc_current_limits_params_t *cl = reinterpret_cast<eOmc_current_limits_params_t*>(ropdescriptor.rd->data);
//                MController_motor_config_max_currents(index, cl);                
//                r = true;                
//            } break;   

//            case eoprot_tag_mc_motor_config_gearbox_M2J:
//            {   // but never called by YRI
//                float32_t* g = reinterpret_cast<float32_t*>(ropdescriptor.rd->data);
//                MController_config_motor_gearbox_M2J(index, *g);
//                
//                r = true;                
//            } break;   

//            case eoprot_tag_mc_motor_config_rotorencoder:
//            {   // rotor encoder resolution
//                int32_t* rotencres = reinterpret_cast<int32_t*>(ropdescriptor.rd->data);
//                MController_config_motor_encoder(index, *rotencres);                
//                r = true;                
//            } break;   

//            case eoprot_tag_mc_motor_config_pwmlimit:
//            {
//                eOmeas_pwm_t* pwmlimit = reinterpret_cast<eOmeas_pwm_t*>(ropdescriptor.rd->data);
//                *pwmlimit = MController_config_motor_pwm_limit(index, *pwmlimit);
//                
//                r = true;                
//            } break;   
// 
//            case eoprot_tag_mc_motor_config_temperaturelimit:
//            {
//                // shall we use it also for the amcbldc etc?
//                eOmeas_temperature_t* temperatureLimit = reinterpret_cast<eOmeas_temperature_t*>(ropdescriptor.rd->data);
//                MController_motor_config_max_temperature(index, temperatureLimit);
//                
//                r = true;                
//            } break;   

//            case eoprot_tag_mc_motor_config_pidcurrent:
//            {
//                eOmc_PID_t* pid = reinterpret_cast<eOmc_PID_t*>(ropdescriptor.rd->data);
//                MController_motor_config_current_PID(index, pid);
//                
//                r = true;                
//            } break;   
//            
//            case eoprot_tag_mc_motor_config_pidspeed:
//            {
//                eOmc_PID_t* pid = reinterpret_cast<eOmc_PID_t*>(ropdescriptor.rd->data);
//                MController_motor_config_speed_PID(index, pid);
//                
//                r = true;                
//            } break;  

//            
//            // the unmanaged tags
//            case eoprot_tag_mc_motor_wholeitem: // = 0 and never used
//                
//            default:
//            {
//                // add a print that tells that a command is not managed
//            } break;
//        }
//        
//    }

//    
////    eOmc_joint_config_t *cfg = (eOmc_joint_config_t*)rd->data;
////    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
//////    eOmc_joint_status_t *jstatus = eo_entities_GetJointStatus(eo_entities_GetHandle(), jxx);
////    
////    // now we see if it is a mc4can or a 2foc or a mc4plus    
////    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();

////    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_advfoc == mcmode))
////    {
////        MController_config_joint(jxx, cfg);
////    }    
//    
//    // #warning TODO: add  embot::app::eth::theMCservice::getInstance().process(ropdescriptor.rd, ropdescriptor.nv);
//    return true;
}


// --------------------------------------------------------------------------------------------------------------------
// - private methods of Impl
// --------------------------------------------------------------------------------------------------------------------


//bool embot::app::eth::theServiceMC::Impl::p_encodersAcquire()
//{ 
//    bool res {true};
//    
//    size_t numofjomos = _mcservice.mcobjects.numberofjomos();
//    
//    // wait for the encoders for some time
//    for (uint8_t i=0; i<30; ++i)
//    {
//        if(true == embot::app::eth::theEncoderReader::getInstance().IsReadingAvailable())
//        {
//            break;
//        }
//        embot::core::wait(5*embot::core::time1microsec);
//    }        
//            
//    if(false == embot::app::eth::theEncoderReader::getInstance().IsReadingAvailable())
//    {
//        for(uint8_t i=0; i<numofjomos; i++)
//        {
//            MController_timeout_absEncoder_fbk(i);
//        }
//        
//        return false;
//    }
//    
//    // read the encoders        
//    for(uint8_t i=0; i<numofjomos; i++)
//    {
//        embot::app::eth::encoder::v1::valueInfo encoder1valueinfo {};
//        embot::app::eth::encoder::v1::valueInfo encoder2valueinfo {};

//        embot::app::eth::theEncoderReader::getInstance().Read(i, encoder1valueinfo, encoder2valueinfo);
//                   
//        if(true != p_encodersApplyValue(i, &encoder1valueinfo))
//        {
//            res = false;
//        }
//        
//        if(true != p_encodersApplyValue(i, &encoder2valueinfo))
//        {
//            res = false;
//        }
//        
//    } 
//    
//    embot::app::eth::theEncoderReader::getInstance().Diagnostics_Tick();
//    
//    // and finally start reading again
//    embot::app::eth::theEncoderReader::getInstance().StartReading();
//    
//    return res;

//}

//bool embot::app::eth::theServiceMC::Impl::p_encodersApplyValue(uint8_t index, embot::app::eth::encoder::v1::valueInfo *valueinfo)
//{
//    bool res = true;
//    
//    if(valueinfo->errortype == embot::app::eth::encoder::v1::Error::NOTCONNECTED) 
//    {
//        return res;
//    }
//        
//    if(valueinfo->placement == embot::app::eth::encoder::v1::Placement::atjoint)
//    {
//        if(valueinfo->errortype != embot::app::eth::encoder::v1::Error::NONE)
//        {
//          
//            MController_invalid_absEncoder_fbk(index, embot::core::tointegral(valueinfo->errortype));
//            res = false;
//        }
//        else
//        { 
//            // marco.accame on 03jul2023: MController_update_absEncoder_fbk() needs uint32_t* BUT MController_update_motor_pos_fbk() asks for an int32_t value            
//            MController_update_absEncoder_fbk(index, valueinfo->value);
//        }
//    }
//    else if(valueinfo->placement == embot::app::eth::encoder::v1::Placement::atmotor)
//    {
//        // marco.accame on 03jul2023: MController_update_absEncoder_fbk() needs uint32_t* BUT MController_update_motor_pos_fbk() asks for an int32_t value
//        MController_update_motor_pos_fbk(index, static_cast<int32_t>(valueinfo->value[0]));
//    }
//    else
//    {
//        //do nothing;
//    }
//    return res;
//} 

//void embot::app::eth::theServiceMC::Impl::p_JOMOupdatestatus()
//{
//    const uint8_t transmit_decoupled_pwms = 0;
//    
//    size_t numofjomos = _mcservice.numberofjomos();
//        

//    for(uint8_t jId = 0; jId<numofjomos; jId++)
//    {
//        eOmc_joint_status_t *jstatus = &_mcservice.joint(jId)->status;

//        MController_get_joint_state(jId, jstatus);            
//        MController_get_pid_state(jId, &jstatus->core.ofpid, transmit_decoupled_pwms);
//    }   
//    
//    for(uint8_t jId = 0; jId<numofjomos; jId++)
//    {
//        eOmc_motor_status_t *mstatus = &_mcservice.motor(jId)->status;
//        MController_get_motor_state(jId, mstatus);
//    }
//}

//bool embot::app::eth::theServiceMC::Impl::p_applysetpoint(eOprotIndex_t jointindex, eOmc_setpoint_t *setpoint)
//{   
//    if(jointindex >= _mcservice.numberofjomos())
//    {
//        return false;
//    }
//    
//    eOmc_joint_t *joint = _mcservice.joint(jointindex);
//    
//    if(nullptr == joint)
//    {
//        return false;
//    }
//    
//    // at first we clear this
//    joint->status.core.modes.ismotiondone = eobool_false;
//    
//    // then we apply the setpoint
//    
//    switch(setpoint->type)
//    { 
//        case eomc_setpoint_position:
//        {
//            MController_set_joint_pos_ref(jointindex, setpoint->to.position.value, setpoint->to.position.withvelocity);
//        } break;
//        
//        case eomc_setpoint_positionraw:
//        {
//            MController_set_joint_pos_raw(jointindex, setpoint->to.positionraw.value);
//        } break;
//        
//        case eomc_setpoint_velocity:
//        {
//            MController_set_joint_vel_ref(jointindex, setpoint->to.velocity.value, setpoint->to.velocity.withacceleration);
//        } break;

//        case eomc_setpoint_torque:
//        {
//            MController_set_joint_trq_ref(jointindex, setpoint->to.torque.value);
//        } break;

//        case eomc_setpoint_openloop:
//        {
//            MController_set_joint_pwm_ref(jointindex, setpoint->to.openloop.value);
//        } break;

//        case eomc_setpoint_current:
//        {
//            MController_set_joint_cur_ref(jointindex, setpoint->to.current.value);
//        } break;
//        
//        default:
//        {
//            
//        } break;
//    }
//    
//    // then i internally update the relevant joint->status.target
//    MController_update_joint_targets(jointindex);    
//    
//    return true;
//}


//uint8_t embot::app::eth::theServiceMC::Impl::p_getentityindex(eOprot_entity_t entity, const DescriptorCANframe &canframedescriptor)
//{
//    uint8_t ret {EOK_uint08dummy};
//    
//    eObrd_canlocation_t loc = {0};

//    loc.port = canframedescriptor.port;
//    loc.addr = EOCANPROT_FRAME_GET_SOURCE(canframedescriptor.frame);    
//    loc.insideindex = eobrd_caninsideindex_first;
//    
//    ret = eo_canmap_GetEntityIndex(eo_canmap_GetHandle(), loc, eoprot_endpoint_motioncontrol, entity);
//    
//    return(ret);   
//}

bool embot::app::eth::theServiceMC::Impl::p_isID32relevant(uint32_t id32)
{
    static const uint32_t mask0 = (((uint32_t)eoprot_endpoint_motioncontrol) << 24) | (((uint32_t)eoprot_entity_mc_joint) << 16);
    static const uint32_t mask1 = (((uint32_t)eoprot_endpoint_motioncontrol) << 24) | (((uint32_t)eoprot_entity_mc_motor) << 16);
    static const uint32_t mask2 = (((uint32_t)eoprot_endpoint_motioncontrol) << 24) | (((uint32_t)eoprot_entity_mc_controller) << 16);
    
    if((id32 & mask0) == mask0)
    {
        return(true);
    }
    if((id32 & mask1) == mask1)
    {
        return(true);
    } 
    if((id32 & mask2) == mask2)
    {
        return(true);
    }    
    
    return(false); 
}

} // namespace embot::app::eth {

// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::eth::theServiceMC& embot::app::eth::theServiceMC::getInstance()
{
    static theServiceMC* p = new theServiceMC();
    return *p;
}

embot::app::eth::theServiceMC::theServiceMC()
{
    pImpl = std::make_unique<Impl>(this);
}  

    
embot::app::eth::theServiceMC::~theServiceMC() { }
        

bool embot::app::eth::theServiceMC::initialise(const Config &config)
{
    pImpl->_config = config;
    return pImpl->initialise();
}

embot::app::eth::Service* embot::app::eth::theServiceMC::service()
{
    return this;
}

embot::app::eth::Service::Category embot::app::eth::theServiceMC::category() const 
{
    return pImpl->category();
}

embot::app::eth::Service::Type embot::app::eth::theServiceMC::type() const
{
    return pImpl->type();
}

embot::app::eth::Service::State embot::app::eth::theServiceMC::state() const
{
    return pImpl->state();
}

void embot::app::eth::theServiceMC::set(embot::app::eth::Service::State s)
{
    pImpl->set(s);
}

bool embot::app::eth::theServiceMC::verifyactivate(const embot::app::eth::Service::Config * servcfg, OnEndOfOperation onendofverifyactivate)
{
    return pImpl->verifyactivate(servcfg, onendofverifyactivate);
}

bool embot::app::eth::theServiceMC::deactivate()
{
    return pImpl->deactivate();
}

bool embot::app::eth::theServiceMC::start()
{
    return pImpl->start();
}

bool embot::app::eth::theServiceMC::stop()
{
    return pImpl->stop();
}

bool embot::app::eth::theServiceMC::setregulars(const eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t& numberofthem)
{
    return pImpl->set(arrayofid32, numberofthem);
}

bool embot::app::eth::theServiceMC::tick(bool resetstatus)
{
    return pImpl->tick(resetstatus);
}

bool embot::app::eth::theServiceMC::report()
{
    return pImpl->report();
}

bool embot::app::eth::theServiceMC::process(const DescriptorCANframe &canframedescriptor)
{
    return pImpl->process(canframedescriptor);
}

bool embot::app::eth::theServiceMC::process(const DescriptorROP &ropdescriptor)
{
    return pImpl->process(ropdescriptor);
}


// other objects




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


