
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_application_theFAPreader2.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_core.h"
#include "embot_core_binary.h"

#include <new>
#include "embot_os_Timer.h"
#include "embot_os_Action.h"
#include "embot_os_rtos.h"
#include "embot_hw.h"
#include "embot_prot_can.h"
#include "embot_prot_can_analog_periodic.h"

#include <cstdio>

#include "embot_app_theCANboardInfo.h"

#include "embot_app_application_theCANtracer.h"


#include "embot_core.h"

#include "embot_hw_sys.h"


#include "embot_app_theLEDmanager.h"

#include "embot_os_theScheduler.h"

#include "embot_app_application_theCANtracer.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"

#if defined(EMBOT_ENABLE_hw_tlv493d_emulatedMODE)
    #warning EMBOT_ENABLE_hw_tlv493d_emulatedMODE is defined, the theFAPreader2 will use fake tlv readings
#endif


// the sensors are tlv only
#include "embot_hw_tlv493d.h"

#define EMBOT_POSREADER2_compensatereadings

//#define CONTINUOUS_ACQUISITION

namespace embot { namespace app { namespace application {
    
//    volatile uint64_t st{0};
//    volatile uint64_t du{0};
    
    struct SensorHelper
    {
        static embot::prot::can::analog::posLABEL getPOSlabel(const theFAPreader2::Sensor &snsr)
        {
            return snsr.params.label;     
        }

        static std::string to_string(const theFAPreader2::Sensor &snsr)
        {
            if(embot::hw::ANY::none == snsr.id)
            {
                return "ANY::none";
            }
            
            if(theFAPreader2::sensorType::tlv == snsr.type)
            {
                return embot::hw::tlv493d::to_string(static_cast<embot::hw::TLV493D>(snsr.id));
            }
            
            return "UNKNOWN";
        } 


        static embot::hw::LED to_led(const theFAPreader2::Sensor &snsr)
        {
    //        static constexpr std::array<embot::hw::LED, numberoftlv> tlvleds 
    //        { 
    //            embot::hw::LED::five,   
    //            embot::hw::LED::one,     
    //            embot::hw::LED::two,    
    //            embot::hw::LED::three   
    //        };
    //        
    //        if(embot::hw::ANY::none == snsr.id)
    //        {
    //            return embot::hw::LED::none;
    //        }
    //        
    //        if(sensorType::tlv == snsr.type)
    //        {
    //            uint8_t pos = embot::core::tointegral(snsr.id);
    //            return (pos < tlvleds.size()) ? tlvleds[pos] : embot::hw::LED::none;
    //        }       
                
            return embot::hw::LED::none;
        }     
                 
        static bool init(const theFAPreader2::Sensor &snsr)
        { 
            bool ret {false};
            
            if(embot::hw::ANY::none != snsr.id)
            {
                if(theFAPreader2::sensorType::tlv == snsr.type) 
                {                
                    if(embot::hw::resOK == embot::hw::tlv493d::init(static_cast<embot::hw::TLV493D>(snsr.id), {embot::hw::tlv493d::Config::startupMODE::dontresetCHIP}))
                    {
                        ret = true;
                    } 
                    embot::hw::sys::delay(50*embot::core::time1millisec); // why???????
                }
            }

            return ret;
        }
          

        static void acquisition(const theFAPreader2::Sensor &snsr, const embot::core::Callback &cbk)
        {        
            if(theFAPreader2::sensorType::tlv == snsr.type)
            {
//                st = embot::core::now();
                embot::hw::tlv493d::acquisition(static_cast<embot::hw::TLV493D>(snsr.id), cbk);
//                du = embot::core::now() - st;
//                du = du;
            }
        }
        
        static embot::hw::result_t read(const theFAPreader2::Sensor &snsr, theFAPreader2::Position &pos)
        {
            embot::hw::result_t r = embot::hw::resNOK;
            
            if(theFAPreader2::sensorType::tlv == snsr.type)
            {
                r = embot::hw::tlv493d::read(static_cast<embot::hw::TLV493D>(snsr.id), pos);
            }    
                   
            if(embot::hw::resOK != r)
            {
                pos = theFAPreader2::valueOfPositionACQUISITIONnotvalid;
            } 

            return r;        
        }        
        
    };

     
    
}}}
    
