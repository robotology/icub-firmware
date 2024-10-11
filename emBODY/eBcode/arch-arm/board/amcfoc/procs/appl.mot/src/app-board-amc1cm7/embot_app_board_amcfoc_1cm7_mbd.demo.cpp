
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_board_amcfoc_1cm7_mbd.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_motor_bldc.h"

#include "embot_core_binary.h"
#include "embot_core_binary.h"
#include "embot_prot_can_motor_polling.h"

//#define DONTUSE_theMBD

//#if defined(DONTUSE_theMBD)
//#include "embot_app_board_amcfoc_1cm7_theMBD.h"
//#endif

volatile int pwmperc = 10;


    int32_t CPHA1 = 0;

namespace embot::app::board::amcfoc::cm7::mbd {
    
   
    void bldc_sixsteps(embot::hw::MOTOR m, const embot::hw::motor::bldc::Currents * const currs, void *owner);
//    void sixsteps(embot::hw::MOTOR m, const embot::hw::motor::PhaseCurrents * const phasecurrents, void *owner);
//    
//    void motorphasecurrent(void *owner, embot::hw::MOTOR h, const embot::hw::motor::PhaseCurrents * const currents);
    

    uint8_t payload_motor_config[8] =
    {
        0x77,               // cmd
        0x02 | 0x20,        // flags
        0x10,               // enc res, le -> 16
        0x00,
        0xfb,               // enc off -> -5
        0xff,
        7,                  // poles
        1                   // tolerance
    };
    
    
    void Startup(embot::prot::can::Address adr)
    {
        volatile uint8_t vvv = embot::core::binary::word::memory2value<uint8_t>(&payload_motor_config[1]);
        
        volatile int16_t xxx = embot::core::binary::word::memory2value<int16_t>(&payload_motor_config[2]);
        
        embot::prot::can::Frame fr {0x003, 8, payload_motor_config};
        
        embot::prot::can::motor::polling::Message_SET_MOTOR_CONFIG m {};
        m.load(fr);
            
        volatile bool hh = m.info.config.check(embot::prot::can::motor::polling::MotorConfig::Flag::hasHALLsensor);
         hh = hh;  
        
        embot::hw::motor::bldc::init(embot::hw::MOTOR::one, {});
            

        embot::hw::motor::bldc::Config cfg {0, 7, 0, false, true, false};
        embot::hw::motor::bldc::configure(embot::hw::MOTOR::one, cfg);
        
//        embot::hw::motor::set1(embot::hw::MOTOR::one, sixsteps, nullptr);
        
        embot::hw::motor::bldc::set(embot::hw::MOTOR::one, {embot::hw::MOTOR::one, bldc_sixsteps, nullptr});
        
//        embot::hw::motor::enable(embot::hw::MOTOR::one, true);

    }
    
    void OnTick(const std::vector<embot::app::bldc::MSG> &input, std::vector<embot::app::bldc::MSG> &output)
    {
        static volatile uint32_t ii {0};
        size_t s = input.size();
        if(s > 0)
        {
            ii++;
            
            
        }
//#if !defined(TEST_embot_hw_icc)

//#else        
//        embot::app::board::amc2c::theMBD::getInstance().tick(input, output);      
//#endif                 
    }
    
    std::array<embot::hw::motor::bldc::Currents, embot::hw::motor::bldc::MAXnumber> myBLDCcurrents {};
        
    int32_t bldcCURR1 {0};
    int32_t bldcCURR2 {0};
    int32_t bldcCURR3 {0};
    
