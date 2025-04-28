

/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_mc_Trajectory.h"


// --------------------------------------------------------------------------------------------------------------------
// - choice of implementation by assigning a macro
// --------------------------------------------------------------------------------------------------------------------

//#define useDUMMYimplementation
//#define useLEGACYimplementation
#define useMBDv01implementation

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#if defined(useDUMMYimplementation)
// nothing is needed
#elif defined(useLEGACYimplementation)
#include "Trajectory.h"
#include "Trajectory_hid.h"
using LegacyTrajectory = Trajectory; // the one in Trajectory_hid.h
#elif defined(useMBDv01implementation)

#include "Trajectory1.h"
using MBDv01Trajectory = namespace_s1::Trajectory;

#endif

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

// dummy implementation

#if defined(useDUMMYimplementation)
namespace trajectory::dummy
{
    
    struct Impl
    { 
        Impl() = default;     

        bool config(const  embot::app::mc::Trajectory::Config &c) { return false; }
        bool reset() { return false; }
        bool set(const  embot::app::mc::Trajectory::Setpoint &s) { return false; }
        bool tick() { return false; }
        bool get( embot::app::mc::Trajectory::Point &target) const { return false; }
        bool isdone() const { return false; }    
        bool stop(const float &v) { return false; } 
        bool stopvel() { return false; } 
        bool start2end(int32_t start, float end, float avgvel) { return false; }         
    };

}
#endif // #if defined(useDUMMYimplementation)


// legacy implementation

#if defined(useLEGACYimplementation)

namespace trajectory::legacy
{
    
    struct Impl
    { 
        LegacyTrajectory traj {};
        
        struct Status
        {
            uint32_t tbd {0};  
            embot::app::mc::Trajectory::Point target {};
            embot::app::mc::Trajectory::Setpoint setpoint {};       
            
            constexpr Status(uint32_t t) : tbd(t) {}
            constexpr Status() = default;
            bool isvalid() const { return true; }
        };
        
        Status _status {};
            
            
        embot::app::mc::Trajectory::Config _config {};    

        Impl() = default;     

        bool config(const  embot::app::mc::Trajectory::Config &c);
        bool reset();
        bool set(const  embot::app::mc::Trajectory::Setpoint &s);
        bool tick();
        bool get( embot::app::mc::Trajectory::Point &target) const;
        bool isdone();    
        bool stop(const float &v);
        bool stopvel();
        bool start2end(int32_t start, float end, float avgvel); 
                            
    };

}


bool trajectory::legacy::Impl::config(const embot::app::mc::Trajectory::Config &c)
{
    if(false == c.isvalid())
    {
        return false;
    }
    
    _config = c;
    
    Trajectory_config_limits(&traj, _config.posmin, _config.posmax, _config.velmax, _config.accmax);
    
    return true;        
}


bool trajectory::legacy::Impl::reset()
{
    bool r {true};
    
    Trajectory_init(&traj, 0, 0, 0);
    return r;
}


bool trajectory::legacy::Impl::set(const embot::app::mc::Trajectory::Setpoint &s)
{
    _status.setpoint = s;
    bool r {true};
    
    switch(_status.setpoint.type)
    {
        case embot::app::mc::Trajectory::Setpoint::Type::POS:
        {
            Trajectory_set_pos_end(&traj, s.point.pos, s.point.vel);
        } break;

        case embot::app::mc::Trajectory::Setpoint::Type::VEL:
        {
            Trajectory_set_vel_end(&traj, s.point.vel, s.point.acc);
        } break;   

        case embot::app::mc::Trajectory::Setpoint::Type::POSraw:
        {
            Trajectory_set_pos_raw(&traj, s.point.pos);
        } break; 

        case embot::app::mc::Trajectory::Setpoint::Type::VELraw:
        {
            Trajectory_set_vel_raw(&traj, s.point.vel);
        } break; 
        
        default:
        {
        } break;
    }
    
    
    return r;
}

bool trajectory::legacy::Impl::tick()
{
    bool r {true};
     
    float pos {};
    float vel {};
    float acc {};
    Trajectory_do_step(&traj, &pos, &vel, &acc);
    _status.target.pos = pos;
    _status.target.vel = vel;
    _status.target.acc = acc;
    return r;
}

bool trajectory::legacy::Impl::get(embot::app::mc::Trajectory::Point &target) const
{
    bool r {true};
    target = _status.target;
    return r;
}

bool trajectory::legacy::Impl::isdone()
{
    bool r {true};
    r = Trajectory_is_done(&traj);
    return r;
}

bool trajectory::legacy::Impl::stop(const float &v)
{
    bool r {true};
    Trajectory_stop(&traj, v);
    return r;    
}  


bool trajectory::legacy::Impl::stopvel()
{
    bool r {true};
    Trajectory_velocity_stop(&traj);
    return r;    
}  

bool trajectory::legacy::Impl::start2end(int32_t start, float end, float avgvel)
{
    bool r {true};
#if defined(WRIST_MK2)    
    Trajectory_start2end(&traj, end, avgvel);
#endif
    return r;    
} 

#endif // #if defined(useLEGACYimplementation)


// mbd first implementation

#if defined(useMBDv01implementation)

namespace trajectory::mbd01
{
    
    struct Impl
    { 
        MBDv01Trajectory traj {};
        