struct embot::app::application::theFAPreader2::Impl
{   

    void print(const std::string &str)
    {
 //       embot::core::print(str);
    }    

    Config config {};   
    //std::vector<Sensor *> activeSensors {}; 
    std::vector<uint8_t> activeSensorsIndex {};             
    embot::os::EventMask globaleventmask {0};           
    bool ticking {false};
   
    // if the the sensor inside config.sensors[x] can be initted during initialise() at startup of the board then we set sensors_mask_of_connected
    // so, to check if a sensor in pos x is usable we use embot::core::binary::bit::check(pImpl->sensors_mask_of_connected, x);
    // or also we see config.sensors[x].connected is true.
    uint8_t sensors_mask_of_connected {0};
    
    // inside sensors_mask_of_active the bit in position x tells that config.sensors[x] is surely connected and that must be acquired and tranmsitted
    std::uint8_t sensors_mask_of_active {0};
        
    embot::os::Timer *timerAcquisition {nullptr};
    embot::os::Action actionAcquisition {};        
    embot::core::relTime periodAcquisition {50*embot::core::time1millisec};
    embot::os::Timer *timerTOUT {nullptr};
    embot::core::relTime maxTOUT {5*embot::core::time1millisec};
    
    embot::os::Timer *timerTX {nullptr};
    embot::os::Action actionTX {};
    embot::os::Action actionTOUT {};        
        
    embot::core::relTime periodTX {50*embot::core::time1millisec};
    
    static constexpr Position valueOfPositionCHIPnotinitted = 2000*100;         // which will results in 2000 degrees or 20000 decidegrees
    
    embot::os::rtos::mutex_t *_mtxOf_positions {nullptr};
    
        
    struct threadsafePositions
    {
        embot::os::rtos::mutex_t *_mtx {nullptr};
        std::array<Position, numberofpositions> _positions {0};
        threadsafePositions() : _mtx(embot::os::rtos::mutex_new()) {}
        bool set(Position p, uint8_t n, embot::core::relTime tout = embot::core::reltimeWaitForever)
        {
            if(n >= numberofpositions)
            {
                return false;
            }
            
            if(true == embot::os::rtos::mutex_take(_mtx, tout))
            {
                _positions[n] = p;
                embot::os::rtos::mutex_release(_mtx);
                return true;
            }
            
            return false;            
        }
        
        Position get(uint8_t n, embot::core::relTime tout = embot::core::reltimeWaitForever)
        {
            Position p {0};
            if(n >= numberofpositions)
            {
                return p;
            }
            
            if(true == embot::os::rtos::mutex_take(_mtx, tout))
            {
                p = _positions[n];
                embot::os::rtos::mutex_release(_mtx);
                return p;
            }            
            return p;            
        }
        
        bool get(std::array<Position, numberofpositions> &pp, embot::core::relTime tout = embot::core::reltimeWaitForever)
        {
            if(true == embot::os::rtos::mutex_take(_mtx, tout))
            {
                pp = _positions;
                embot::os::rtos::mutex_release(_mtx);
                return true;
            }            
            return false;            
        }
        
    };
    
    threadsafePositions *tspositions {nullptr};
    
    
    void updateActiveSensorsIndex()
    {
        activeSensorsIndex.clear();
        for(uint8_t s=0; s<numberofpositions; s++)
        {
            if(true == embot::core::binary::bit::check(sensors_mask_of_active, s))
            {
                activeSensorsIndex.push_back(s);
            }
        }
    }
    
    bool isvalidposition(const Position &po) const
    {
        if((po == valueOfPositionCHIPnotinitted) || (po == valueOfPositionACQUISITIONnotvalid))
        {
            return false;
        }
        return true;        
    }
    
   
    Impl() 
    {   
        ticking = false;  
        sensors_mask_of_connected = 0;
        sensors_mask_of_active = 0;
        periodTX = 50*embot::core::time1millisec;
        
        timerAcquisition = new embot::os::Timer;  
        timerTX = new embot::os::Timer; 
        timerTOUT = new embot::os::Timer;         
                
        
        tspositions = new threadsafePositions;
        
        activeSensorsIndex.reserve(numberofpositions);
    }
   
