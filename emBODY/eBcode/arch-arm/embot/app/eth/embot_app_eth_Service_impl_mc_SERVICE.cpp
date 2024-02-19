
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "embot_app_eth_Service_impl_mc_SERVICE.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theServiceMC.h"


#include "embot_app_eth_Service_impl.h"

#include "EOtheCANprotocol.h"
#include "EOtheCANmapping.h"


#include "embot_app_eth_theErrorManager.h"
#include "EoError.h"
#include "EOtheErrorManager.h"

#include "embot_os_theScheduler.h"

#include "EoProtocolMC.h"
#include "EOtheEntities.h"
#include "Controller.h"
#include "embot_app_eth_theEncoderReader.h"
#include "embot_app_eth_theICCmapping.h"
#include "embot_app_eth_theServices.h"



#if 0

marco.accame on 12 feb 2024:

in this implementation the mcAgent is used ONLY for ICC messages, so it gets the motorindex from theICCmapping. 
the messages from CAN are parsed with similar duplicated code and ask the motorindex to the CAN mapping via p_getentityindex()

to avoid code duplication in parsing messages we could use just this mcAgent and inside 
we could check the msg.source (that is a embot::app::msg::Location) and ask the relevant object: 
the ICC or the CAN mapping depending on the bus type 

#endif


namespace embot::app::eth::mc::messaging::receiver {
    
  
    bool getentityindex(const embot::app::msg::Location &source, uint8_t index)
    {
        bool ret {true};
        embot::app::msg::typeofBUS bustype = source.typeofbus();

        switch(bustype)
        {
            case embot::app::msg::typeofBUS::ICC:
            {
                index = embot::app::eth::theICCmapping::getInstance().toindex(source, {eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor});
                ret = (embot::app::eth::theICCmapping::invalidindex == index) ? false : true;           
            } break;

            case embot::app::msg::typeofBUS::CAN:
            {
                eObrd_canlocation_t loc = {0};
                loc.port = embot::core::tointegral(source.getbus()); // direct conversion to eOcanport_t is OK
                loc.addr = source.getadr();    
                loc.insideindex = eobrd_caninsideindex_first;                
                index = eo_canmap_GetEntityIndex(eo_canmap_GetHandle(), loc, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor);
                ret = (EOK_uint08dummy == index) ? false : true;
            } break;
            
            default:
            {
                ret = false;
            } break;
        }        
        
        return ret;        
    }
        
    class mcAgent : public Agent 
    {
    public:
        
        mcAgent() {}
        virtual ~mcAgent() {}
            
        bool get(const sigFOCstatus1 &msg) override 
        { 
            uint8_t motorindex {0}; 
            
            bool ok = getentityindex(msg.source, motorindex);
                        
            if(false == ok)
            {
                return false;
            }
            
            eOmc_motor_t *motor = reinterpret_cast<eOmc_motor_t*>(eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, motorindex));
            motor->status.basic.mot_current = msg.info.current;
            motor->status.basic.mot_velocity = msg.info.velocity;
            motor->status.basic.mot_position = msg.info.position;
            MController_update_motor_odometry_fbk_can(motorindex, (void*)msg.info.payload);      
            
            return ok; 
        } 
        
        bool get(const sigFOCstatus2 &msg) override 
        { 
            uint8_t motorindex {0}; 
            
            bool ok = getentityindex(msg.source, motorindex);
                        
            if(false == ok)
            {
                return false;
            }
            
            MController_update_motor_state_fbk(motorindex, (void*)msg.info.payload);

            return ok; 
        } 
        
        bool get(const sigPrint &msg) override 
        { 
            // the message may have history ...
            // we however just print the substring
            embot::os::Thread *thr {embot::os::theScheduler::getInstance().scheduled()};
            embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"ICC agent received CAN print:", thr}, {}, std::string(msg.info.substring));        
 
            //embot::core::print("received CAN print: " + std::string(msg.info.substring));
            return true; 
        }
    };

    mcAgent mca {};    

    Agent * agent()
    {
        return &mca;   
    }

} // namespace embot::app::eth::mc::messaging::receiver {


namespace embot::app::eth::service::impl::mc {    

    
    // struct SERVICE

        
    bool SERVICE::initted() const
    {
        return core.initted();
    }
    