    void bldc_sixsteps(embot::hw::MOTOR m, const embot::hw::motor::bldc::Currents * const currs, void *owner)
    {
        static volatile uint32_t ciao {0};
        
//        static volatile embot::hw::MOTOR mm {embot::hw::MOTOR::one};
//        mm = m;
       
        myBLDCcurrents[embot::core::tointegral(m)] = *currs;
        
        
        if(embot::hw::MOTOR::one == m)
        {
            static uint64_t cnt {0};
            
            embot::hw::motor::bldc::HallStatus hallstatus = 0;
            hallstatus = embot::hw::motor::bldc::hall(embot::hw::MOTOR::one);
            
            bldcCURR1 = static_cast<int32_t>(myBLDCcurrents[embot::core::tointegral(m)].u * 1000.0);
            bldcCURR2 = static_cast<int32_t>(myBLDCcurrents[embot::core::tointegral(m)].v * 1000.0);
            bldcCURR3 = static_cast<int32_t>(myBLDCcurrents[embot::core::tointegral(m)].w * 1000.0);
            
            cnt++;
            if(cnt > 1000)
            {
                static uint32_t mm = 0;
                cnt = 0;

                embot::core::Time n = embot::core::now();
                embot::core::TimeFormatter tf{n};
                embot::core::print(std::to_string(mm) + ", " + tf.to_string() + ", " 
                                   + std::to_string(myBLDCcurrents[embot::core::tointegral(m)].u) + ", " 
                                   + std::to_string(myBLDCcurrents[embot::core::tointegral(m)].v) + ", " 
                                   + std::to_string(myBLDCcurrents[embot::core::tointegral(m)].w));
            }
            

            // now i revet to ABC to use mattia's table
           uint8_t hallstatusH1H2H3 = hallstatus;
            
//            hallstatusH1H2H3 = 0;
//            if(true == embot::core::binary::bit::check(hallstatusH3H2H1, 0))
//            {
//                embot::core::binary::bit::set(hallstatusH1H2H3, 2);
//            }
//            if(true == embot::core::binary::bit::check(hallstatusH3H2H1, 1))
//            {
//                embot::core::binary::bit::set(hallstatusH1H2H3, 1);
//            }   
//            if(true == embot::core::binary::bit::check(hallstatusH3H2H1, 2))
//            {
//                embot::core::binary::bit::set(hallstatusH1H2H3, 0);
//            }  
            
//            if(prev != hallstatusH1H2H3)
//            {
//                float aa = embot::hw::motor::angle(embot::hw::MOTOR::one);
//                embot::core::TimeFormatter tf(embot::core::now());
//                embot::core::print(tf.to_string() + " from to: " + std::to_string(prev) + " -> " + std::to_string(hallstatusH3H2H1) + " = " + std::to_string(aa));
//                prev = hallstatusH3H2H1;
//            }           

//            rotation(m, pwmperc);


//            six_step_control(m, hallstatus, pwmperc);    

//            testpwm(m);
        }        
        
    }
    
//    void rotation(embot::hw::MOTOR m, float pwm);
//    void six_step_control(embot::hw::MOTOR m, uint8_t maskh1h2h3, float pwm);
//    
//    void testpwm(embot::hw::MOTOR m)
//    {

//        embot::hw::motor::PWMperc pwm3 {25, 25, 25};
//        
////        embot::hw::motor::PWMperc pwm3 {0, 0, 0};
//        
////         embot::hw::motor::PWMperc pwm3 {75, 75, 75};
//        
//        embot::hw::motor::setPWM(m, pwm3);
//    }
//    
//    volatile uint64_t nnn[2] = {0, 0};
//    


//    
//    volatile uint8_t prev = 0;
//    
//    std::array<embot::hw::motor::PhaseCurrents, 2> phasecur {};
//        
//    embot::hw::motor::PhaseCurrents coo {};
//    
//    void sixsteps(embot::hw::MOTOR m, const embot::hw::motor::PhaseCurrents * const phasecurrents, void *owner)
//    {
//        static volatile uint32_t ciao {0};
//        
//        static volatile embot::hw::MOTOR mm {embot::hw::MOTOR::one};
//        mm = m;
//       
//        phasecur[embot::core::tointegral(m)] = *phasecurrents;
//        

//        
//        nnn[embot::core::tointegral(m)] ++;
//        ciao ++;
//        
//        
//        
//        if(embot::hw::MOTOR::one == m)
//        {
//            static uint64_t cnt {0};
//            
//            const embot::hw::motor::PhaseCurrents * const pc =  reinterpret_cast<const embot::hw::motor::PhaseCurrents * const>(phasecurrents);
//            coo = *pc;
//            uint8_t hallstatus = 0;
//            embot::hw::motor::gethallstatus(embot::hw::MOTOR::one, hallstatus);
//            
//            
//            CPHA1 = 1000 * coo.v1;
//            
//            cnt++;
//            if(cnt > 1000)
//            {
//                static uint32_t mm = 0;
//                cnt = 0;
//                mm++;
////                embot::core::print(std::to_string(mm) + ", " + std::to_string(coo.u) + ", " + std::to_string(coo.v) + ", " + std::to_string(coo.w));
//                embot::core::Time n = embot::core::now();
//                embot::core::TimeFormatter tf{n};
//                embot::core::print(std::to_string(mm) + ", " + tf.to_string() + ", " + std::to_string(phasecur[embot::core::tointegral(m)].v1) + ", " + std::to_string(phasecur[embot::core::tointegral(m)].v2) + ", " + std::to_string(phasecur[embot::core::tointegral(m)].v3));
//            }
//            

//            // now i revet to ABC to use mattia's table
//           uint8_t hallstatusH1H2H3 = hallstatus;
//            
////            hallstatusH1H2H3 = 0;
////            if(true == embot::core::binary::bit::check(hallstatusH3H2H1, 0))
////            {
////                embot::core::binary::bit::set(hallstatusH1H2H3, 2);
////            }
////            if(true == embot::core::binary::bit::check(hallstatusH3H2H1, 1))
////            {
////                embot::core::binary::bit::set(hallstatusH1H2H3, 1);
////            }   
////            if(true == embot::core::binary::bit::check(hallstatusH3H2H1, 2))
////            {
////                embot::core::binary::bit::set(hallstatusH1H2H3, 0);
////            }  
//            
////            if(prev != hallstatusH1H2H3)
////            {
////                float aa = embot::hw::motor::angle(embot::hw::MOTOR::one);
////                embot::core::TimeFormatter tf(embot::core::now());
////                embot::core::print(tf.to_string() + " from to: " + std::to_string(prev) + " -> " + std::to_string(hallstatusH3H2H1) + " = " + std::to_string(aa));
////                prev = hallstatusH3H2H1;
////            }           

////            rotation(m, pwmperc);


////            six_step_control(m, hallstatus, pwmperc);    

//            testpwm(m);
//        }
//    }
//    
 
float pp {0};
float ppdiv2 {0};

uint64_t cnt {0};

void six_step_control(embot::hw::MOTOR m, uint8_t maskh1h2h3, float pwm)
{
    #if 0
/*
H1 H2 H3 | mask
1  0  1  | 5
1  0  0  | 4
1  1  0  | 6
0  1  0  | 2
0  1  1  | 3
0  0  1  | 1
*/
    
//    if((cnt++ % 66666) != 0) 
//    {
//        return;
//    }
    
    
    
    
    if(maskh1h2h3 >= 7U || maskh1h2h3 == 0U)
        return;
    
    embot::hw::motor::PWMperc pwm3 {};
    
    float hiz {0};    
    if(pwm > 0)
    {
        hiz = pwm/2;
        switch(maskh1h2h3)
        {
            case 5: // Move 5 to first place because it looks nicer
                pwm3 = {pwm, 0, hiz};
//                embot::hw::motor::setPWM(m, pwm3);    // Set pwm to maintain quadrature between hall and reference
                break;
            case 4:
                pwm3 = {pwm, hiz, 0};
//                embot::hw::motor::setPWM(m, {pwm, hiz, 0});
                break;
            case 6:
                pwm3 = {hiz, pwm, 0};
//                embot::hw::motor::setPWM(m, {hiz, pwm, 0});
                break;
            case 2:
                pwm3 = {0, pwm, hiz};
//                embot::hw::motor::setPWM(m, {0, pwm, hiz});
                break;
            case 3:
                pwm3 = {0, hiz, pwm};
//                embot::hw::motor::setPWM(m, {0, hiz, pwm});
                break;
            case 1:
                pwm3 = {hiz, 0, pwm};
//                embot::hw::motor::setPWM(m, {hiz, 0, pwm});
                break;
            
            default:
                pwm3 = {0, 0, 0};
//                embot::hw::motor::setPWM(m, {0, 0, 0});
                break;
        }
    }
    else
    {
        pwm = -pwm;
        hiz = pwm/2;
        switch(maskh1h2h3)
        {
            case 5: 
                pwm3 = {hiz, pwm, 0};
//                embot::hw::motor::setPWM(m, {hiz, pwm, 0});    // Set pwm to maintain quadrature between hall and reference
                break;
            case 4:
                pwm3 = {0, pwm, hiz};
//                embot::hw::motor::setPWM(m, {0, pwm, hiz});
                break;
            case 6:
                pwm3 = {0, hiz, pwm};
//                embot::hw::motor::setPWM(m, {0, hiz, pwm});
                break;
            case 2:
                pwm3 = {hiz, 0, pwm};
//                embot::hw::motor::setPWM(m, {hiz, 0, pwm});
                break;
            case 3:
                pwm3 = {pwm, 0, hiz};
//                embot::hw::motor::setPWM(m, {pwm, 0, hiz});
                break;
            case 1:
                pwm3 = {pwm, hiz, 0};
//                embot::hw::motor::setPWM(m, {pwm, hiz, 0});
                break;
            
            default:
                pwm3 = {0, 0, 0};
//                embot::hw::motor::setPWM(m, {0, 0, 0});
                break;
        }
    }
    
    embot::hw::motor::setPWM(m, pwm3);
    
//    embot::hw::motor::setPWM(m, {25, 50, 75});
    
    pp = pwm;
    ppdiv2 = hiz;
    
    constexpr uint32_t div {66666 / 30};
    if((cnt++ % div) != 0) 
    {
        return;
    }

//    embot::core::print(tf.to_string() + ": pwm = (" + std::to_string(pwm3.a) + ", " + std::to_string(pwm3.b) + ", " + std::to_string(pwm3.c) + "); angle = " + std::to_string(aa));

//    float aa = embot::hw::motor::angle(embot::hw::MOTOR::one);
//    uint8_t hall {0};
//    embot::hw::motor::gethallstatus(embot::hw::MOTOR::one, hall);    
//    embot::core::TimeFormatter tf(embot::core::now());    
//    embot::core::print(tf.to_string() + ": hall angle = " + std::to_string(aa) + " deg (h3h2h1 = " + std::to_string(hall) + ")");

#endif
} 


void rotation(embot::hw::MOTOR m, float pwm)
{
#if 0    
/*
H1 H2 H3 | mask
1  0  1  | 5
1  0  0  | 4
1  1  0  | 6
0  1  0  | 2
0  1  1  | 3
0  0  1  | 1
*/
    
//    if((cnt++ % 22222) != 0) 
//    {
//        return;
//    }
    
    
    uint8_t maskh1h2h3 = 0;

    static uint8_t i {1};
    static constexpr uint8_t mapsector2hallh3h2h1[6] = 
    {
        0b00000100, // 000 deg is 4
        0b00000110, // 060 deg is 6
        0b00000010, // 120 deg is 2
        0b00000011, // 180 deg is 3
        0b00000001, // 240 deg is 1
        0b00000101  // 300 deg is 5       
    };
    
    static constexpr uint8_t mapsector2hallfussi[6] = 
    {
        0b00000100, // 000 deg is 4
        0b00000110, // 060 deg is 6
        0b00000010, // 120 deg is 2
        0b00000011, // 180 deg is 3
        0b00000001, // 240 deg is 1
        0b00000101  // 300 deg is 5       
    }; 

    maskh1h2h3 = mapsector2hallfussi[i];
    i++;
    i %= 6;
        
    if(maskh1h2h3 >= 7U || maskh1h2h3 == 0U)
        return;
    
    embot::hw::motor::PWMperc pwm3 {};
    
    float hiz {0};    
    if(pwm > 0)
    {
        hiz = pwm/2;
        switch(maskh1h2h3)
        {
            case 5: // Move 5 to first place because it looks nicer
                pwm3 = {pwm, 0, hiz};
//                embot::hw::motor::setPWM(m, pwm3);    // Set pwm to maintain quadrature between hall and reference
                break;
            case 4:
                pwm3 = {pwm, hiz, 0};
//                embot::hw::motor::setPWM(m, {pwm, hiz, 0});
                break;
            case 6:
                pwm3 = {hiz, pwm, 0};
//                embot::hw::motor::setPWM(m, {hiz, pwm, 0});
                break;
            case 2:
                pwm3 = {0, pwm, hiz};
//                embot::hw::motor::setPWM(m, {0, pwm, hiz});
                break;
            case 3:
                pwm3 = {0, hiz, pwm};
//                embot::hw::motor::setPWM(m, {0, hiz, pwm});
                break;
            case 1:
                pwm3 = {hiz, 0, pwm};
//                embot::hw::motor::setPWM(m, {hiz, 0, pwm});
                break;
            
            default:
                pwm3 = {0, 0, 0};
//                embot::hw::motor::setPWM(m, {0, 0, 0});
                break;
        }
    }
    else
    {
        pwm = -pwm;
        hiz = pwm/2;
        switch(maskh1h2h3)
        {
            case 5: 
                pwm3 = {hiz, pwm, 0};
//                embot::hw::motor::setPWM(m, {hiz, pwm, 0});    // Set pwm to maintain quadrature between hall and reference
                break;
            case 4:
                pwm3 = {0, pwm, hiz};
//                embot::hw::motor::setPWM(m, {0, pwm, hiz});
                break;
            case 6:
                pwm3 = {0, hiz, pwm};
//                embot::hw::motor::setPWM(m, {0, hiz, pwm});
                break;
            case 2:
                pwm3 = {hiz, 0, pwm};
//                embot::hw::motor::setPWM(m, {hiz, 0, pwm});
                break;
            case 3:
                pwm3 = {pwm, 0, hiz};
//                embot::hw::motor::setPWM(m, {pwm, 0, hiz});
                break;
            case 1:
                pwm3 = {pwm, hiz, 0};
//                embot::hw::motor::setPWM(m, {pwm, hiz, 0});
                break;
            
            default:
                pwm3 = {0, 0, 0};
//                embot::hw::motor::setPWM(m, {0, 0, 0});
                break;
        }
    }
    
    embot::hw::motor::setPWM(m, pwm3);
    
    pp = pwm;
    ppdiv2 = hiz;
     

//        float aa = embot::hw::motor::angle(embot::hw::MOTOR::one);
//        embot::core::TimeFormatter tf(embot::core::now());
//        embot::core::print(tf.to_string() + ": pwm = (" + std::to_string(pwm3.a) + ", " + std::to_string(pwm3.b) + ", " + std::to_string(pwm3.c) + "); angle = " + std::to_string(aa));
  
#endif  
}   

} // end of namespace



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


