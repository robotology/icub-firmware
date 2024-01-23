
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

#include "embot_prot_can_motor_periodic.h"

#include "EOtheCANprotocol.h"
#include "EOtheCANmapping.h"

#include "EOtheMotionController.h"

#include "embot_app_eth_theErrorManager.h"
#include "embot_os_theScheduler.h"

#include "EoProtocolMC.h"
#include "EOtheEntities.h"
#include "Controller.h"
#include "embot_app_eth_theEncoderReader.h"
#include "embot_app_eth_theICCmapping.h"
#include "embot_app_eth_theServices.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


namespace embot::app::eth {

// finally the implementation struct
struct embot::app::eth::theServiceMC::Impl
{ 
        // to be removed:
        EOtheMotionController *_eom {nullptr};
        
    theServiceMC *_owner {nullptr};   
    
    theServiceMC::Config _config {}; // so far empty
        
    
    MCservice _mcservice {};
    

    embot::app::eth::service::impl::ServiceOperation2 _operation2 {};
        
    
    // methods used by theServiceMC 
    
    Impl(theServiceMC *o) : _owner(o) {};   
 
    
    // interface to Service
        
    bool initialise();
        
    Category category() const { return embot::app::eth::Service::Category::mc; }
    Type type() const { return _mcservice.mcobjects.mcservicetype; }
    State state() const { return _mcservice.core.paraphernalia.state; }
    void set(State s) { _mcservice.core.paraphernalia.state = s; }
    
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
   
    static constexpr char objname[] = "theServiceMC";

    void traceprint(const char *str)
    {
        embot::app::eth::theErrorManager::getInstance().trace(str, {objname, embot::os::theScheduler::getInstance().scheduled()}); 
    }  
    
    bool applysetpoint(eOprotIndex_t jointindex, eOmc_setpoint_t *setpoint);
    void* s_eocanprotMCperiodic_get_entity(eOprot_entity_t entity, eOcanframe_t *frame, eOcanport_t port, eObrd_caninsideindex_t insideindex, uint8_t *index);

    bool encodersAcquire();
    bool encodersApplyValue(uint8_t index, embot::app::eth::encoder::v1::valueInfo *valueinfo);
    void JOMOupdatestatus();
    
    static bool isID32relevant(uint32_t id32)
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

