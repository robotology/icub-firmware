
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_application_theFAPreader.h"



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
#include "embot_hw_tlv493d.h"

#include "embot_app_theLEDmanager.h"

#include "embot_os_theScheduler.h"

#include "embot_app_application_theCANtracer.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


std::string to_string(embot::hw::TLV493D id)
{
    constexpr std::array<const char *, embot::core::tointegral(embot::hw::TLV493D::maxnumberof)> TLV493D_map =
    { 
        "TLV493D::one", "TLV493D::two", "TLV493D::three", "TLV493D::four", "TLV493D::five", "TLV493D::six"
    };
    uint8_t pos = embot::core::tointegral(id);
    return (pos < TLV493D_map.size()) ? TLV493D_map[pos] : "TLV493D::none";    
}



//#define DEBUG_COMPENSATE_SENSOR

#define CONTINUOUS_ACQUISITION
    
struct embot::app::application::theFAPreader::Impl
{ 
    
    void print(const std::string &str)
    {
//        embot::core::print(str);
    }
    
    // we have numberofpositions = 2 sensors, hence we have 2 config
        
    struct canConfig
    {
        std::array<embot::prot::can::analog::polling::deciDegPOSdescriptor, numberofpositions> descriptor;
        canConfig() { reset(); }
        void reset()
        {
            for(auto &a : descriptor)
            {
                a.reset();
            }
        }
    };
    

    Config config {};
        
    uint8_t numofvalidsensors {0};
    
    std::vector<uint8_t> validIDpositions {};
    
    embot::os::EventMask globaleventmask {0};
           
    bool ticking {false};
    std::uint8_t acquisitionmask {0};
    std::uint8_t sensorstoacquiremask {0};
    uint8_t sensorspresencemask {0};
        
    embot::os::Timer *timerAcquisition {nullptr};
    embot::os::Action actionAcquisition {};        
    embot::core::relTime periodAcquisition {50*embot::core::time1millisec};
    embot::os::Timer *timerTOUT {nullptr};
    embot::core::relTime maxTOUT {3*embot::core::time1millisec};
    
    embot::os::Timer *timerTX {nullptr};
    embot::os::Action actionTX {};
    embot::os::Action actionTOUT {};        
        
    embot::core::relTime periodTX {10*embot::core::time1millisec};

    canConfig canconfig {};
    
    static constexpr embot::hw::tlv493d::Position valueOfPositionCHIPnotinitted = 2000*100;         // which will results in 2000 degrees or 20000 decidegrees
    static constexpr embot::hw::tlv493d::Position valueOfPositionACQUISITIONnotvalid = 1000*100;    // which will results in 1000 degrees or 10000 decidegrees
    
    embot::os::rtos::mutex_t *_mtxOf_positions {nullptr};
    //std::array<embot::hw::tlv493d::Position, numberofpositions> positions {0};
    // the following is for canprotocol
    std::array<embot::prot::can::analog::deciDeg, 3> decidegvalues {0};
    
    struct threadsafePositions
    {
        embot::os::rtos::mutex_t *_mtx {nullptr};
        std::array<embot::hw::tlv493d::Position, numberofpositions> _positions {0};
        threadsafePositions() : _mtx(embot::os::rtos::mutex_new()) {}
        bool set(embot::hw::tlv493d::Position p, uint8_t n, embot::core::relTime tout = embot::core::reltimeWaitForever)
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
        