    bool start();
    bool stop();    
    bool tick(std::vector<embot::prot::can::Frame> &replies);
    bool isvalid(embot::os::EventMask evtmask) const;
    bool process(embot::os::EventMask evtmask, std::vector<embot::prot::can::Frame> &outframes);
    bool get(std::vector<embot::prot::can::Frame> &frames2transmit);

    
    void acquisitionchain_start(bool yes)
    {
        if(true == yes)
        {
            embot::os::Timer::Config cfgac(periodAcquisition, actionAcquisition, embot::os::Timer::Mode::forever);
            timerAcquisition->start(cfgac); 
        }
        else
        {
            timerAcquisition->stop();
        }
    }        
        
    static void alertdataisready00(void *p)
    {
        embot::app::application::theFAPreader2::Impl *mypImpl = reinterpret_cast<embot::app::application::theFAPreader2::Impl*>(p);
        
        if(true == mypImpl->ticking)
        {
            mypImpl->config.reader->setEvent(mypImpl->config.sensors[0].dataready);
        }   
    }
    
    
    static void alertdataisready01(void *p)
    {
        embot::app::application::theFAPreader2::Impl *mypImpl = reinterpret_cast<embot::app::application::theFAPreader2::Impl*>(p);
        
        if(true == mypImpl->ticking)
        {
            mypImpl->config.reader->setEvent(mypImpl->config.sensors[1].dataready);
        }        
    }
        
    static constexpr std::array<embot::core::fpCaller, numberofpositions> alertdataisready = { alertdataisready00, alertdataisready01, nullptr, nullptr };
          
    struct cbkData
    {
        cbkData() = default;
        cbkData(embot::app::application::theFAPreader2::Impl *p, uint8_t _n) : pimpl(p), n(_n) {}
        embot::app::application::theFAPreader2::Impl *pimpl {nullptr};
        uint8_t n {0};
    };
    
    std::array<cbkData, numberofpositions> cbkdataarray = { };
    
    static void acquisition_parallel_alert_dataready(void *p)
    {
        cbkData *cbkdata = reinterpret_cast<embot::app::application::theFAPreader2::Impl::cbkData*>(p);
        cbkdata->pimpl->config.reader->setEvent(cbkdata->pimpl->config.sensors[cbkdata->n].dataready);
    }    


    static void acquisition_daisychain_alert_dataready(void *p)
    {
        embot::app::application::theFAPreader2::Impl *pimpl = reinterpret_cast<embot::app::application::theFAPreader2::Impl*>(p);
        pimpl->config.reader->setEvent(pimpl->config.sensors[pimpl->activeSensorsIndex[pimpl->daisychain_counter]].dataready);
    }
    
    
    void timerTOUT_start()
    {
        if(0 != maxTOUT)
        {
            embot::os::Timer::Config cfgtmt(maxTOUT, actionTOUT, embot::os::Timer::Mode::oneshot);
            timerTOUT->start(cfgtmt);
        }
    }
    
    void timerTOUT_stop()
    {
        if(0 != maxTOUT)
        {
            timerTOUT->stop();
        }
    }
    
    // daisychain
        
    volatile uint8_t daisychain_counter {0};
  
    void acquisition_daisychain_trigger()
    {        
        if(daisychain_counter < activeSensorsIndex.size())
        {
//            print("acquisition_daisychain_trigger() @ " + embot::core::TimeFormatter(embot::core::now()).to_string());
            // send event to thread for the current daisychained
            uint8_t n = activeSensorsIndex[daisychain_counter];
            config.reader->setEvent(config.sensors[n].askdata);           
        }        
    }
     
    
    void acquisition_daisychain_start()
    {
        print("acquisition_daisychain_start() @ " + embot::core::TimeFormatter(embot::core::now()).to_string());
                
        daisychain_counter = 0;
        acquisition_daisychain_trigger();
    }
            
    void acquisition_daisychain_on_askdata(uint8_t n)
    {
        // print("acquisition_daisychain_on_askdata(" + std::to_string(n) + ") @ " + embot::core::TimeFormatter(embot::core::now()).to_string());
        // config a callback which sends the dataready event
        embot::core::Callback cbk {acquisition_daisychain_alert_dataready, this};
        // start acquisition
        SensorHelper::acquisition(config.sensors[n], cbk);
        
        timerTOUT_start();
    }     