    static void cbk_afterverifyactivate(void* p, eObool_t operationisok)
    {
        // the embobj service executes a callback where we passed p as Impl * and operationisok contains success of operation
        // remember that the callback is called after verification is OK or when verification is OK and activation is already done
        // in any case, the callback should signal to YRI the result of the verify() operation     
        Impl *impl = reinterpret_cast<Impl*>(p);
        impl->_operation2.onend(static_cast<bool>(operationisok));  
    }   

    
};

bool embot::app::eth::theServiceMC::Impl::encodersAcquire()
{ 
    bool res {true};
    
    size_t numofjomos = _mcservice.mcobjects.numofjomos;
    
    // wait for the encoders for some time
    for (uint8_t i=0; i<30; ++i)
    {
        if(true == embot::app::eth::theEncoderReader::getInstance().IsReadingAvailable())
        {
            break;
        }
        embot::core::wait(5*embot::core::time1microsec);
    }        
            
    if(false == embot::app::eth::theEncoderReader::getInstance().IsReadingAvailable())
    {
        for(uint8_t i=0; i<numofjomos; i++)
        {
            MController_timeout_absEncoder_fbk(i);
        }
        
        return false;
    }
    
    // read the encoders        
    for(uint8_t i=0; i<numofjomos; i++)
    {
        embot::app::eth::encoder::v1::valueInfo encoder1valueinfo {};
        embot::app::eth::encoder::v1::valueInfo encoder2valueinfo {};

        embot::app::eth::theEncoderReader::getInstance().Read(i, encoder1valueinfo, encoder2valueinfo);
                   
        if(true != encodersApplyValue(i, &encoder1valueinfo))
        {
            res = false;
        }
        
        if(true != encodersApplyValue(i, &encoder2valueinfo))
        {
            res = false;
        }
        
    } 
    
    embot::app::eth::theEncoderReader::getInstance().Diagnostics_Tick();
    
    // and finally start reading again
    embot::app::eth::theEncoderReader::getInstance().StartReading();
    
    return res;

}

bool embot::app::eth::theServiceMC::Impl::encodersApplyValue(uint8_t index, embot::app::eth::encoder::v1::valueInfo *valueinfo)
{
    bool res = true;
    
    if(valueinfo->errortype == embot::app::eth::encoder::v1::Error::NOTCONNECTED) 
    {
        return res;
    }
        
    if(valueinfo->placement == embot::app::eth::encoder::v1::Placement::atjoint)
    {
        if(valueinfo->errortype != embot::app::eth::encoder::v1::Error::NONE)
        {
          
            MController_invalid_absEncoder_fbk(index, embot::core::tointegral(valueinfo->errortype));
            res = false;
        }
        else
        { 
            // marco.accame on 03jul2023: MController_update_absEncoder_fbk() needs uint32_t* BUT MController_update_motor_pos_fbk() asks for an int32_t value            
            MController_update_absEncoder_fbk(index, valueinfo->value);
        }
    }
    else if(valueinfo->placement == embot::app::eth::encoder::v1::Placement::atmotor)
    {
        // marco.accame on 03jul2023: MController_update_absEncoder_fbk() needs uint32_t* BUT MController_update_motor_pos_fbk() asks for an int32_t value
        MController_update_motor_pos_fbk(index, static_cast<int32_t>(valueinfo->value[0]));
    }
    else
    {
        //do nothing;
    }
    return res;
} 

void embot::app::eth::theServiceMC::Impl::JOMOupdatestatus()
{
    const uint8_t transmit_decoupled_pwms = 0;
    
    size_t numofjomos = _mcservice.mcobjects.numofjomos;
    
    eOmc_joint_status_t *jstatus = NULL;
    eOmc_motor_status_t *mstatus = NULL;
    
    uint8_t jId = 0;
        
    for(jId = 0; jId<numofjomos; jId++)
    {
        eOmc_joint_status_t *jstatus = &_mcservice.mcobjects.joints[jId]->status;

        MController_get_joint_state(jId, jstatus);            
        MController_get_pid_state(jId, &jstatus->core.ofpid, transmit_decoupled_pwms);
    }   
    
    for(jId = 0; jId<numofjomos; jId++)
    {
        eOmc_motor_status_t *mstatus = &_mcservice.mcobjects.motors[jId]->status;
        MController_get_motor_state(jId, mstatus);
    }
}

bool embot::app::eth::theServiceMC::Impl::applysetpoint(eOprotIndex_t jointindex, eOmc_setpoint_t *setpoint)
{   
    if(jointindex >= _mcservice.mcobjects.joints.size())
    {
        return false;
    }
    
    eOmc_joint_t *joint = _mcservice.mcobjects.joints[jointindex];
    
    if(nullptr == joint)
    {
        return false;
    }
    
    // at first we clear this
    joint->status.core.modes.ismotiondone = eobool_false;
    
    // then we apply the setpoint
    
    switch(setpoint->type)
    { 
        case eomc_setpoint_position:
        {
            MController_set_joint_pos_ref(jointindex, setpoint->to.position.value, setpoint->to.position.withvelocity);
        } break;
        
        case eomc_setpoint_positionraw:
        {
            MController_set_joint_pos_raw(jointindex, setpoint->to.positionraw.value);
        } break;
        
        case eomc_setpoint_velocity:
        {
            MController_set_joint_vel_ref(jointindex, setpoint->to.velocity.value, setpoint->to.velocity.withacceleration);
        } break;

        case eomc_setpoint_torque:
        {
            MController_set_joint_trq_ref(jointindex, setpoint->to.torque.value);
        } break;

        case eomc_setpoint_openloop:
        {
            MController_set_joint_pwm_ref(jointindex, setpoint->to.openloop.value);
        } break;

        case eomc_setpoint_current:
        {
            MController_set_joint_cur_ref(jointindex, setpoint->to.current.value);
        } break;
        
        default:
        {
            
        } break;
    }
    
    // then i internally update the relevant joint->status.target
    MController_update_joint_targets(jointindex);    
    
    return true;
}

void* embot::app::eth::theServiceMC::Impl::s_eocanprotMCperiodic_get_entity(eOprot_entity_t entity, eOcanframe_t *frame, eOcanport_t port, eObrd_caninsideindex_t insideindex, uint8_t *index)
{
    void * ret = NULL;
    uint8_t ii = 0;
    eObrd_canlocation_t loc = {0};
    
    loc.port = port;
    loc.addr = EOCANPROT_FRAME_GET_SOURCE(frame);    
    loc.insideindex = insideindex;
    
    ii = eo_canmap_GetEntityIndex(eo_canmap_GetHandle(), loc, eoprot_endpoint_motioncontrol, entity);
    
    if(EOK_uint08dummy == ii)
    {     
        //#warning -> TODO: add diagnostics about not found board as in s_eo_icubCanProto_mb_send_runtime_error_diagnostics()
        return(NULL);
    }
    
    ret = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, entity, ii);
    