        embot::hw::tlv493d::Position get(uint8_t n, embot::core::relTime tout = embot::core::reltimeWaitForever)
        {
            embot::hw::tlv493d::Position p {0};
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
        
        bool get(std::array<embot::hw::tlv493d::Position, numberofpositions> &pp, embot::core::relTime tout = embot::core::reltimeWaitForever)
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
    
   
    Impl() 
    {   
        ticking = false;  
        acquisitionmask = 0;
        sensorstoacquiremask = 0;
        sensorspresencemask = 0;
        periodTX = 10*embot::core::time1millisec;
        
        numofvalidsensors = 0;

        timerAcquisition = new embot::os::Timer;  
        timerTX = new embot::os::Timer; 
        timerTOUT = new embot::os::Timer;         
        
        #warning see it
        canconfig.reset(); 
        canconfig.descriptor[0].label = embot::prot::can::analog::posLABEL::zero;
        canconfig.descriptor[1].label = embot::prot::can::analog::posLABEL::one;
        
        //positions[0] = positions[1] = 0;  
        
        tspositions = new threadsafePositions;
        
        decidegvalues[0] = decidegvalues[1] = decidegvalues[2] = 0;   

        validIDpositions.reserve(numberofpositions);

    }
    
    bool isvalidposition(const embot::hw::tlv493d::Position &po) const
    {
        if((po == valueOfPositionCHIPnotinitted) || (po == valueOfPositionACQUISITIONnotvalid))
        {
            return false;
        }
        return true;        
    }
   
    bool start();
    bool stop();    
    bool tick(std::vector<embot::prot::can::Frame> &replies);
    bool isvalid(embot::os::EventMask evtmask) const;
    bool process(embot::os::EventMask evtmask);
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
    
    bool acquisition_start(std::uint8_t n);
    bool acquisition_retrieve(std::uint8_t n);
    bool acquisition_processing(std::uint8_t n);
    bool acquisition_transmit(std::vector<embot::prot::can::Frame> &replies);
    
    
    static void alertdataisready00(void *p)
    {
        embot::app::application::theFAPreader::Impl *mypImpl = reinterpret_cast<embot::app::application::theFAPreader::Impl*>(p);
        
        if(true == mypImpl->ticking)
        {
            mypImpl->config.reader->setEvent(mypImpl->config.sensors[0].dataready);
        }   
    }
    
    
    static void alertdataisready01(void *p)
    {
        embot::app::application::theFAPreader::Impl *mypImpl = reinterpret_cast<embot::app::application::theFAPreader::Impl*>(p);
        
        if(true == mypImpl->ticking)
        {
            mypImpl->config.reader->setEvent(mypImpl->config.sensors[1].dataready);
        }        
    }
        
    static constexpr std::array<embot::core::fpCaller, numberofpositions> alertdataisready = { alertdataisready00, alertdataisready01 };
    
    
    
//    static void alertnoreply(void *p)
//    {
//        embot::app::application::theFAPreader::Impl *pimpl = reinterpret_cast<embot::app::application::theFAPreader::Impl*>(p);
//        pimpl->config.reader->setEvent(pimpl->config.events.noreply);     
//    }    
    
    struct cbkData
    {
        cbkData() = default;
        cbkData(embot::app::application::theFAPreader::Impl *p, uint8_t _n) : pimpl(p), n(_n) {}
        embot::app::application::theFAPreader::Impl *pimpl {nullptr};
        uint8_t n {0};
    };
    
    std::array<cbkData, numberofpositions> cbkdataarray = { };
    
    static void acquisition_parallel_alert_dataready(void *p)
    {
        cbkData *cbkdata = reinterpret_cast<embot::app::application::theFAPreader::Impl::cbkData*>(p);
        cbkdata->pimpl->config.reader->setEvent(cbkdata->pimpl->config.sensors[cbkdata->n].dataready);
    }    


    static void acquisition_daisychain_alert_dataready(void *p)
    {
        embot::app::application::theFAPreader::Impl *pimpl = reinterpret_cast<embot::app::application::theFAPreader::Impl*>(p);
        pimpl->config.reader->setEvent(pimpl->config.sensors[pimpl->validIDpositions[pimpl->daisychain_counter]].dataready);
//        pimpl->print("acquisition_daisychain_alert_dataready()");
    }
    
    
    void timerTOUT_start()
    {
        if(0 != maxTOUT)
        {
            embot::os::Timer::Config cfgtmt(maxTOUT, actionTOUT, embot::os::Timer::Mode::oneshot);
            //timerTOUT->stop();
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
        if(daisychain_counter <= validIDpositions.size())
        {
//            print("acquisition_daisychain_trigger() @ " + embot::core::TimeFormatter(embot::core::now()).to_string());
            // send event to thread for the current daisychained
            uint8_t n = validIDpositions[daisychain_counter];
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
//        if(n == daisychain_counter)
//        {
//            print("acquisition_daisychain_on_askdata(" + std::to_string(n) + ") @ " + embot::core::TimeFormatter(embot::core::now()).to_string());
            // config a callback which sends the dataready event
            embot::core::Callback cbk {acquisition_daisychain_alert_dataready, this};
            // start acquisition
            embot::hw::tlv493d::acquisition(config.sensors[n].id, cbk);
            
            timerTOUT_start();
//        }
    }     

    void acquisition_daisychain_on_dataready(uint8_t n)
    {
//        if(n == daisychain_counter)
//        {
            timerTOUT_stop();    
            
 //           print("acquisition_daisychain_on_dataready(" + std::to_string(n) + ") @ " + embot::core::TimeFormatter(embot::core::now()).to_string());            
            // retrieve data from sensor n
            embot::hw::tlv493d::Position pos {0};
    
            if(embot::hw::resOK != embot::hw::tlv493d::read(config.sensors[n].id, pos))
            {
                pos = valueOfPositionACQUISITIONnotvalid;
            }
            
            embot::hw::TLV493D id = config.sensors[n].id;            
            print(to_string(id) + " (n=" +  std::to_string(n) + ") is " + std::to_string(pos) + " centiDEG @ " + embot::core::TimeFormatter(embot::core::now()).to_string()); 
    
            #warning in here we use n or ... daisychain_counter??? we use n. even if some n keep non valid values
            tspositions->set(pos, n);
            
            // increment
            daisychain_counter++;
            
            // if not last, trigger a start()
            if(daisychain_counter < validIDpositions.size())
            {
                acquisition_daisychain_trigger();
            }
            else
            {
                daisychain_counter = 0;
            }
//        }
    } 

    void acquisition_daisychain_noreply()
    {
        embot::hw::TLV493D id = config.sensors[validIDpositions[daisychain_counter]].id;
        print("acquisition_daisychain_noreply() of " + to_string(id) + ", index = " + std::to_string(daisychain_counter) + ", @ " + embot::core::TimeFormatter(embot::core::now()).to_string());

        timerTOUT_stop();
        
        // then... what to do? 
        // - i could mark a failure mask
        // - i could get the mask at the end of acquisition chain and fill a can print frame to transmit 
        // for now... i just trigger the next one as if i had received a acquisition_daisychain_on_dataready()
        
        // increment
        daisychain_counter++;
        
        // if not last, trigger a start()
        if(daisychain_counter < validIDpositions.size())
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
        for(uint8_t i=0; i<validIDpositions.size(); i++)
        {
            uint8_t n = validIDpositions[i];
            eventmask |= config.sensors[n].askdata;  
            embot::core::binary::bit::set(acquisition_fullyparallel_maskofrequests, n);
            embot::hw::TLV493D id = config.sensors[n].id;            
            str += to_string(id);
            str += " ";
        }
        config.reader->setEvent(eventmask); 
        
        print("acquisition_fullyparallel_start() for " + str + "@ " + embot::core::TimeFormatter(embot::core::now()).to_string());
               
        timerTOUT_start();
    }
    
   
    void acquisition_fullyparallel_on_askdata(uint8_t n)
    {
        embot::hw::TLV493D id = config.sensors[n].id;
        print("acquisition_fp_on_askdata(" + std::to_string(n) + ") for " + to_string(id) + " @ " + embot::core::TimeFormatter(embot::core::now()).to_string());
//        print("acquisition_fp_on_askdata(" + std::to_string(n) + ") @ " + embot::core::TimeFormatter(embot::core::now()).to_string());
        // config a callback which sends the dataready event
        cbkdataarray[n] = {this, n};
        embot::core::Callback cbk {acquisition_parallel_alert_dataready, &cbkdataarray[n]};
        // start acquisition
        embot::hw::tlv493d::acquisition(config.sensors[n].id, cbk);
    }     

    void acquisition_fullyparallel_on_dataready(uint8_t n)
    {
//        print("acquisition_fp_on_dataready(" + std::to_string(n) + ") @ " + embot::core::TimeFormatter(embot::core::now()).to_string());
        // retrieve data from sensor n
        embot::hw::tlv493d::Position pos {0};

        if(embot::hw::resOK != embot::hw::tlv493d::read(config.sensors[n].id, pos))
        {
            pos = valueOfPositionACQUISITIONnotvalid;
        }
        
        embot::hw::TLV493D id = config.sensors[n].id;       
        print(to_string(id) + " (n=" +  std::to_string(n) + ") is " + std::to_string(pos) + " centiDEG @ " + embot::core::TimeFormatter(embot::core::now()).to_string()); 
  
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
        for(uint8_t i=0; i<validIDpositions.size(); i++)
        {
            uint8_t n = validIDpositions[i];
            if(embot::core::binary::bit::check(maskOFmissing, n))
            {
                embot::hw::TLV493D id = config.sensors[n].id;
                str += to_string(id);
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
    
    uint32_t acquisition_mod2parallel_maskofrequests = 0;
    uint32_t acquisition_mod2parallel_maskofreplies = 0;    
    uint8_t mod2_index = 0;
    uint8_t mod2_number = 0;
    uint8_t mod2_replies = 0;
    enum class mod2Mode : uint8_t { even = 0, odd = 1};
    
    mod2Mode currmod2mode = mod2Mode::even;
    void acquisition_mod2parallel_start(mod2Mode mo)
    {
        std::string st = ((mo == mod2Mode::odd) ? "odd" : "even");
        print("acquisition_mod2parallel_start(" + st + ") @ " + embot::core::TimeFormatter(embot::core::now()).to_string());
        currmod2mode = mo;
        mod2_index = 0;
        mod2_number = 0;
        mod2_replies = 0;
        acquisition_mod2parallel_maskofrequests = acquisition_mod2parallel_maskofreplies = 0;
        embot::os::EventMask eventmask = 0;
        std::string str {};
        for(uint8_t i=0; i<validIDpositions.size(); i++)
        {
            if(embot::core::tointegral(mo) == (i%2))
            {
                mod2_number++;
                uint8_t n = validIDpositions[i];
                eventmask |= config.sensors[n].askdata;
                embot::core::binary::bit::set(acquisition_mod2parallel_maskofrequests, n); 
                embot::hw::TLV493D id = config.sensors[n].id;            
                str += to_string(id);
                str += " ";
            }                
        }
        
        if(0 != eventmask)
        {        
            config.reader->setEvent(eventmask); 
            print("acquisition_mod2parallel_start() for " + str + "@ " + embot::core::TimeFormatter(embot::core::now()).to_string());       
            timerTOUT_start();  
        } 
        else        
        {
            print(std::string("acquisition_mod2parallel_start() does nothing as there are no sensors ") + "@ " + embot::core::TimeFormatter(embot::core::now()).to_string()); 
        }    

    }
    
    void acquisition_mod2parallel_on_askdata(uint8_t n)
    {
        embot::hw::TLV493D id = config.sensors[n].id;
        print("acquisition_mod2parallel_on_askdata(" + std::to_string(n) + ") for " + to_string(id) + " @ " + embot::core::TimeFormatter(embot::core::now()).to_string());
//        print("acquisition_mod2parallel_on_askdata(" + std::to_string(n) + ") @ " + embot::core::TimeFormatter(embot::core::now()).to_string());
        // config a callback which sends the dataready event
        cbkdataarray[n] = {this, n};
        embot::core::Callback cbk {acquisition_parallel_alert_dataready, &cbkdataarray[n]};
        // start acquisition
        embot::hw::tlv493d::acquisition(config.sensors[n].id, cbk);
    }     

    void acquisition_mod2parallel_on_dataready(uint8_t n)
    {
        print("acquisition_mod2parallel_on_dataready(" + std::to_string(n) + ") @ " + embot::core::TimeFormatter(embot::core::now()).to_string());
        // retrieve data from sensor n
        embot::hw::tlv493d::Position pos {0};

        if(embot::hw::resOK != embot::hw::tlv493d::read(config.sensors[n].id, pos))
        {
            pos = valueOfPositionACQUISITIONnotvalid;
        }
        
        embot::hw::TLV493D id = config.sensors[n].id;       
        print(to_string(id) + " (n=" +  std::to_string(n) + ") is " + std::to_string(pos) + " centiDEG @ " + embot::core::TimeFormatter(embot::core::now()).to_string()); 
   
        tspositions->set(pos, n);
        
        embot::core::binary::bit::set(acquisition_mod2parallel_maskofreplies, n);
        
        if(acquisition_mod2parallel_maskofreplies == acquisition_mod2parallel_maskofrequests)
        {
            // ok, we have finished one stage. i stop the timer
            timerTOUT_stop();
        }        
        
        mod2_replies++;        
        
        if((mod2_replies == mod2_number) && (currmod2mode == mod2Mode::even))
        {
            acquisition_mod2parallel_start(mod2Mode::odd);
        }
    }   

    void acquisition_mod2parallel_noreply()
    {
        uint32_t maskOFmissing = acquisition_mod2parallel_maskofrequests ^ acquisition_mod2parallel_maskofreplies;
        std::string str {};
        for(uint8_t i=0; i<validIDpositions.size(); i++)
        {
            uint8_t n = validIDpositions[i];
            if(embot::core::binary::bit::check(maskOFmissing, n))
            {
                tspositions->set(0xffff, n);
                embot::hw::TLV493D id = config.sensors[n].id;
                str += to_string(id);
                str += " ";
            }            
        }
        
        print("acquisition_mod2parallel_noreply() of " + str + " @ " + embot::core::TimeFormatter(embot::core::now()).to_string());        
        
        timerTOUT_stop();
        
        // then... what to do? 
        // - i could use the differences between acquisition_md2parallel_maskofreplies and acquisition_mod2parallel_maskofrequests
        // - i could fill a can print frame to transmit 
        // for now... i just stop the timer and start the odd if i were in even
        if((currmod2mode == mod2Mode::even))
        {
            acquisition_mod2parallel_start(mod2Mode::odd);
        }
        
    } 


    

    bool acquisition_get(std::vector<embot::prot::can::Frame> &replies);    
                      
};


// this starts the tx actually
bool embot::app::application::theFAPreader::Impl::start()
{ 
#if !defined(CONTINUOUS_ACQUISITION)
    acquisitionchain_start(true);
#endif    
    
    embot::os::Timer::Config cfgtx(periodTX, actionTX, embot::os::Timer::Mode::forever);
    timerTX->start(cfgtx);
    ticking = true;    
    return true;            
}

// this stops the tx
bool embot::app::application::theFAPreader::Impl::stop()
{
#if !defined(CONTINUOUS_ACQUISITION)
    acquisitionchain_start(false);
#endif   
    
    timerTX->stop();
    ticking = false;    
    return true;
}

// this retrieves the 
bool embot::app::application::theFAPreader::Impl::get(std::vector<embot::prot::can::Frame> &frames2transmit)
{
    // that is called by the transmitter thread only ... i may check vs thread correctness but i dont so that i am quicker

//    if(config.transmitter != embot::os::theScheduler::getInstance().scheduled())
//    {
//        return false;
//    }
    
    acquisition_get(frames2transmit);    
    return true;
}

bool embot::app::application::theFAPreader::Impl::isvalid(embot::os::EventMask evtmask) const
{ 
    return (0 == (evtmask & globaleventmask)) ? false : true;    
}

bool embot::app::application::theFAPreader::Impl::process(embot::os::EventMask evtmask)
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

            case AcquisitionMode::mod2parallel:
            {
                acquisition_mod2parallel_start(mod2Mode::even);
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

            case AcquisitionMode::mod2parallel:
            {
                acquisition_mod2parallel_noreply();
            } break;            
                        
            default:
            {
            } break;
        }       
    }
    
    // check the events related to the sensors 
    for(uint8_t i=0; i<validIDpositions.size(); i++)
    {
        // the events for asking to the sensor 
        uint8_t n = validIDpositions[i];
        
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
                
                case AcquisitionMode::mod2parallel:
                {
                    acquisition_mod2parallel_on_askdata(n);
                } break;                 
                
                default:
                {
                } break;
            }       
//            if(config.acquisitionmode == AcquisitionMode::daisychain) 
//            {
//                acquisition_daisychain_on_askdata(config.sensors[n].askdata);
//            }                
            //acquisition_start(n);
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

                case AcquisitionMode::mod2parallel:
                {
                    acquisition_mod2parallel_on_dataready(n);
                } break;  
                
                default:
                {
                } break;
            } 
//            if(config.acquisitionmode == AcquisitionMode::daisychain) 
//            {
//                acquisition_daisychain_on_dataready(config.sensors[n].dataready);
//            }   
//            acquisition_retrieve(n);    
//            acquisition_processing(n);
        }
    }

    return true;    
}

bool embot::app::application::theFAPreader::Impl::acquisition_get(std::vector<embot::prot::can::Frame> &replies)
{   
            
    std::array<embot::hw::tlv493d::Position, numberofpositions> positions {0};
    tspositions->get(positions);
        
    // we are ready to transmit    
    embot::prot::can::Frame frame;     

    std::string str;    
    
    for(uint8_t i=0; i<validIDpositions.size(); i++)
    {
        uint8_t n = validIDpositions[i];
        
        embot::prot::can::analog::periodic::Message_POS msg;
        embot::prot::can::analog::periodic::Message_POS::Info info;  
    
        info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();   
        
        // so far i load one value in one packet ...
        int16_t v =  (0xffff == positions[n]) ? +10000 : positions[n]/10;
        std::array<embot::prot::can::analog::deciDeg, 3> values = { v, 0, 0};
        
        embot::hw::TLV493D id = config.sensors[n].id; 
        str += to_string(id);
        str += " = ";
        str += std::to_string(v/10);
        str += " DEG ";
        
        info.loadDeciDeg(static_cast<embot::prot::can::analog::posLABEL>(embot::core::tointegral(id)), 1, values);
           
        msg.load(info);
        msg.get(frame);
        replies.push_back(frame); 
        if(0xffff == positions[n])
        {
            embot::app::theCANtracer &tr = embot::app::theCANtracer::getInstance();
            tr.print("FAP" + std::to_string(n) + "err", replies);
        }
    }

    print("theFAPreader transmits: " + str + embot::core::TimeFormatter(embot::core::now()).to_string());

             
    return true;           
}



bool embot::app::application::theFAPreader::Impl::acquisition_transmit(std::vector<embot::prot::can::Frame> &replies)
{   
        
//    if(false == embot::core::binary::mask::check(acquisitionmask, sensorspresencemask))
//    {
//        return false;
//    }
    
//    positions[0] = tspositions->get(0);
//    positions[1] = tspositions->get(1);
    
    #warning TBD: this code manages only two positions. we must extend it.
    
    std::array<embot::hw::tlv493d::Position, numberofpositions> positions {0};
    tspositions->get(positions);
        
    // we are ready to transmit    
    embot::prot::can::Frame frame;   
                 
    embot::prot::can::analog::periodic::Message_POS msg;
    embot::prot::can::analog::periodic::Message_POS::Info info;  
    
    info.canaddress = embot::app::theCANboardInfo::getInstance().cachedCANaddress();   
    if((true == canconfig.descriptor[0].enabled) && (true == canconfig.descriptor[1].enabled))
    {   // we transmit two
        if(isvalid(positions[0]))
        {
            decidegvalues[0] = canconfig.descriptor[0].transform(positions[0]/10);
        }
        else
        {
            decidegvalues[0] = positions[0]/10;
        }
        if(isvalid(positions[1]))
        {
            decidegvalues[1] = canconfig.descriptor[1].transform(positions[1]/10);
        }
        else
        {
            decidegvalues[1] = positions[1]/10;
        }
        info.loadDeciDeg(canconfig.descriptor[0].label, 2, decidegvalues);
    }
    else if((true == canconfig.descriptor[0].enabled))
    {   // we transmit only the first
        if(isvalid(positions[0]))
        {
            decidegvalues[0] = canconfig.descriptor[0].transform(positions[0]/10);
        }
        else
        {
            decidegvalues[0] = positions[0]/10;
        }
        info.loadDeciDeg(canconfig.descriptor[0].label, 1, decidegvalues);
    }
    else if((true == canconfig.descriptor[1].enabled))
    {   // we transmit only the second
        if(isvalid(positions[1]))
        {
            decidegvalues[0] = canconfig.descriptor[1].transform(positions[1]/10);
        }
        else
        {
            decidegvalues[0] = positions[1]/10;
        }
        info.loadDeciDeg(canconfig.descriptor[1].label, 1, decidegvalues);
    }
    else
    {   // we transmit none
        
    }
    
    msg.load(info);
    msg.get(frame);
    replies.push_back(frame);        

    acquisitionmask = 0;
    sensorstoacquiremask = 0;
             
    return true;           
}


bool embot::app::application::theFAPreader::Impl::acquisition_start(std::uint8_t n)
{
    acquisitionmask =  0;   
    sensorstoacquiremask = 0;
    
    embot::core::Callback cbk(alertdataisready[n], this);
    embot::hw::tlv493d::acquisition(config.sensors[1].id, cbk);
    embot::core::binary::bit::set(sensorstoacquiremask, n);
    
//    if(0 == sensorspresencemask)
//    {
//        // if we dont have any sensor because the init has failed, then we must transmit the failure anyway
//        // hence, we emit a alertdataisready00
//        alertdataisready00(this);        
//        return true;
//    }
//      
//    // else, we start acquisition from the sensors which are available
//    if(embot::core::binary::bit::check(sensorspresencemask, 0))
//    {        
//        embot::core::Callback cbk00(alertdataisready00, this);
//        embot::hw::tlv493d::acquisition(config.sensors[0].id, cbk00);
//        embot::core::binary::bit::set(sensorstoacquiremask, 0);
//    }

//    if(embot::core::binary::bit::check(sensorspresencemask, 1))
//    {
//        embot::core::Callback cbk01(alertdataisready01, this);
//        embot::hw::tlv493d::acquisition(config.sensors[1].id, cbk01);
//        embot::core::binary::bit::set(sensorstoacquiremask, 1);
//    }
       
    return true;
}


bool embot::app::application::theFAPreader::Impl::acquisition_retrieve(std::uint8_t n)
{
    embot::hw::tlv493d::Position pos {0};
    
    if(embot::hw::resOK != embot::hw::tlv493d::read(config.sensors[n].id, pos))
    {
        pos = valueOfPositionACQUISITIONnotvalid;
    }
    
    tspositions->set(pos, n);
    
    return true;
}

bool embot::app::application::theFAPreader::Impl::acquisition_processing(std::uint8_t n)
{    
//    embot::core::binary::bit::set(acquisitionmask, n);
    
//    n++;
    
//    if(n < numofvalidsensors)
//    {
//        // trigger the next acquisition
//        acquisition_start(n);
//    }
//    else
//    {
//        // trigger the transmission
//        config.transmitter->setEvent(config.events.transmit);
//    }
        
    return true;    
}


// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------



embot::app::application::theFAPreader& embot::app::application::theFAPreader::getInstance()
{
    static theFAPreader* p = new theFAPreader();
    return *p;
}

embot::app::application::theFAPreader::theFAPreader()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::application::theFAPreader::~theFAPreader() { }

         
bool embot::app::application::theFAPreader::initialise(const Config &config)
{
    pImpl->config = config;
    
    // this event triggers a periodic reading. 
    pImpl->actionAcquisition.load(embot::os::EventToThread(pImpl->config.events.acquire, pImpl->config.reader));
    if(pImpl->config.acquisitionperiod > 10*embot::core::time1millisec)
    {
        pImpl->periodAcquisition = pImpl->config.acquisitionperiod;
    }
    
    // this event signals a no reply in data reading
    pImpl->actionTOUT.load(embot::os::EventToThread(pImpl->config.events.noreply, pImpl->config.reader));
    
    // this event triggers a periodic transmission with period configured by can message
    pImpl->actionTX.load(embot::os::EventToThread(pImpl->config.events.transmit, pImpl->config.transmitter));
     
    pImpl->tspositions->set(pImpl->valueOfPositionCHIPnotinitted, 0);
    pImpl->tspositions->set(pImpl->valueOfPositionCHIPnotinitted, 1);
    
    pImpl->numofvalidsensors = 0;
    
    std::string str {};
    pImpl->globaleventmask = pImpl->config.events.acquire | pImpl->config.events.noreply;
    pImpl->maxTOUT = pImpl->config.acquisitiontimeout;
    for(uint8_t n=0; n<numberofpositions; n++)
    {
        if(embot::hw::TLV493D::none != pImpl->config.sensors[n].id)
        {  
            bool ok {false};
            if(false == pImpl->config.sensors[n].config.isvalid())
            {
                // someone xternally has initted it. that ok for me.
                ok = true;
            }
            else 
            {                
                if(embot::hw::resOK == embot::hw::tlv493d::init(pImpl->config.sensors[n].id, pImpl->config.sensors[n].config))
                {
                    ok = true;
                } 
                embot::hw::sys::delay(50*embot::core::time1millisec); // why???????
            }
            
            if(ok)
            {
                if((pImpl->config.sensors[n].timeout != 0) && (pImpl->config.sensors[n].noreply != 0))
                {
                    pImpl->maxTOUT = std::max(pImpl->maxTOUT, pImpl->config.sensors[n].timeout);
                }
                pImpl->validIDpositions.push_back(n);
                pImpl->numofvalidsensors++;
                embot::core::binary::bit::set(pImpl->sensorspresencemask, static_cast<uint8_t>(pImpl->config.sensors[n].id));
                pImpl->globaleventmask |= (pImpl->config.sensors[n].askdata | pImpl->config.sensors[n].dataready | pImpl->config.sensors[n].noreply);   

                embot::hw::TLV493D id = config.sensors[n].id;            
                str += to_string(id);
                str += " ";                
            }
            
        }            
    }
    
    
    
    // in here we use a led wave to alert about a missing sensor ... i dont need that for now
    
//    if(false == embot::core::binary::mask::check(pImpl->sensorspresencemask, static_cast<uint8_t>(0b11)))
//    {        
//        embot::app::LEDwaveT<64> ledwave(100*embot::core::time1millisec, 50, std::bitset<64>(0b0101010101));
//        
//        if(true == embot::core::binary::bit::check(pImpl->sensorspresencemask, 0))
//        {
//            ledwave.load(20, std::bitset<64>(0b0001), 4); 
//        }
//        else if(true == embot::core::binary::bit::check(pImpl->sensorspresencemask, 1))
//        {
//            ledwave.load(20, std::bitset<64>(0b0101), 4);
//        }
//        
//        embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();
//        theleds.get(embot::hw::LED::one).wave(&ledwave);  
//    }
                
    embot::core::print("theFAPreader::initialise() -> found: " + str);

#if defined(CONTINUOUS_ACQUISITION)
    embot::core::print("theFAPreader::initialise() -> starting acquisition @ " + std::to_string(pImpl->periodAcquisition/1000) + " ms");
    pImpl->acquisitionchain_start(true);
#endif

     
    return true;
}


bool embot::app::application::theFAPreader::start(embot::core::relTime period)
{   
    if(0 != period)
    {
        pImpl->periodTX = period;
    }
    embot::core::print("theFAPreader::start() -> starting streaming @ " + std::to_string(pImpl->periodTX/1000) + " ms");
    return pImpl->start();
}


bool embot::app::application::theFAPreader::stop()
{ 
    embot::core::print("theFAPreader::stop() -> stopping streaming");    
    return pImpl->stop();
}


bool embot::app::application::theFAPreader::get(std::vector<embot::prot::can::Frame> &frames2transmit)
{   
    return pImpl->get(frames2transmit);
}


bool embot::app::application::theFAPreader::isvalid(embot::os::EventMask evtmask) const
{
    return pImpl->isvalid(evtmask);
}

bool embot::app::application::theFAPreader::process(embot::os::EventMask evtmask)
{   
    return pImpl->process(evtmask);
}



// interface to CANagentPOS

bool embot::app::application::theFAPreader::set(const embot::prot::can::analog::polling::Message_POS_CONFIG_SET::Info &info)
{
    // if ticking: stop it
    if(true == pImpl->ticking)
    {
        stop();
    }
    
    if(info.type == embot::prot::can::analog::posTYPE::angleDeciDeg)
    {
        pImpl->canconfig.descriptor[0] = info.descriptor[0];
        pImpl->canconfig.descriptor[1] = info.descriptor[1];
    }
    
    return true;    
}

bool embot::app::application::theFAPreader::get(const embot::prot::can::analog::polling::Message_POS_CONFIG_GET::Info &info, embot::prot::can::analog::polling::Message_POS_CONFIG_GET::ReplyInfo &replyinfo)
{    

    replyinfo.type = info.type;
    if(info.type == embot::prot::can::analog::posTYPE::angleDeciDeg)
    {
        replyinfo.descriptor[0] = pImpl->canconfig.descriptor[0];
        replyinfo.descriptor[1] = pImpl->canconfig.descriptor[1];
    }
    
    return true;    
}


bool embot::app::application::theFAPreader::set(const embot::prot::can::analog::polling::Message_POS_TRANSMIT::Info &info)
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


