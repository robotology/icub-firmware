
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_board_amcfoc_1cm7_mbd.h"
#include "embot_app_board_amcfoc_1cm7_theMBD.h"
#include "embot_app_application_theCANtracer.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

//#define TEST_theService

#if defined(TEST_theService)

//#define TEST_theService_basic
#define TEST_theService_theMC2agent


#if defined(TEST_theService_basic)
namespace test::basic 
{
    embot::app::msg::ADR adr2use {};
    
    void regularTX(std::vector<embot::prot::can::Frame> &output)
    {
        // print over debugger ... ?
        // add a canprint to output queue _tCOMMoutframes
        // send a ...
        
        static constexpr embot::core::Time period {5000*embot::core::time1millisec};    
        static embot::core::Time lastcall {0};    
        embot::core::Time now = embot::core::now();
        embot::core::Time delta = now - lastcall;    
        if(delta < period)
        {
            return;
        }    
        lastcall = now;
        
        // and now what i have to do
         
        embot::core::print("emitting a CAN PRINT w/ string = 5sEc");    
        embot::app::theCANtracer::getInstance().print("5sEc", output);         
    }
    
    void init(embot::app::msg::ADR adr)
    {
        adr2use = adr; 
    }

    void tick(const std::vector<embot::app::bldc::MSG> &input, std::vector<embot::app::bldc::MSG> &output)
    {
        std::vector<embot::prot::can::Frame> outfrs {};        
        // at first i print the messages
        
        if(false == input.empty())
        {
            embot::core::Time n {embot::core::now()};
            
            embot::core::print(embot::core::TimeFormatter(n).to_string() + ": received " +
            std::to_string(input.size()) + " ICC frames -> ");
            for(auto const &msg : input)
            {
                embot::core::print(msg.frame.to_string());
            }
            embot::core::print("sending back a CAN PRINT w/ string = acKed");
            

            // ok, i will send back one frame then

            embot::app::theCANtracer::getInstance().print("acKed", outfrs);       
           
        }
        
        // i regularse sends messages out
//        regularTX(outfrs);    

        
        //auto bus2use = input[0].location.getbus();
        auto bus2use = embot::app::msg::BUS::can2;
        embot::app::msg::Location loc {bus2use, adr2use};               
        for(const auto &f : outfrs)
        {
            embot::app::bldc::MSG m {loc, f};
            output.push_back(m);
        } 
                
    }
    
} // namespace test::basic 

#endif // #if defined(TEST_theService_basic)

#if defined(TEST_theService_theMC2agent)

#include "embot_app_bldc_theMC2agent.h"

namespace test::MC2agent {
    
    embot::app::msg::ADR adr2use {};
        
    embot::app::bldc::mbd::interface::ExtU_iterative_motion_controller_T input {};
    embot::app::bldc::mbd::interface::ExtY_iterative_motion_controller_T output {};
    embot::app::bldc::mbd::interface::Status_iterative_motion_controller_T status {};
    embot::app::bldc::mbd::interface::IO2 io2 {&input, &output, &status};
    
    std::vector<embot::prot::can::Frame> caninputframes {};
    std::vector<embot::prot::can::Frame> canoutputframes {};   

    auto bus2use = embot::app::msg::BUS::can2;        
        
    void init(embot::app::msg::ADR adr)
    {
        adr2use = adr; 
        std::array<uint8_t, embot::app::bldc::theMC2agent::numberofmotors> adrs {static_cast<uint8_t>(adr), static_cast<uint8_t>(adr+1)};
        embot::app::bldc::theMC2agent::getInstance().initialise({adrs, io2}); 
    }

    void tick(const std::vector<embot::app::bldc::MSG> &input, std::vector<embot::app::bldc::MSG> &output)
    {
        
        // sadly we have embot::app::bldc::MSG and the theMC2agent manages can frames, so we have to adapt them
        
        bool EXTFAULTisPRESSED {false};
        

        caninputframes.clear();
        canoutputframes.clear();
        
        for(const auto i : input)
        {
            caninputframes.push_back(i.frame);
        }
                
        
        embot::app::bldc::theMC2agent::getInstance().tick(caninputframes, {EXTFAULTisPRESSED}, canoutputframes);
        
        for(const auto o : canoutputframes)
        {
            embot::app::msg::Location l {bus2use, embot::prot::can::frame2sender(o)};
            embot::app::bldc::MSG msg {l, o};
            output.push_back(msg);
        }          
        
    }
       
    
} // namespace test::basic 

#endif // #if defined(TEST_theService_theMC2agent)

#endif // #if defined(TEST_theService)

namespace embot::app::board::amcfoc::cm7::mbd {
    
    
    void Startup(embot::app::msg::ADR adr)
    {
#if defined(TEST_theService_theMC2agent)
        
        //test::basic::init(adr);  
        test::MC2agent::init(adr);
        
#else
        embot::app::board::amcfoc::cm7::theMBD::getInstance().initialise({adr});          
#endif        

    }
    
    void OnTick(const std::vector<embot::app::bldc::MSG> &input, std::vector<embot::app::bldc::MSG> &output)
    {        
#if defined(TEST_theService_theMC2agent)
        
        //test::basic::tick(input, output);
        test::MC2agent::tick(input, output);
        
#else
        embot::app::board::amcfoc::cm7::theMBD::getInstance().tick(input, output);    
#endif   
                
    }
    
 
    

} // end of namespace



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