    if(NULL != index)
    {
        *index = ii;        
    }  

    return(ret);   
}

      
bool embot::app::eth::theServiceMC::Impl::initialise()
{
    traceprint("initialise()");

    if(true == _mcservice.core.paraphernalia.initted)
    {
        return true;
    }
    
    // i manage up to 4 different boards, 4 can targets, 4 entities
    constexpr size_t nboards {maxJOMOs};
    constexpr size_t ntargets {maxJOMOs};
    constexpr size_t nentities {maxJOMOs};
    constexpr size_t maxregs {maxREGULARs};
    _mcservice.initialise(nboards, ntargets, nentities, maxregs);
    
    // controller
    constexpr size_t eo_motcon_standardENCOs {6}; // as in legacy code.
    MController_new(maxJOMOs, eo_motcon_standardENCOs);
    
    // encoder reader
    embot::app::eth::theEncoderReader::getInstance().initialise();
           
    // synch service
    _mcservice.synch();

    return true;
}


bool embot::app::eth::theServiceMC::Impl::verifyactivate(const embot::app::eth::Service::Config * servcfg, OnEndOfOperation onendofverifyactivate)
{
    traceprint("verifyactivate()");
    
//    _operation2.load(_owner, servcfg, onendofverifyactivate);
//    const eOmn_serv_configuration_t * eomnservcfg = reinterpret_cast<const eOmn_serv_configuration_t *>(servcfg->memory());
//    eo_motioncontrol_Verify2(_eom, eomnservcfg, cbk_afterverifyactivate, this, eobool_true);   
    
    
    // i would ... allow this funtion only if we have not started .... or activated . in such a case we call deactivate().
    
    if(true == _mcservice.active())
    {
        deactivate();        
    }         
      
    
    if((nullptr == servcfg) || (embot::app::eth::Service::Config::Type::eOmn_serv != servcfg->type()))
    {
        _mcservice.state(embot::app::eth::Service::State::failureofverify);    
        // _mcservice.started(false);  
        _mcservice.synch();        
        if(nullptr != onendofverifyactivate)
        {
            onendofverifyactivate(_owner, false); 
        }        
        return false;
    }
    

    const eOmn_serv_configuration_t * svc =  reinterpret_cast<const eOmn_serv_configuration_t*>(servcfg->memory());
      
    const embot::app::eth::Service::Type t {static_cast<const embot::app::eth::Service::Type>(svc->type)};
    
    if(false == _mcservice.supported(t))  
    {
        traceprint("verifyactivate() has unrecognised type");
        _mcservice.state(embot::app::eth::Service::State::failureofverify);    
        // _mcservice.started(false);  
        _mcservice.synch();        
        if(nullptr != onendofverifyactivate)
        {
            onendofverifyactivate(_owner, false); 
        }        
        return false;
    }
    
    // all the rest
    
    // ok, i can verify
    
    // assign the proper state
    _mcservice.state(embot::app::eth::Service::State::verifying);    
    _mcservice.synch();    

    // make a local copy of the service configuration
    // process the foc mode
    _mcservice.load(_owner, svc);
    
    _mcservice.verifyactivate(onendofverifyactivate);
    
    return true;
}