    void acquisition_daisychain_on_dataready(uint8_t n)
    {

        timerTOUT_stop();    
        
//           print("acquisition_daisychain_on_dataready(" + std::to_string(n) + ") @ " + embot::core::TimeFormatter(embot::core::now()).to_string());            
        // retrieve data from sensor n
        Position pos {0};    
        SensorHelper::read(config.sensors[n], pos);
                  
        print(SensorHelper::to_string(config.sensors[n]) + " (n=" +  std::to_string(n) + ") is " + std::to_string(pos) + " centiDEG @ " + embot::core::TimeFormatter(embot::core::now()).to_string()); 

        tspositions->set(pos, n);
        
        // increment
        daisychain_counter++;
        
        // if not last, trigger a start()
        if(daisychain_counter < activeSensorsIndex.size())
        {
            acquisition_daisychain_trigger();
        }
        else
        {
            daisychain_counter = 0;
        }
    } 

    void acquisition_daisychain_noreply()
    {
        uint8_t n = activeSensorsIndex[daisychain_counter];
        print("acquisition_daisychain_noreply() of " + SensorHelper::to_string(config.sensors[n]) + ", index = " + std::to_string(daisychain_counter) + ", @ " + embot::core::TimeFormatter(embot::core::now()).to_string());

        timerTOUT_stop();
        
        // then... what to do? 
        // - i could mark a failure mask
        // - i could get the mask at the end of acquisition chain and fill a can print frame to transmit 
        // for now... i just trigger the next one as if i had received a acquisition_daisychain_on_dataready()
        
        // increment
        daisychain_counter++;
        
        // if not last, trigger a start()
        if(daisychain_counter < activeSensorsIndex.size())
        {
            acquisition_daisychain_trigger();
        }
        else
        {
            daisychain_counter = 0;
        }
    }    

    // parallel
    
    uint32_t acquisition_fullyparallel_maskofrequests = 0;
    uint32_t acquisition_fullyparallel_maskofreplies = 0;
    void acquisition_fullyparallel_start()
    {
        std::string str {};        
        acquisition_fullyparallel_maskofrequests = acquisition_fullyparallel_maskofreplies = 0;
        embot::os::EventMask eventmask = 0;
        for(uint8_t i=0; i<activeSensorsIndex.size(); i++)
        {
            uint8_t n = activeSensorsIndex[i];
            eventmask |= config.sensors[n].askdata;  
            embot::core::binary::bit::set(acquisition_fullyparallel_maskofrequests, n);           
            str += SensorHelper::to_string(config.sensors[n]);
            str += " ";
        }
        config.reader->setEvent(eventmask); 
        
        print("acquisition_fullyparallel_start() for " + str + "@ " + embot::core::TimeFormatter(embot::core::now()).to_string());
               
        timerTOUT_start();
    }
    
   
    void acquisition_fullyparallel_on_askdata(uint8_t n)
    {
        print("acquisition_fp_on_askdata(" + std::to_string(n) + ") for " + SensorHelper::to_string(config.sensors[n]) + " @ " + embot::core::TimeFormatter(embot::core::now()).to_string());
        // config a callback which sends the dataready event
        cbkdataarray[n] = {this, n};
        embot::core::Callback cbk {acquisition_parallel_alert_dataready, &cbkdataarray[n]};
        // start acquisition
        SensorHelper::acquisition(config.sensors[n], cbk);
    }     

    void acquisition_fullyparallel_on_dataready(uint8_t n)
    {
//        print("acquisition_fp_on_dataready(" + std::to_string(n) + ") @ " + embot::core::TimeFormatter(embot::core::now()).to_string());
        // retrieve data from sensor n
        Position pos {0};
        SensorHelper::read(config.sensors[n], pos);
             
        print(SensorHelper::to_string(config.sensors[n]) + " (n=" +  std::to_string(n) + ") is " + std::to_string(pos) + " centiDEG @ " + embot::core::TimeFormatter(embot::core::now()).to_string()); 
  
        tspositions->set(pos, n);
        
        embot::core::binary::bit::set(acquisition_fullyparallel_maskofreplies, n);
        
        if(acquisition_fullyparallel_maskofreplies == acquisition_fullyparallel_maskofrequests)
        {
            // ok, we have finished. i stop the timer
            timerTOUT_stop();
        }
    } 
    