    void SERVICE::initialise(size_t maxregs)
    {
        if(true == initted())
        {
            return;
        }
        
        // core
        //const size_t nboards {maxjomos};
        //const size_t nentities {maxjomos};
        //const size_t ntargets {maxjomos};
        core.initialise(maxregs);
        
        // mcagents
        mcagents.initialise(&core); 

        // controller
        constexpr size_t eo_motcon_standardENCOs {6}; // as in legacy code.
        MController_new(embot::app::eth::theServiceMC::maxJOMOs, eo_motcon_standardENCOs);
        
        // encoder reader
        embot::app::eth::theEncoderReader::getInstance().initialise(); 

        // load the the receiver agent for MC
        embot::app::eth::mc::messaging::receiver::load(embot::app::eth::mc::messaging::receiver::agent()); 
      
    }


    Service::Type SERVICE::type() const
    {
        return mcagents.type();
    }

    void SERVICE::clear()
    {
        core.clear();
        mcagents.clear();
    }

    bool SERVICE::active() const
    {
        return core.active();
    }  
    
    bool SERVICE::started() const
    {
        return core.started();
    }  
    
    embot::app::eth::Service::State SERVICE::state() const
    {
        return core.state();
    }
    
    void SERVICE::state(embot::app::eth::Service::State s)
    {
        core.state(s);
    }
    
    void SERVICE::synch(embot::app::eth::Service::State s)
    {
        state(s);
        synch(); 
    }
    
    void SERVICE::synch()
    {
        embot::app::eth::Service::State ss {core.state()};
        embot::app::eth::theServices::getInstance().synch(embot::app::eth::Service::Category::mc, ss); 
    }    
    
    size_t SERVICE::numberofjomos() const
    {
        return mcagents.numberofjomos();
    }
    
    eOmc_joint_t* SERVICE::joint(size_t n)
    {
        return mcagents.joint(n);
    }

    eOmc_motor_t* SERVICE::motor(size_t n)
    {
        return mcagents.motor(n);
    }
    
    bool SERVICE::load(embot::app::eth::Service *serv, const eOmn_serv_configuration_t *sc)
    {
        // it gets a sc, checks if it is supported, inits all the required values in here
        // if everything is OK it returns true, else false
        bool r {true};
        
        if(false == AGENTs::supported(sc))
        {            
            r = false;
            clear();
            return r;            
        }
        
        // ok, we can proceed: copy service configuration into internal memory 
        std::memmove(&core.serviceconfig, sc, sizeof(core.serviceconfig));
        
        // then load what we need
        r = mcagents.load(serv, &core.serviceconfig);
        
        if(false == r)
        {
            clear();
        }
        
        return r;        
    }


    bool SERVICE::verifyactivate(embot::app::eth::Service::OnEndOfOperation onend)
    {
        // it gets a sc, checks if it is supported, inits all the required values in here
        // if everything is OK it returns true, else false
        bool r {false};
        
        r = mcagents.verifyactivate(onend);

        if(false == r)
        {
            // also call failure of verify ... maybe
            clear();
        }
        
        return r;        
    }    
    
    bool SERVICE::deactivate()
    {
        bool r {true};
        
        mcagents.deactivate();

        clear();
        
        return r;          
    }   
 
    bool SERVICE::supported(embot::app::eth::Service::Type t)
    {
        return AGENTs::supported(t);
    }
    
    bool SERVICE::stop()
    {
        bool r {true};
        
        // i just idle the controller, not the encoder reader      
        MController_go_idle();
        
        r = mcagents.stop();
      
        return r;          
    }   
    
    bool SERVICE::forceidle()
    {
        bool r {true};
        r = mcagents.forceidle();
//        // so far we just ... deactivate
//        deactivate();
        return r;          
    }   
    
    bool SERVICE::start()
    {
        bool r {true};
        
        r = mcagents.start();

        // we just need to start a read of the encoder, nothing else 
        embot::app::eth::theEncoderReader::getInstance().StartReading();        
     
        return r;          
    }   

    bool SERVICE::tick()
    {
        bool r {true};

        // 1) get the encoders
        p_encodersAcquire();

        // 2) starts another reading
        embot::app::eth::theEncoderReader::getInstance().StartReading();
        
        // 3) perform motor control  
        MController_do();
        
        // 4) update status of motors and joints
        p_JOMOupdatestatus();    
        
        return r;          
    }  
    