bool embot::app::eth::theServiceMC::Impl::deactivate()
{
    traceprint("deactivate()");
        
    if(false == _mcservice.active())
    {
        _mcservice.clear(); 
        _mcservice.synch();
        return true;        
    } 
        
    
    // we stop the service
    if(true == _mcservice.started())
    {
        Impl::stop();   
    }    

    // and we clear the regulars
    uint8_t n {0};
    Impl::set(nullptr, n);  
    
    // then we call the proper deactivate of the mcservice
    _mcservice.deactivate();
    
    return true;
}

bool embot::app::eth::theServiceMC::Impl::start()
{
    traceprint("start()");
       
    if(false == _mcservice.active())
    {   // nothing to do because object must be first activated
        return true;
    } 
        
    if(true == _mcservice.started())
    {   // it is already started
        return true;
    }
    
    // check if the type is correct ... NO NEED to do it because that is already done in verifyactivate()
    
//    if((eo_motcon_mode_foc != p->service.servconfig.type) && (eo_motcon_mode_advfoc != p->service.servconfig.type))
//    {
//        embot::core::print("[ERROR] eo_motioncontrol_Start: unrecognized servconfig.type");
//        eo_errman_Trace(eo_errman_GetHandle(), "eo_motioncontrol_Start()", s_eobj_ownname);  
//        return(eores_OK);        
//    }
       
    // now i set the state
    _mcservice.state(embot::app::eth::Service::State::started);    
    _mcservice.started(true);  
    _mcservice.synch();
    
    // we just need to start a read of the encoder, nothing else 
    embot::app::eth::theEncoderReader::getInstance().StartReading();

        
    return true;
}

bool embot::app::eth::theServiceMC::Impl::stop()
{
    traceprint("stop()");
    
    // eo_motioncontrol_Stop(_eom);
    
    if(false == _mcservice.active())
    {   // nothing to do because object must be first activated
        return true;
    } 
        
    if(false == _mcservice.started())
    {   // it is already stopped
        return true;
    }
    
    // i idle the controller
   
    MController_go_idle();

    // and i update the state: i revert to activate
    _mcservice.state(embot::app::eth::Service::State::activated);    
    _mcservice.started(false);  
    _mcservice.synch();
       
    return true;
}

bool embot::app::eth::theServiceMC::Impl::set(const eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t& numberofthem)
{
    traceprint("set(ID32)");
    
    if(false == _mcservice.active())
    {   // nothing to do because object must be first activated
        return true;
    } 

    bool r = embot::app::eth::theServices::getInstance().setREGULARS(_mcservice.core.id32ofRegulars, arrayofid32, isID32relevant, &numberofthem);
    
    return r;  
}

bool embot::app::eth::theServiceMC::Impl::tick(bool resetstatus)
{
    traceprint("tick()");
    
    // eo_motioncontrol_Tick(_eom);

    if(false == _mcservice.active())
    {   // nothing to do because object must be first activated
        return true;
    } 
        
    if(false == _mcservice.started())
    {   // not yet started
        return true;
    }
     
    
    // 1) manage encoders
    encodersAcquire();

    embot::app::eth::theEncoderReader::getInstance().StartReading();
    
    // 2) perform motor control  
    MController_do();
    
    // 3) update status of motors and joints
    JOMOupdatestatus();  
    
    return true;
}

bool embot::app::eth::theServiceMC::Impl::report()
{
    traceprint("report()");
    #warning TODO: add embot::app::eth::theServiceMC::Impl::report()
    //eo_motioncontrol_SendReport(_eom);
    return true;
}