    void acquisition_fullyparallel_noreply()
    {
        uint32_t maskOFmissing = acquisition_fullyparallel_maskofrequests ^ acquisition_fullyparallel_maskofreplies;
        std::string str {};
        for(uint8_t i=0; i<activeSensorsIndex.size(); i++)
        {
            uint8_t n = activeSensorsIndex[i];
            if(embot::core::binary::bit::check(maskOFmissing, n))
            {
                str += SensorHelper::to_string(config.sensors[n]);
                str += " ";
            }            
        }
        
        print("acquisition_fullyparallel_noreply() of " + str + " @ " + embot::core::TimeFormatter(embot::core::now()).to_string());
       
        timerTOUT_stop();
        
        // then... what to do? 
        // - i could use the differences between acquisition_fullyparallel_maskofreplies and acquisition_fullyparallel_maskofrequests
        // - i could fill a can print frame to transmit 
        // for now... i just stop the timer
        
    }      
   
    bool acquisition_get(std::vector<embot::prot::can::Frame> &replies);    
                      
};


// this starts the tx actually
bool embot::app::application::theFAPreader2::Impl::start()
{ 
//#if !defined(CONTINUOUS_ACQUISITION)
    acquisitionchain_start(true);
//#endif  

//#if defined(CONTINUOUS_ACQUISITION)
//    #warning TODO: verify what CONTINUOUS_ACQUISITION does ......
//#endif    
    
    embot::os::Timer::Config cfgtx(periodTX, actionTX, embot::os::Timer::Mode::forever);
    timerTX->start(cfgtx);
    ticking = true;    
    return true;            
}

// this stops the tx
bool embot::app::application::theFAPreader2::Impl::stop()
{
//#if !defined(CONTINUOUS_ACQUISITION)
    acquisitionchain_start(false);
//#endif   
    
    timerTX->stop();
    ticking = false;    
    return true;
}

// this retrieves the 
bool embot::app::application::theFAPreader2::Impl::get(std::vector<embot::prot::can::Frame> &frames2transmit)
{
    // that is called by the transmitter thread only ... i may check vs thread correctness but i dont so that i am quicker

//    if(config.transmitter != embot::os::theScheduler::getInstance().scheduled())
//    {
//        return false;
//    }
    
    acquisition_get(frames2transmit);    
    return true;
}

bool embot::app::application::theFAPreader2::Impl::isvalid(embot::os::EventMask evtmask) const
{ 
    return (0 == (evtmask & globaleventmask)) ? false : true;    
}

bool embot::app::application::theFAPreader2::Impl::process(embot::os::EventMask evtmask, std::vector<embot::prot::can::Frame> &outframes)
{  
    if(false == isvalid(evtmask))
    {
        return false;
    }
    
    // start acquisition chain
    if(true == embot::core::binary::mask::check(evtmask, config.events.acquire))
    {
        switch(config.acquisitionmode)
        {
            case AcquisitionMode::daisychain:
            {
                acquisition_daisychain_start();
            } break;
            
            case AcquisitionMode::fullyparallel:
            {
                acquisition_fullyparallel_start();
            } break;       
                        
            default:
            {
            } break;
        }       
    }


    // process a timeout in acquisition chain
    if(true == embot::core::binary::mask::check(evtmask, config.events.noreply))
    {
        switch(config.acquisitionmode)
        {
            case AcquisitionMode::daisychain:
            {
                acquisition_daisychain_noreply();
            } break;
            
            case AcquisitionMode::fullyparallel:
            {
                acquisition_fullyparallel_noreply();
            } break;         
                        
            default:
            {
            } break;
        }       
    }
    
    // check the events related to the sensors 
    for(uint8_t i=0; i<activeSensorsIndex.size(); i++)
    {
        // the events for asking to the sensor 
        uint8_t n = activeSensorsIndex[i];
        
        if(true == embot::core::binary::mask::check(evtmask, config.sensors[n].askdata)) 
        {
            switch(config.acquisitionmode)
            {
                case AcquisitionMode::daisychain:
                {
                    acquisition_daisychain_on_askdata(n);
                } break;
                
                case AcquisitionMode::fullyparallel:
                {
                   acquisition_fullyparallel_on_askdata(n);
                } break;
                                            
                default:
                {
                } break;
            }       
        }
        
        // the events for retrieving the data
        if(true == embot::core::binary::mask::check(evtmask, config.sensors[n].dataready)) 
        {
            switch(config.acquisitionmode)
            {
                case AcquisitionMode::daisychain:
                {
                    acquisition_daisychain_on_dataready(n);
                } break;
                
                case AcquisitionMode::fullyparallel:
                {
                   acquisition_fullyparallel_on_dataready(n);
                } break;
                
                default:
                {
                } break;
            } 
        }
    }
    
    
    if(true == embot::core::binary::mask::check(evtmask, config.events.transmit))
    {
        get(outframes);
    }

    return true;    
}


