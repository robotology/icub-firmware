
/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_MC_TRAJECTORY_H_
#define __EMBOT_APP_MC_TRAJECTORY_H_


#include "embot_core.h"



namespace embot::app::mc {
    
    class Trajectory
    {
    public:
        
        struct Point
        {
            float pos {0};
            float vel {0};
            float acc {0}; 
            
            constexpr Point() = default; 
            constexpr Point(float p, float v, float a) : pos(p), vel(v), acc(a) {}  
                
            void load(float p, float v, float a) { pos = p; vel = v; acc = a; }
            void clear() { load(0, 0, 0); }             
        };
        
        struct Setpoint
        {
            enum class Type { NONE, POS, VEL, POSraw, VELraw} ;
            
            Type type { Type::NONE };
            Point point {};
                        
            constexpr Setpoint() = default;
            constexpr Setpoint(Type t, float p, float v, float a) : type(t), point({p, v, a}) {}
            constexpr Setpoint(Type t, const Point &po) : type(t), point(po) {}
                
            void clear() { point.clear(); type = Type::NONE; }
                
            void loadPOS(float p, float withvel)    { type = Type::POS; point.load(p, withvel, 0); }
            void loadVEL(float v, float withacc)    { type = Type::VEL; point.load(0, v, withacc); }
            void loadPOSraw(float p)                { type = Type::POSraw; point.load(p, 0, 0); }
            void loadVELraw(float v)                { type = Type::VELraw; point.load(0, v, 0);  }
            bool isvalid() const                    { return Type::NONE != type; }
        };
        
        struct Config
        {
            float posmin {0};
            float posmax {0};
            float velmax {0};
            float accmax {0};         
            
            constexpr Config(float p0, float p1, float vm, float am) : posmin(p0), posmax(p1), velmax(vm), accmax(am) {}
            constexpr Config() = default;
            bool isvalid() const { return true; }
        };
        
                    
        Trajectory();
        ~Trajectory();
    
        bool config(const Config &config);        
        bool reset(); 

        bool set(const Setpoint &s);        
        
        bool tick();       
        bool get(Point &target) const;
        
        bool isdone();
        
        // if the former setpoint is position we stop with a position
        // else with zero velocity
        bool stop(const float &withvalue);
        
        bool stopvel();
        
        bool start2end(int32_t start, float end, float avgvel);
        
        // maybe some methods to retrieve the kind of setpoint we have

           
    private:        
        struct Impl;
        Impl *pImpl;  
    };  
   
    
} // namespace embot::app::mc {




#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