    bool SERVICE::process(const Service::DescriptorCANframe &canframedescriptor)
    {
        bool r {false};

        embot::prot::can::Frame frame {canframedescriptor.frame->id, canframedescriptor.frame->size, canframedescriptor.frame->data};    
        embot::prot::can::Clas cls = embot::prot::can::frame2clas(frame);
        
        // we only accept commands of class periodicMotorControl   
        if(embot::prot::can::Clas::periodicMotorControl != cls)
        {
            return r;
        }
        
        embot::prot::can::motor::periodic::CMD cmd = static_cast<embot::prot::can::motor::periodic::CMD>(embot::prot::can::frame2cmd(frame));

            
        switch(cmd)
        {
            case embot::prot::can::motor::periodic::CMD::FOC:
            {
                eOprotIndex_t motorindex = p_getentityindex(eoprot_entity_mc_motor, canframedescriptor);
                eOmc_motor_t *motor = SERVICE::motor(motorindex);
                if(nullptr != motor)
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
                eOprotIndex_t jointindex = p_getentityindex(eoprot_entity_mc_joint, canframedescriptor);          
                eOmc_joint_t *joint = SERVICE::joint(jointindex);
                if(nullptr != joint)
                {    
                    r = true;
                    MController_update_motor_state_fbk(jointindex, canframedescriptor.frame->data); 
                }                
            } break;
            
            default:
            {
            } break;
        }        
        return r;          
    }  
    
    bool SERVICE::process(const Service::DescriptorFrame &framedescriptor)
    {
        bool r {false};
        
        // the Service::DescriptorFrame can be either CAN or ICC, so we must in here call the proper parser
        // luckily enough the embot::app::eth::mc::messaging::receiver::parse() is able to parse both 
        
        r = embot::app::eth::mc::messaging::receiver::parse(framedescriptor.bus, framedescriptor.frame);
        return r;          
    }
    
    

    bool SERVICE::process(const Service::DescriptorROP &ropdescriptor)
    {
        bool r {false};

        eOprotEntity_t entity = eoprot_ID2entity(ropdescriptor.rd->id32);
        eOprotIndex_t index = eoprot_ID2index(ropdescriptor.rd->id32);
        eOprotTag_t tag = eoprot_ID2tag(ropdescriptor.rd->id32);
        
        
//        std::string str = eoprot_TAG2string(eoprot_endpoint_motioncontrol, entity, tag);
//        embot::app::eth::theErrorManager::getInstance().trace(str, {"process()", embot::os::theScheduler::getInstance().scheduled()}); 
        
        
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
                // 1. status is signalled by the board to YRI and never asked
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
                    r = p_applysetpoint(index, setpoint);               
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
        
        return r;          
    }  
    

    bool SERVICE::p_encodersAcquire()
    { 
        bool res {true};
        
        size_t numofjomos = mcagents.numberofjomos();
        
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
                       
            if(true != p_encodersApplyValue(i, &encoder1valueinfo))
            {
                res = false;
            }
            
            if(true != p_encodersApplyValue(i, &encoder2valueinfo))
            {
                res = false;
            }
            
        } 
        
        embot::app::eth::theEncoderReader::getInstance().Diagnostics_Tick();
        
        // and finally start reading again
        embot::app::eth::theEncoderReader::getInstance().StartReading();
        
        return res;

    }

    bool SERVICE::p_encodersApplyValue(uint8_t index, embot::app::eth::encoder::v1::valueInfo *valueinfo)
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

    void SERVICE::p_JOMOupdatestatus()
    {
        const uint8_t transmit_decoupled_pwms = 0;
        
        size_t numofjomos = numberofjomos();
            

        for(uint8_t jId = 0; jId<numofjomos; jId++)
        {
            eOmc_joint_status_t *jstatus = &joint(jId)->status;

            MController_get_joint_state(jId, jstatus);            
            MController_get_pid_state(jId, &jstatus->core.ofpid, transmit_decoupled_pwms);
        }   
        
        for(uint8_t jId = 0; jId<numofjomos; jId++)
        {
            eOmc_motor_status_t *mstatus = &motor(jId)->status;
            MController_get_motor_state(jId, mstatus);
        }
    }
       

    bool SERVICE::p_applysetpoint(eOprotIndex_t jointindex, eOmc_setpoint_t *setpoint)
    {   
        if(jointindex >= numberofjomos())
        {
            return false;
        }
        
        eOmc_joint_t *joint = SERVICE::joint(jointindex);
        
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


    uint8_t SERVICE::p_getentityindex(eOprot_entity_t entity, const Service::DescriptorCANframe &canframedescriptor)
    {
        uint8_t ret {EOK_uint08dummy};
        
        eObrd_canlocation_t loc = {0};

        loc.port = canframedescriptor.port;
        loc.addr = EOCANPROT_FRAME_GET_SOURCE(canframedescriptor.frame);    
        loc.insideindex = eobrd_caninsideindex_first;
        
        ret = eo_canmap_GetEntityIndex(eo_canmap_GetHandle(), loc, eoprot_endpoint_motioncontrol, entity);
        
        return(ret);   
    }    


} // namespace embot::app::eth {  




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