bool embot::app::application::theFAPreader2::Impl::acquisition_get(std::vector<embot::prot::can::Frame> &replies)
{   
            
    std::array<Position, numberofpositions> positions {0};
    tspositions->get(positions);
        
    // we are ready to transmit    
    embot::prot::can::Frame frame;     

    std::string str;    
    
    for(uint8_t i=0; i<activeSensorsIndex.size(); i++)
    {
        embot::prot::can::analog::periodic::Message_POS msg {};
        embot::prot::can::analog::periodic::Message_POS::Info info {};  
        info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress(); 
        
        uint8_t n = activeSensorsIndex[i];
        Position rawvalue = positions[n];
            
        // we gets some other properties
        // sensorType st = config.sensors[n].type;           
#if defined(EMBOT_POSREADER2_compensatereadings)
        sensorParams sp = config.sensors[n].params; 
#endif            
        constexpr bool isrotational {true};
                           
        
        // now we transform the values        
        if(true == isrotational)
        {
            embot::prot::can::analog::deciDeg v = valueOfPositionACQUISITIONnotvalid / 10;
            
            if(valueOfPositionACQUISITIONnotvalid != rawvalue)
            {
#if defined(EMBOT_POSREADER2_compensatereadings)        
                // ok, compensate to embot::prot::can::analog::posTYPE::angleDeciDeg
                // for now i transform everything in degrees. yes, i know i lose resolution
                embot::prot::can::analog::deciDeg decideg_not_compensated = rawvalue/10;                
                v = sp.calib.transform(decideg_not_compensated);
#else                
                v = rawvalue/10; 
#endif                
            }
                   

            // now we transmit v as a deciDeg. one can frame at a time
            std::array<embot::prot::can::analog::deciDeg, 3> values = { v, 0, 0};

            str += SensorHelper::to_string(config.sensors[n]);
            str += " = ";
            str += std::to_string(v/10);
            str += " DEG ";
            
            info.loadDeciDeg(SensorHelper::getPOSlabel(config.sensors[n]), 1, values);
               
            msg.load(info);
            msg.get(frame);
            replies.push_back(frame); 
        
            // finally, in here we manage a wrong measure. in addition to led off we also send a canprint message
//            embot::hw::LED led = sensor_to_led(config.sensors[n]);
            if(valueOfPositionACQUISITIONnotvalid == rawvalue)
            {
//                embot::hw::led::off(led);
                embot::app::theCANtracer &tr = embot::app::theCANtracer::getInstance();
                tr.print("FAP" + std::to_string(n) + "err", replies);
            }
            else
            {
//                embot::hw::led::on(led);
            }            
        }
     
    }
    
    print("theFAPreader2 transmits: " + str + embot::core::TimeFormatter(embot::core::now()).to_string());
             
    return true;           
}




// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------



embot::app::application::theFAPreader2& embot::app::application::theFAPreader2::getInstance()
{
    static theFAPreader2* p = new theFAPreader2();
    return *p;
}