bool embot::app::eth::theServiceMC::Impl::process(const DescriptorCANframe &canframedescriptor)
{    
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
    
    embot::prot::can::Frame frame {canframedescriptor.frame->id, canframedescriptor.frame->size, canframedescriptor.frame->data};    
    embot::prot::can::Clas cls = embot::prot::can::frame2clas(frame);
    
    // we only accept commands of class periodicMotorControl   
    if(embot::prot::can::Clas::periodicMotorControl != cls)
    {
        return r;
    }
    
    embot::prot::can::motor::periodic::CMD cmd = static_cast<embot::prot::can::motor::periodic::CMD>(embot::prot::can::frame2cmd(frame));

//    eOmc_motor_t *motor {nullptr};
//    eOprotIndex_t motorindex {EOK_uint08dummy};
//    if(nullptr == (motor = (eOmc_motor_t*) s_eocanprotMCperiodic_get_entity(eoprot_entity_mc_motor, canframedescriptor.frame, canframedescriptor.port, eobrd_caninsideindex_first, &motorindex)))
//    { 
//        return r;
//    }

    #warning -> in here could get the joint / motor by just getting the index and not w/ s_eocanprotMCperiodic_get_entity()
        
    switch(cmd)
    {
        case embot::prot::can::motor::periodic::CMD::FOC:
        {
            eOmc_motor_t *motor {nullptr};
            eOprotIndex_t motorindex {EOK_uint08dummy};
            if(nullptr != (motor = (eOmc_motor_t*) s_eocanprotMCperiodic_get_entity(eoprot_entity_mc_motor, canframedescriptor.frame, canframedescriptor.port, eobrd_caninsideindex_first, &motorindex)))
            {    
                r = true;
                
                // note of marco.accame: the following code is ok as long as the 2foc has been configured to send up in its periodic message 
                // current, velocity, and position. if so, frame->data contains: [current:2bytes, velocity:2bytes, position:4bytes]. 
                // the following code extract these values. 
                motor->status.basic.mot_current  = ((int16_t*)canframedescriptor.frame->data)[0];
                motor->status.basic.mot_velocity = ((int16_t*)canframedescriptor.frame->data)[1];
                motor->status.basic.mot_position = ((int32_t*)canframedescriptor.frame->data)[1];

                MController_update_motor_odometry_fbk_can(motorindex, canframedescriptor.frame->data); 
            }                
        } break;

        case embot::prot::can::motor::periodic::CMD::STATUS:
        {
            eOmc_joint_t *joint {nullptr};
            eOprotIndex_t jointindex {EOK_uint08dummy};
            if(nullptr != (joint = (eOmc_joint_t*) s_eocanprotMCperiodic_get_entity(eoprot_entity_mc_joint, canframedescriptor.frame, canframedescriptor.port, eobrd_caninsideindex_first, &jointindex)))
            {    
                r = true;
                MController_update_motor_state_fbk(jointindex, canframedescriptor.frame->data); 
            }                
        } break;
        
        default:
        {
        } break;
    }
    
    
    return true;
}

