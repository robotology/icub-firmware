
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef EMBOT_APP_PZDRIVER_H_
#define EMBOT_APP_PZDRIVER_H_

#include "embot_core.h"


namespace embot { namespace app {
    
    
    class PZlut
    {
    public:
        // interface
        virtual bool get(const uint32_t pos, uint32_t *arrayof4values) const = 0;
    
    public:
        virtual ~PZlut() {};         
    };    


    class PZdriver
    {
    public:
    
        using setpoint_t = uint32_t;
        
       
        struct Config
        {
            uint32_t tbd {0};
            PZlut *pzlut {nullptr};            
            constexpr Config() = default;
            constexpr Config(uint32_t t, PZlut *p) : tbd(t), pzlut(p) {}
        };
      
        PZdriver();
        ~PZdriver();
        
        bool init(const Config &config); 
        
        bool start();
        bool stop(); 

        bool set(const setpoint_t &value);       
        

    private:        
        struct Impl;
        Impl *pImpl;    
    };
    
    
    class myPZlut : public PZlut
    {
        public:
        static constexpr uint32_t numberofvalues {8};
        static constexpr uint32_t thevalues[numberofvalues] {1, 2, 3, 4, 5, 6, 7, 8};
        
        virtual bool get(const uint32_t pos, uint32_t *arrayof4values) const override
        {
            if(nullptr == arrayof4values)
            {
                return false;
            }
            
            uint32_t p = pos % numberofvalues;
            
            if(p < (numberofvalues - 4))
            {
                std::memmove(arrayof4values, &thevalues[p], 4*sizeof(uint32_t));
            }
            else
            {
                // whatever code to copy the tail of thevalues and then its head
                // TO BE TESTED...........
                uint32_t a = (numberofvalues-p);
                std::memmove(arrayof4values, &thevalues[p], a*sizeof(uint32_t));
                std::memmove(&arrayof4values[a], &thevalues[p+a], (4-a)*sizeof(uint32_t));               
            }
            
            return true;
        }
        
    };
    
    

 
}}

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