embot::app::application::theFAPreader2::theFAPreader2()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::application::theFAPreader2::~theFAPreader2() { }


         
bool embot::app::application::theFAPreader2::initialise(const Config &config)
{

    pImpl->config = config;
    
    // this event triggers a periodic reading. 
    pImpl->actionAcquisition.load(embot::os::EventToThread(pImpl->config.events.acquire, pImpl->config.reader));
    pImpl->periodAcquisition = std::max(pImpl->config.acquisitionperiod, static_cast<embot::core::relTime>(10*embot::core::time1millisec));
    
    // this event signals a no reply in data reading
    pImpl->actionTOUT.load(embot::os::EventToThread(pImpl->config.events.noreply, pImpl->config.reader));
    
    // this event triggers a periodic transmission with period configured by can message
    pImpl->actionTX.load(embot::os::EventToThread(pImpl->config.events.transmit, pImpl->config.transmitter));
     
    pImpl->tspositions->set(pImpl->valueOfPositionCHIPnotinitted, 0);
    pImpl->tspositions->set(pImpl->valueOfPositionCHIPnotinitted, 1);
    
    
    std::string str {};
    pImpl->globaleventmask = pImpl->config.events.acquire | pImpl->config.events.noreply | pImpl->config.events.transmit;
    pImpl->maxTOUT = pImpl->config.acquisitiontimeout;
        
    pImpl->activeSensorsIndex.clear();
        
    pImpl->sensors_mask_of_connected = 0;
        
    for(uint8_t s=0; s<numberofpositions; s++)
    {
        pImpl->config.sensors[s].connected = false;
        
        if(embot::hw::ANY::none != pImpl->config.sensors[s].id)
        {  
            bool ok = SensorHelper::init(config.sensors[s]);
            
            if(ok)
            {
                pImpl->config.sensors[s].connected = true;
                embot::core::binary::bit::set(pImpl->sensors_mask_of_connected, s);

                if((pImpl->config.sensors[s].timeout != 0) && (pImpl->config.sensors[s].noreply != 0))
                {
                    pImpl->maxTOUT = std::max(pImpl->maxTOUT, pImpl->config.sensors[s].timeout);
                }

                pImpl->globaleventmask |= (pImpl->config.sensors[s].askdata | pImpl->config.sensors[s].dataready | pImpl->config.sensors[s].noreply);   
         
                str += SensorHelper::to_string(config.sensors[s]);
                str += " ";                

//                embot::hw::LED l = pImpl->sensor_to_led(config.sensors[n]);
//                embot::hw::led::on(l);
            }
            else
            {
//                embot::hw::LED l = pImpl->sensor_to_led(config.sensors[n]);
//                embot::hw::led::off(l);
            }
            
        }            
    }
    
    // we also enables for acquisitions all the sensors that are connected and we keep pImpl->config.sensors[s].params we have, until some can message changes them    
    pImpl->sensors_mask_of_active = pImpl->sensors_mask_of_connected;
    //and update the activeSensorsIndex
    pImpl->updateActiveSensorsIndex();
    
                       
    embot::core::print("theFAPreader2::initialise() -> found: " + str);

//#if defined(CONTINUOUS_ACQUISITION)
//    embot::core::print("theFAPreader2::initialise() -> starting acquisition @ " + std::to_string(pImpl->periodAcquisition/1000) + " ms");
//    pImpl->acquisitionchain_start(true);
//#endif


    return true;
}


bool embot::app::application::theFAPreader2::start(embot::core::relTime period)
{   
    if(0 != period)
    {
        pImpl->periodTX = period;
    }
    embot::core::print("theFAPreader2::start() -> starting streaming @ " + std::to_string(pImpl->periodTX/1000) + " ms");
    return pImpl->start();
}


bool embot::app::application::theFAPreader2::stop()
{ 
    embot::core::print("theFAPreader2::stop() -> stopping streaming");    
    return pImpl->stop();
}


bool embot::app::application::theFAPreader2::get(std::vector<embot::prot::can::Frame> &frames2transmit)
{   
    return pImpl->get(frames2transmit);
}


bool embot::app::application::theFAPreader2::isvalid(embot::os::EventMask evtmask) const
{
    return pImpl->isvalid(evtmask);
}

