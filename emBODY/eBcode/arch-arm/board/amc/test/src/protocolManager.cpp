/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Jacopo Losi
 * email:   valentina.gaggero@iit.it, jacopo.losi@iit.it
*/

// header include
#include "protocolManager.h"

// embot library manager includes
#include "embot_os_Thread.h"

// embot library hw includes

uint32_t rxcount {0};   
uint32_t txcount {0};

    
// Private methods definitions
void ProtocolManager::alertonrxframe(void *t)
{
    embot::os::EventThread* evtsk = reinterpret_cast<embot::os::EventThread*>(t);
    if(nullptr != evtsk)
    {
        evtsk->setEvent(ProtocolManager::evRXcanframe);
    }
    rxcount++; 
    embot::core::print("Counter for alertonrxframe: " + std::to_string(rxcount));
}
    
void ProtocolManager::alertontxframe(void *t)
{
    txcount++;       
}

// Public methods definitions
void ProtocolManager::emitONcan(embot::app::eth::theErrorManager::Severity sev, const embot::app::eth::theErrorManager::Caller &caller, const embot::app::eth::theErrorManager::Descriptor &des, const std::string &str)
{
    std::string timenow = embot::core::TimeFormatter(embot::core::now()).to_string();
    std::string eobjstr = (true == caller.isvalid()) ? caller.objectname : "OBJ";
    std::string threadname = (true == caller.isvalid()) ? caller.owner->getName() : "THR";
    std::string severity = embot::app::eth::theErrorManager::to_cstring(sev);
    
    embot::core::print(std::string("[[") + severity + "]] @" + timenow + " (" + eobjstr + ", " + threadname + "): " + str);
    
    if(embot::app::eth::theErrorManager::Severity::trace == sev) 
    {
        return;
    } 

    // you may in here send the diagnostics message over a channel, maybe udp or can
    #if defined(ENABLE_DEBUG_FEAT)
    if(true == des.isvalid())
    {
        embot::prot::can::Frame frame 
        {
            0x701, 
            8, 
            {
                0x66, 
                static_cast<uint8_t>(des.code&0xff), static_cast<uint8_t>((des.code>>8)&0xff), 
                static_cast<uint8_t>((des.code>>16)&0xff), static_cast<uint8_t>((des.code>>24)&0xff),
                0, 
                static_cast<uint8_t>((des.par16)&0xff), static_cast<uint8_t>((des.par16>>8)&0xff)
            }
        };
        
        embot::hw::can::put(canbus, {frame.id, frame.size, frame.data});                         
    }
    #endif
    
    if(embot::app::eth::theErrorManager::Severity::fatal == sev)
    {
        // fatal error. i must do something. maybe send the system in an error state and ...
        embot::app::theLEDmanager::getInstance().get(embot::hw::LED::one).stop();
        for(;;)
        {
            embot::hw::led::toggle(embot::hw::LED::one);
            embot::core::wait(200*embot::core::time1millisec);            
        }
    }     
}

void ProtocolManager::tick(size_t tt)
{                     
    // process rx can frames
    std::uint8_t insideRXQ = embot::hw::can::inputqueuesize(canbus);
    std::uint8_t remainingINrx = 0;
    embot::hw::can::Frame hwframe {};
    for(uint8_t i=0; i<insideRXQ; i++)
    {
        remainingINrx = 0;            
        if(embot::hw::resOK == embot::hw::can::get(canbus, hwframe, remainingINrx))
        {            
            // print info abou the can frame
            // we have a .to_string() for the prot::canframe ...
            
            embot::prot::can::Frame frame {hwframe.id, hwframe.size, hwframe.data};                    
            embot::core::print("RX frame: " + frame.to_string());                
        }
    } 
    
    embot::os::Thread *thr {embot::os::theScheduler::getInstance().scheduled()};
    embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::warning, {"ProtocolManager::tick", thr}, {0xaabbccdd, 0x1234, 0}, "just called");
    
    

    // and now tx the can frames we need
    #if defined(ENABLE_DEBUG_FEAT)
    constexpr size_t num {8};
    for(size_t i=0; i<num; i++)
    {   
        embot::prot::can::Frame frame {0x123, 8, {static_cast<uint8_t>(tt), static_cast<uint8_t>(i), 2, 3, 4, 5, 6, 7}};
        embot::hw::can::put(canbus, {frame.id, frame.size, frame.data});                                       
    }
    embot::hw::can::transmit(canbus);    
    #endif    
}

void ProtocolManager::canProtocolInit(void *t)
{
    // Initialize eventThread masked by the void ptr and pass it to the CAN event callbacks
    embot::os::EventThread* eventhread = nullptr;
    eventhread = reinterpret_cast<embot::os::EventThread*>(t);
    
    // Iniit the CAN bus and its driver
    embot::hw::can::Config canconfig {};
    canconfig.rxcapacity = canRXcapacity;
    canconfig.txcapacity = canTXcapacity;
    canconfig.onrxframe = embot::core::Callback(&ProtocolManager::alertonrxframe, eventhread); 
    canconfig.ontxframe = embot::core::Callback(&ProtocolManager::alertontxframe, eventhread);
    embot::hw::can::init(canbus, canconfig);   
        
    // and ok, enable it
    embot::hw::can::enable(canbus);   
        
    embot::app::eth::theErrorManager::getInstance().initialise(&ProtocolManager::emitONcan);
        
    // Initializing output canframe since id and size if fixed. Only data will be cleaned
    _outputCanFrame.id = 0x551;
    _outputCanFrame.size = 8;
    std::fill(std::begin(_outputCanFrame.filler), std::end(_outputCanFrame.filler), 0);
    std::fill(std::begin(_outputCanFrame.data), std::end(_outputCanFrame.data), 0);
    
    // Initializing and cleaning input canframe
    _inputCanFrame.id = 0x0;
    _inputCanFrame.size = 0;
    std::fill(std::begin(_inputCanFrame.filler), std::end(_inputCanFrame.filler), 0);
    std::fill(std::begin(_inputCanFrame.data), std::end(_inputCanFrame.data), 0);
}

void ProtocolManager::parseTestCommand(ProtocolManager::TestCommand &cmd)
{
    embot::core::print("Entered in ProtocolManager::parseTestCommand()");
    
    embot::hw::can::Frame canframe {};
    std::uint8_t remaining = 0;
    cmd = ProtocolManager::TestCommand::testDummy; // first override as dummy cmd --> it gets initialized in testManagerRun() as testUndefineds
			
    if(embot::hw::resOK != embot::hw::can::get(canbus, canframe, remaining))
    {
        embot::core::print("Failed to get canframe in ProtocolManager::parseTestCommand()");
        return;
    }
    else
    {
        embot::prot::can::Frame frame {canframe.id, canframe.size, canframe.data};                    
        embot::core::print("RX frame: " + frame.to_string()); 
        cmd = static_cast<ProtocolManager::TestCommand>(canframe.data[0]);
    }
}

void ProtocolManager::sendTestResult(uint8_t *outdata)
{
    embot::os::Thread *thr {embot::os::theScheduler::getInstance().scheduled()};
    std::string str = thr->getName();
    str += + "just called";
    embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::warning, {"ProtocolManager::sendTestResult", thr}, {}, str.c_str());
    // overwrite canframe data
    
    for(uint8_t i = 0; i < 8; i++) {_outputCanFrame.data[i] = outdata[i];}
    embot::hw::can::put(canbus, {_outputCanFrame.id, _outputCanFrame.size, _outputCanFrame.data});       
    embot::hw::can::transmit(canbus);
}
    
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// ----