bool embot::app::eth::theServiceMC::Impl::process(const DescriptorROP &ropdescriptor)
{
    traceprint("process(ROPdes)");
    
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
    
    eOprotEntity_t entity = eoprot_ID2entity(ropdescriptor.rd->id32);
    eOprotIndex_t index = eoprot_ID2index(ropdescriptor.rd->id32);
    eOprotTag_t tag = eoprot_ID2tag(ropdescriptor.rd->id32);
    
    
    if(eomc_entity_joint == entity)
    {
        // process the joint tags
        switch(static_cast<eOprot_tag_mc_joint_t>(tag))
        {
            // .config section
            // almost every tag is managed BUT ... I believe that YRI sends ony set<eoprot_tag_mc_joint_config, data>
            // so, we may prune the code
            
            case eoprot_tag_mc_joint_config: 
            {   // 1
                eOmc_joint_config_t *jcfg = reinterpret_cast<eOmc_joint_config_t*>(ropdescriptor.rd->data);
                MController_config_joint(index, jcfg);   
                r = true;
            } break;

            case eoprot_tag_mc_joint_config_pidposition:
            {   // 2
                eOmc_PID_t *pid = reinterpret_cast<eOmc_PID_t*>(ropdescriptor.rd->data);
                MController_config_minjerk_pid(index, pid);   
                r = true;
            } break;

            case eoprot_tag_mc_joint_config_pidvelocity:
            {   // 3
                eOmc_PID_t *pid = reinterpret_cast<eOmc_PID_t*>(ropdescriptor.rd->data);
                // apparently eoprot_fun_UPDT_mc_joint_config_pidvelocity() originally does not call it
                // MController_config_direct_pid(index, pid);   
                r = true;
            } break;

            case eoprot_tag_mc_joint_config_pidtorque:
            {   // 4
                eOmc_PID_t *pid = reinterpret_cast<eOmc_PID_t*>(ropdescriptor.rd->data);
                MController_motor_config_torque_PID(index, pid);   
                r = true;
            } break;

            case eoprot_tag_mc_joint_config_userlimits:
            {   // 5
                eOmeas_position_limits_t *limitsofjoint = reinterpret_cast<eOmeas_position_limits_t*>(ropdescriptor.rd->data);
                MController_config_joint_pos_limits(index, limitsofjoint->min, limitsofjoint->max);   
                r = true;
            } break;

            case eoprot_tag_mc_joint_config_impedance:
            {   // 6
                eOmc_impedance_t *impedance = reinterpret_cast<eOmc_impedance_t*>(ropdescriptor.rd->data);
                MController_config_joint_impedance(index, impedance);
                r = true;
            } break;
            
            case eoprot_tag_mc_joint_config_motor_params:
            {   // 7
                eOmc_motor_params_t *mparams = reinterpret_cast<eOmc_motor_params_t*>(ropdescriptor.rd->data);
                MController_config_motor_friction(index, mparams);
                r = true;
            } break;
            
            // .status section
            // nothing is managed in here because:
            // 1. status is signalled by the oard to YRI and never asked
            // 2. the exception was case eoprot_tag_mc_joint_status_core_modes_ismotiondone
            //    that was used only for eo_motcon_mode_mc4 that is not managed in here

            // .input section
            //  only eoprot_tag_mc_joint_inputs_externallymeasuredtorque
            
            case eoprot_tag_mc_joint_inputs_externallymeasuredtorque:
            {   // 18
                eOmeas_torque_t *torque = reinterpret_cast<eOmeas_torque_t*>(ropdescriptor.rd->data);
                MController_update_joint_torque_fbk(index, *torque);
                r = true;                
            } break;
            
            // .cmmnds section
            // 
            
            case eoprot_tag_mc_joint_cmmnds_calibration:
            {   // 19
                eOmc_calibrator_t *calibrator = reinterpret_cast<eOmc_calibrator_t*>(ropdescriptor.rd->data);
                MController_calibrate(index, calibrator);
                r = true;                
            } break;

            case eoprot_tag_mc_joint_cmmnds_setpoint:
            {   // 20
                eOmc_setpoint_t *setpoint = reinterpret_cast<eOmc_setpoint_t*>(ropdescriptor.rd->data);
                r = applysetpoint(index, setpoint);               
            } break;

            case eoprot_tag_mc_joint_cmmnds_stoptrajectory:
            {   // 21
                MController_stop_joint(index);      
                r = true;                
            } break;
            
            case eoprot_tag_mc_joint_cmmnds_controlmode:
            {   // 22
                eOmc_controlmode_command_t *controlmode = reinterpret_cast<eOmc_controlmode_command_t*>(ropdescriptor.rd->data);
                MController_set_control_mode(index, *controlmode);      
                r = true;                
            } break;
            
            case eoprot_tag_mc_joint_cmmnds_interactionmode:
            {   // 23
                eOmc_interactionmode_t* interactionmode = reinterpret_cast<eOmc_interactionmode_t*>(ropdescriptor.rd->data);
                MController_set_interaction_mode(index, *interactionmode);      
                r = true;                
            } break;
            
            
            // the unmanaged tags
            
            case eoprot_tag_mc_joint_wholeitem: // = 0 and never used
                
            #warning: some tags in eoprot_tag_mc_joint_config are duplicated ... why is it?
            // case eoprot_tag_mc_joint_config_pidtrajectory: // = 2 as position ????
            // case eoprot_tag_mc_joint_config_piddirect: // = 3 as velocity ???
            case eoprot_tag_mc_joint_config_tcfiltertype: //= 8
                
            case eoprot_tag_mc_joint_status: // = 9
            case eoprot_tag_mc_joint_status_core: // 10
            case eoprot_tag_mc_joint_status_target:
            case eoprot_tag_mc_joint_status_core_modes_controlmodestatus:
            case eoprot_tag_mc_joint_status_core_modes_interactionmodestatus: 
            case eoprot_tag_mc_joint_status_core_modes_ismotiondone: 
            case eoprot_tag_mc_joint_status_addinfo_multienc:   
            case eoprot_tag_mc_joint_status_debug:                
            
            case eoprot_tag_mc_joint_inputs:
                
            default:
            {
                // add a print that tells that a command is not managed
            } break;
        }
        
    }
    else if(eomc_entity_motor == entity)
    {
        // process the motor tags
        switch(static_cast<eOprot_tag_mc_motor_t>(tag))
        {
            
            case eoprot_tag_mc_motor_config:
            {   // 1
                eOmc_motor_config_t* motorcfg = reinterpret_cast<eOmc_motor_config_t*>(ropdescriptor.rd->data);
                MController_config_motor(index, motorcfg);                
                r = true;                
            } break;            

            case eoprot_tag_mc_motor_config_currentlimits:
            {
                eOmc_current_limits_params_t *cl = reinterpret_cast<eOmc_current_limits_params_t*>(ropdescriptor.rd->data);
                MController_motor_config_max_currents(index, cl);                
                r = true;                
            } break;   

            case eoprot_tag_mc_motor_config_gearbox_M2J:
            {   // but never called by YRI
                float32_t* g = reinterpret_cast<float32_t*>(ropdescriptor.rd->data);
                MController_config_motor_gearbox_M2J(index, *g);
                
                r = true;                
            } break;   

            case eoprot_tag_mc_motor_config_rotorencoder:
            {   // rotor encoder resolution
                int32_t* rotencres = reinterpret_cast<int32_t*>(ropdescriptor.rd->data);
                MController_config_motor_encoder(index, *rotencres);                
                r = true;                
            } break;   

            case eoprot_tag_mc_motor_config_pwmlimit:
            {
                eOmeas_pwm_t* pwmlimit = reinterpret_cast<eOmeas_pwm_t*>(ropdescriptor.rd->data);
                *pwmlimit = MController_config_motor_pwm_limit(index, *pwmlimit);
                
                r = true;                
            } break;   
 
            case eoprot_tag_mc_motor_config_temperaturelimit:
            {
                // shall we use it also for the amcbldc etc?
                eOmeas_temperature_t* temperatureLimit = reinterpret_cast<eOmeas_temperature_t*>(ropdescriptor.rd->data);
                MController_motor_config_max_temperature(index, temperatureLimit);
                
                r = true;                
            } break;   

            case eoprot_tag_mc_motor_config_pidcurrent:
            {
                eOmc_PID_t* pid = reinterpret_cast<eOmc_PID_t*>(ropdescriptor.rd->data);
                MController_motor_config_current_PID(index, pid);
                
                r = true;                
            } break;   
            
            case eoprot_tag_mc_motor_config_pidspeed:
            {
                eOmc_PID_t* pid = reinterpret_cast<eOmc_PID_t*>(ropdescriptor.rd->data);
                MController_motor_config_speed_PID(index, pid);
                
                r = true;                
            } break;  

            
            // the unmanaged tags
            case eoprot_tag_mc_motor_wholeitem: // = 0 and never used
                
            default:
            {
                // add a print that tells that a command is not managed
            } break;
        }
        
    }

    
//    eOmc_joint_config_t *cfg = (eOmc_joint_config_t*)rd->data;
//    eOprotIndex_t jxx = eoprot_ID2index(rd->id32);
////    eOmc_joint_status_t *jstatus = eo_entities_GetJointStatus(eo_entities_GetHandle(), jxx);
//    
//    // now we see if it is a mc4can or a 2foc or a mc4plus    
//    eOmotioncontroller_mode_t mcmode = s_motorcontrol_getmode();

//    if((eo_motcon_mode_foc == mcmode) || (eo_motcon_mode_advfoc == mcmode))
//    {
//        MController_config_joint(jxx, cfg);
//    }    
    
    // #warning TODO: add  embot::app::eth::theMCservice::getInstance().process(ropdescriptor.rd, ropdescriptor.nv);
    return true;
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