bool embot::app::application::theFAPreader2::process(embot::os::EventMask evtmask, std::vector<embot::prot::can::Frame> &outframes)
{   
    return pImpl->process(evtmask, outframes);
}



// interface to CANagentPOS

bool embot::app::application::theFAPreader2::set(const embot::prot::can::analog::polling::Message_POS_CONFIG_SET::Info &info)
{
    // if ticking: stop it
    bool wasticking = pImpl->ticking;
    if(true == wasticking)
    {
        stop();
    }
    
    // evaluate info.type and info.id
    
    if(embot::prot::can::analog::posID::all == info.id)
    {
        // we do something to every sensor 
        if(embot::prot::can::analog::posTYPE::none == info.type)
        {
            // we disable the acquisition of every sensor and we reset the calibration parameters
            pImpl->sensors_mask_of_active = 0;
            for(uint8_t n=0; n<numberofpositions; n++)
            {
                pImpl->config.sensors[n].params.reset();         
            }
        }
        else if(embot::prot::can::analog::posTYPE::angleDeciDeg == info.type)
        {
            // we set all the sensors we have with info.descriptor[0], as long as its .enabled is true
            if(true == info.descriptor[0].enabled)
            {
                for(uint8_t n=0; n<numberofpositions; n++)
                {
                    if(true == embot::core::binary::bit::check(pImpl->sensors_mask_of_connected, n))
                    {
                        embot::core::binary::bit::set(pImpl->sensors_mask_of_active, n);
                        uint8_t la = static_cast<uint8_t>(info.descriptor[0].label);
                        pImpl->config.sensors[n].params.load(static_cast<embot::prot::can::analog::posLABEL>(la+n), info.descriptor[0].calib);
                    }
                }
            }
        }
        
    }
    else // info.id is not embot::prot::can::analog::posID::all, so it is a posID::one, or posID::two, or posID::three ... or posID::fourteen
    {
        // we do something to the sensor w/ ID = info.id
        embot::hw::ANY id = static_cast< embot::hw::ANY>(embot::core::tointegral(info.id));
        for(uint8_t n=0; n<numberofpositions; n++)
        {
            // i need to find the id inside the configured sensors which could be found at bootstrap
            if((id == pImpl->config.sensors[n].id) && (true == embot::core::binary::bit::check(pImpl->sensors_mask_of_connected, n)))
            {
                // found it!
                if((embot::prot::can::analog::posTYPE::angleDeciDeg == info.type) && (true == info.descriptor[0].enabled))
                {                    
                    // we surely enable its reading and ... configure to use a given calibration and to transmit its value w/ a given label
                    embot::core::binary::bit::set(pImpl->sensors_mask_of_active, n);
                    pImpl->config.sensors[n].params.load(info.descriptor[0].label, info.descriptor[0].calib);
                }
                else // if((embot::prot::can::analog::posTYPE::none == info.type) || (false == info.descriptor[0].enabled))
                {
                    // we disable its reading and reset calibration and label
                    embot::core::binary::bit::clear(pImpl->sensors_mask_of_active, n);
                    pImpl->config.sensors[n].params.reset(); 
                }
            }
        }    
        
    }
    
    // after we have (maybe) modified sensors_mask_of_active, we must update the vector which contains the sensors to read
    pImpl->updateActiveSensorsIndex();
    
    
    if(true == wasticking)
    {
        start();
    }
    
    return true;    
}

bool embot::app::application::theFAPreader2::get(const embot::prot::can::analog::polling::Message_POS_CONFIG_GET::Info &info, embot::prot::can::analog::polling::Message_POS_CONFIG_GET::ReplyInfo &replyinfo)
{    

    replyinfo.type = info.type;
    if(info.type == embot::prot::can::analog::posTYPE::angleDeciDeg)
    {
//        replyinfo.descriptor[0] = pImpl->canconfig.descriptor[0];
//        replyinfo.descriptor[1] = pImpl->canconfig.descriptor[1];
    }
    
    return true;    
}


bool embot::app::application::theFAPreader2::set(const embot::prot::can::analog::polling::Message_POS_TRANSMIT::Info &info)
{
    if((true == info.transmit) && (info.txperiod > 0))
    {
        start(info.txperiod);
    }
    else
    {
        stop();        
    }
    
    return true;    
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