        struct Status
        {
            uint32_t tbd {0};  
            embot::app::mc::Trajectory::Point target {};
            embot::app::mc::Trajectory::Setpoint setpoint {};       
            
            constexpr Status(uint32_t t) : tbd(t) {}
            constexpr Status() = default;
            bool isvalid() const { return true; }
        };
        
        Status _status {};
            
            
        embot::app::mc::Trajectory::Config _config {};    

        Impl() = default;     

        bool config(const  embot::app::mc::Trajectory::Config &c);
        bool reset();
        bool set(const  embot::app::mc::Trajectory::Setpoint &s);
        bool tick();
        bool get( embot::app::mc::Trajectory::Point &target) const;
        bool isdone();    
        bool stop(const float &v);
        bool stopvel();
        bool start2end(int32_t start, float end, float avgvel);       
    };
    

bool trajectory::mbd01::Impl::config(const embot::app::mc::Trajectory::Config &c)
{
    if(false == c.isvalid())
    {
        return false;
    }
    
    _config = c;
    
    traj.Trajectory_config_limits(_config.posmin, _config.posmax, _config.velmax, _config.accmax);
    
    return true;        
}


bool trajectory::mbd01::Impl::reset()
{
    bool r {true};
    
    traj.Trajectory_init(0, 0, 0);
    return r;
}


bool trajectory::mbd01::Impl::set(const embot::app::mc::Trajectory::Setpoint &s)
{
    _status.setpoint = s;
    bool r {true};
    
    switch(_status.setpoint.type)
    {
        case embot::app::mc::Trajectory::Setpoint::Type::POS:
        {
            traj.Trajectory_set_pos_end(s.point.pos, s.point.vel);
        } break;

        case embot::app::mc::Trajectory::Setpoint::Type::VEL:
        {
            traj.Trajectory_set_vel_end(s.point.vel, s.point.acc);
        } break;   

        case embot::app::mc::Trajectory::Setpoint::Type::POSraw:
        {
            traj.Trajectory_set_pos_raw(s.point.pos);
        } break; 

        case embot::app::mc::Trajectory::Setpoint::Type::VELraw:
        {
        //    traj.Trajectory_set_vel_raw(s.point.vel);
        } break; 
        
        default:
        {
        } break;
    }
    
    
    return r;
}

bool trajectory::mbd01::Impl::tick()
{
    bool r {true};
     
    float pos {0};
    float vel {0};
    float acc {0};
    int32_t v {0};
    traj.Trajectory_do_step(&pos, &vel, &acc, &v);
    _status.target.pos = pos;
    _status.target.vel = vel;
    _status.target.acc = acc;
    return r;
}

bool trajectory::mbd01::Impl::get(embot::app::mc::Trajectory::Point &target) const
{
    bool r {true};
    target = _status.target;
    return r;
}

bool trajectory::mbd01::Impl::isdone()
{
    bool r {true};
    r = traj.Trajectory_is_done();
    return r;
}

bool trajectory::mbd01::Impl::stop(const float &v)
{
    bool r {true};
    traj.Trajectory_stop(v);
    return r;    
}  


bool trajectory::mbd01::Impl::stopvel()
{
    bool r {true};
    traj.Trajectory_velocity_stop();
    return r;    
}  

bool trajectory::mbd01::Impl::start2end(int32_t start, float end, float avgvel)
{
    bool r {true};
#if defined(WRIST_MK2)    
    Trajectory_start2end(&traj, end, avgvel);
#endif
    return r;    
}   

  
    

}
 #endif // if defined(useMBDv01implementation)

// --------------------------------------------------------------------------------------------------------------------
// - choice of implemention
// --------------------------------------------------------------------------------------------------------------------

#if defined(useDUMMYimplementation)
struct embot::app::mc::Trajectory::Impl : public trajectory::dummy::Impl { };
#elif defined(useLEGACYimplementation)
struct embot::app::mc::Trajectory::Impl : public trajectory::legacy::Impl { };
#elif defined(useMBDv01implementation)
struct embot::app::mc::Trajectory::Impl : public trajectory::mbd01::Impl { };
#endif


// --------------------------------------------------------------------------------------------------------------------
// - pimpl interface
// --------------------------------------------------------------------------------------------------------------------


embot::app::mc::Trajectory::Trajectory() : pImpl(new Impl) {}


embot::app::mc::Trajectory::~Trajectory()
{   
    delete pImpl;
}


bool embot::app::mc::Trajectory::config(const Config &c) 
{   
    return pImpl->config(c);
}


bool embot::app::mc::Trajectory::reset()
{
    return pImpl->reset();
}


bool embot::app::mc::Trajectory::set(const Setpoint &s)
{
    return pImpl->set(s);
}

bool embot::app::mc::Trajectory::tick()
{
    return pImpl->tick();
}

bool embot::app::mc::Trajectory::get(Point &target) const
{
    return pImpl->get(target);
}

bool embot::app::mc::Trajectory::isdone()
{
    return pImpl->isdone();
}

bool embot::app::mc::Trajectory::stop(const float &v)
{
    return pImpl->stop(v);
}

bool embot::app::mc::Trajectory::stopvel()
{
    return pImpl->stopvel();
}

bool embot::app::mc::Trajectory::start2end(int32_t start, float end, float avgvel)
{
    return pImpl->start2end(start, end, avgvel);
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

