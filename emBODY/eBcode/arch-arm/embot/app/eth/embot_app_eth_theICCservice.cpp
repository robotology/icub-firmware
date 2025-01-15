
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theICCservice.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_os_rtos.h"
#include "embot_hw_icc_ltr.h"
#include "embot_hw_icc_ltr_bsp.h"

#include <array>
#include <vector>

using namespace std;

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


void tRXicc(void *p) { reinterpret_cast<embot::os::Thread*>(p)->run(); }
void tTXicc(void *p) { reinterpret_cast<embot::os::Thread*>(p)->run(); }
    
struct embot::app::eth::icc::theICCservice::Impl
{
    bool _initted {false};
    Config _config {};
    
    constexpr static const char _objname[] = "theICCservice";
    
    Impl() = default;
    
    bool initialise(const Config &config);
        
    bool set(Pipe pipe, modeTX txm);
    bool set(Pipe pipe, const embot::core::Callback &onr);
    bool set(Pipe pipe, ItemParser itmp);
    
    bool put(Pipe pipe, const Item &item, embot::core::relTime timeout);   
    size_t output(Pipe pipe) const;
    bool flush(Pipe pipe, const embot::core::Callback &cbk);
    bool flush(Pipe pipe, embot::core::relTime timeout);
    bool flushed(Pipe pipe) const;
    
    size_t input(Pipe pipe) const;
    bool get(Pipe pipe, Item &item, size_t &remaining); 
    bool parse(Pipe pipe);        
        
private:    
    
    // we exchange this data structure
    struct lrtData
    {
        size_t size {0};
        std::array<Item, PipeConfig::maxcapacity> items {};
        void clear()
        {
            size = 0;
            //std::memset(items.data(), 0, items.size()*sizeof(Item));
        }            
    };
    static_assert(sizeof(lrtData) <= 1024, "lrtData is more than 1024");
    
    struct iccInfo
    {        
        embot::hw::icc::LTR ltr {embot::hw::icc::LTR::one}; 
        lrtData ltrdata {};
        embot::core::Data dd {};
        constexpr iccInfo(embot::hw::icc::LTR l) : ltr(l), dd(&ltrdata, sizeof(lrtData)) {}
    };
    
    static constexpr embot::hw::icc::LTR ltrA {embot::hw::icc::LTR::one};
    static constexpr embot::hw::icc::LTR ltrB {embot::hw::icc::LTR::two};
    
    
    void todo();
    
// what the Impl needs:
// 
    
    embot::os::EventThread *tRX {nullptr};
    static void tRXstartup(embot::os::Thread *t, void *p);
    static void tRXonevent(embot::os::Thread *t, os::EventMask m, void *p);    
    
    embot::os::EventThread *tTX {nullptr};
    static void tTXstartup(embot::os::Thread *t, void *p);
    static void tTXonevent(embot::os::Thread *t, os::EventMask m, void *p);
    
  
    struct ChannelImpl
    {        
        struct Config
        { 
            static constexpr size_t maxcapacity {theICCservice::PipeConfig::maxcapacity}; // {40};   
            
            Pipe pipe {Pipe::one};
            Mode mode {Mode::master};
            embot::hw::icc::LTR rxltr {embot::hw::icc::LTR::one};
            embot::hw::icc::LTR txltr {embot::hw::icc::LTR::two};
            size_t rxcapacity {32}; 
            size_t txcapacity {32};
            modeTX modetx {modeTX::instant}; // ?   
            embot::os::Event eventTX {embot::os::bitpos2event(1)};
            embot::os::Event eventRX {embot::os::bitpos2event(2)};
            embot::core::Callback onrx {};
            ItemParser itemparser {nullptr};
            
            constexpr Config() = default;
            
            constexpr Config(   Pipe p, Mode mo, embot::hw::icc::LTR rl, embot::hw::icc::LTR tl,
                                size_t r, size_t t, modeTX mt, const embot::core::Callback &orx, ItemParser ip, 
                                embot::os::Event et, embot::os::Event er)
                            :   pipe(p), mode (mo), rxltr(rl), txltr(tl),
                                rxcapacity(std::min(r, maxcapacity)), txcapacity(std::min(t, maxcapacity)),
                                modetx(mt), 
                                onrx(orx),
                                itemparser(ip),
                                eventTX(et), eventRX(er)
                            {}
                                
        }; 

        Config _config {};  

        Impl *pimpl {nullptr};        

        bool initialise(const Config &c, Impl *i)
        {
            bool r {true}; 
            
            pimpl = i;
            _config = c;
            
            
            _iccRX.ltr = _config.rxltr;    
            _iccTX.ltr = _config.txltr;


            // we now check if the two letters have enough capacity to hold our data
            
            size_t rxs = embot::hw::icc::ltr::size(_iccRX.ltr);
            size_t txs = embot::hw::icc::ltr::size(_iccTX.ltr);
            
            if(embot::hw::icc::ltr::size(_iccRX.ltr) < sizeof(lrtData))
            {
                embot::core::print("embot::app::eth::icc::theICCservice::Impl::initialise(): the RX ltr cannot hold the Config::maxcapacity");
                return false;
            }
            
            if(embot::hw::icc::ltr::size(_iccTX.ltr) < sizeof(lrtData))
            {
                embot::core::print("embot::app::eth::icc::theICCservice::Impl::initialise(): the TX ltr cannot hold the Config::maxcapacity");
                return false;
            }
            
            _TX_fifo.reserve(_config.txcapacity);
            _TX_fifo.clear();
            _TX_mtx = embot::os::rtos::mutex_new();
            
            _RX_fifo.reserve(_config.rxcapacity);
            _RX_fifo.clear();
            _RX_mtx = embot::os::rtos::mutex_new();
            
            
            itemparser = (nullptr != _config.itemparser) ? (_config.itemparser) : (defaultparser);           
            
            return r;
        }    

        // - used by rx thread 
        // the RX fifo and its mutex. it is used by the RX thread and by the other user threads calling ::input() / ::get() / etc.    
        std::vector<Item> _RX_fifo {};
        embot::os::rtos::mutex_t *_RX_mtx {nullptr};    
        // the rx icc letter
        iccInfo _iccRX {embot::hw::icc::LTR::three};

        static void rxICCcbk(void *p) { 
            ChannelImpl *chnl = reinterpret_cast<ChannelImpl*>(p);
            chnl->pimpl->tRX->setEvent(chnl->_config.eventRX);
        }
        
        static bool defaultparser(const Item &item) { return true; };

        // this startup is generic for each channel. it must be called inside the startup of the RX thread inside Impl that manages the RX 
        static void tRXstartup(embot::os::Thread *t, void *p)
        {
            ChannelImpl *chnl = reinterpret_cast<ChannelImpl*>(p);
            volatile embot::hw::result_t r {embot::hw::resNOK};
            r = embot::hw::icc::ltr::init(chnl->_iccRX.ltr, {embot::hw::icc::DIR::rx});
            bool b = embot::hw::icc::ltr::subscribe(chnl->_iccRX.ltr, {{rxICCcbk, chnl}, embot::hw::Subscription::MODE::permanent});
            r = r;
            b = b;
    
        }
        
        // this onevent is generic one
        static void tRXonevent(embot::os::Thread *t, os::EventMask eventmask, void *p)
        {
            static uint32_t prog {666};;
            if(0 == eventmask)
            {   // timeout ...       
                return;
            }

            ChannelImpl *chnl = reinterpret_cast<ChannelImpl*>(p);   
            
            if(true == embot::core::binary::mask::check(eventmask, chnl->_config.eventRX))
            {
                chnl->_iccRX.ltrdata.clear();
                
                embot::hw::icc::ltr::read(chnl->_iccRX.ltr, chnl->_iccRX.dd);
                
                // now i use _iccRX.dd, or rather i use _iccRX.ltrdata
                // take mtx
                // copy _iccRX.ltrdata
                // release mtx
                size_t num = std::min(chnl->_iccRX.ltrdata.size, chnl->_iccRX.ltrdata.items.size());
                
                embot::os::rtos::mutex_take(chnl->_RX_mtx, embot::core::reltimeWaitForever);
                for(size_t i=0; i<num; i++)
                {
                    Item item = chnl->_iccRX.ltrdata.items[i];

                    if(chnl->_RX_fifo.size() >= chnl->_config.txcapacity)
                    {
                        chnl->_RX_fifo.erase(chnl->_RX_fifo.begin());
                    }
                    chnl->_RX_fifo.push_back(item);
                }
                
                size_t numofRX = chnl->_RX_fifo.size();
                
                embot::os::rtos::mutex_release(chnl->_RX_mtx); 
                // question: shall we put it out of the mutex? to be ok it should be called
                // in a protected way using a dedicated mutex that is also used when ...
                // we change _config.onrx
                // the same applies for the 
                
                if(numofRX > 0)
                {
                    chnl->_config.onrx.execute();  
                }            
            }            
        }
        
        
        ItemParser itemparser {defaultparser};
        
        // - used by tx thread 
        
        // the tx icc letter
        iccInfo _iccTX {embot::hw::icc::LTR::four};
        // the TX fifo and its mutex. it is used by the TX thread and by the other user threads calling ::output() / ::put() / etc. 
        std::vector<Item> _TX_fifo {};
        embot::os::rtos::mutex_t *_TX_mtx {nullptr};    
        volatile bool _TXacked {false};
        volatile bool _flushed {false};
        embot::os::rtos::semaphore_t *txsemaphore {nullptr};
        embot::core::Callback cbkOnTXdone {}; 
        static void donothing(void *p) {}
        constexpr static embot::core::Callback cbkDoNothing {donothing, nullptr};                            
        static void tTXstartup(embot::os::Thread *t, void *p)
        {
            ChannelImpl *chnl = reinterpret_cast<ChannelImpl*>(p);
            
            volatile embot::hw::result_t r {embot::hw::resNOK};
            r = embot::hw::icc::ltr::init(chnl->_iccTX.ltr, {embot::hw::icc::DIR::tx});
            r = r;
            chnl->txsemaphore = embot::os::rtos::semaphore_new(2, 0);
            chnl->cbkOnTXdone.clear();
            chnl->_flushed = chnl->_TXacked = false;            
        }
        
        static void tTXonevent(embot::os::Thread *t, os::EventMask eventmask, void *p)
        {
            ChannelImpl *chnl = reinterpret_cast<ChannelImpl*>(p);

            if(true == embot::core::binary::mask::check(eventmask, chnl->_config.eventTX))
            {
                // note: i cannot flush more items than what the max size of the letter
                // so, we just ... lose some if we have a txfifo w/ capacity larger than max size of the
                // letter ... but we checkd about that in the initialise() ....
                       
                chnl->_iccTX.ltrdata.clear();
                
                embot::os::rtos::mutex_take(chnl->_TX_mtx, embot::core::reltimeWaitForever);
                
                // we checked that in the initialise(), so this min() is redundant. important but useless in here
                size_t num = std::min(chnl->_TX_fifo.size(), chnl->_iccTX.ltrdata.items.size());
                        
                chnl->_iccTX.ltrdata.size = num;
                
                for(size_t i=0; i<num; i++)
                {
                    chnl->_iccTX.ltrdata.items[i] = chnl->_TX_fifo[i];          
                }
                        
                chnl->_TX_fifo.clear();
                
                embot::os::rtos::mutex_release(chnl->_TX_mtx);  

                chnl->_flushed = chnl->_TXacked = false;
                embot::core::Callback cbk {txackcbk, chnl};
                embot::hw::icc::ltr::post(chnl->_iccTX.ltr, chnl->_iccTX.dd, cbk);     
            }                       
        }            
            
        static void txackcbk(void *p)
        {
            ChannelImpl *chnl = reinterpret_cast<ChannelImpl*>(p);
            
            chnl->_flushed = chnl->_TXacked = embot::hw::icc::ltr::acked(chnl->_iccTX.ltr, true);
            
            if(true == chnl->cbkOnTXdone.isvalid())
            {
                chnl->cbkOnTXdone.execute();
            }
            else
            {
                embot::os::rtos::semaphore_release(chnl->txsemaphore);
            }           
        }
                
    };  // struct ChannelImpl
    
    static constexpr size_t maxchannels {2};
    std::array<ChannelImpl, maxchannels> _channels {};        
    static constexpr size_t nchannels2use {2};
        
    ChannelImpl * get(Pipe p)
    {
        return &_channels[embot::core::tointegral(p)];
    }      

    const ChannelImpl * getconst(Pipe p) const
    {
        return const_cast<const ChannelImpl *>(&_channels[embot::core::tointegral(p)]);  
    }        
          
};


bool embot::app::eth::icc::theICCservice::Impl::initialise(const Config &config)
{
    if(true == _initted)
    {
        return true;
    }
        
    // embot::core::print("embot::app::eth::icc::theICCservice::Impl::initialise()");
     
    // ok, do something
    _config = config;
       
    // init the channels
    for(size_t c=0; c<nchannels2use; c++)
    {
        ChannelImpl::Config cfg
        {
            static_cast<Pipe>(c), config.mode, 
            config.pipeconfig[c].rxltr, config.pipeconfig[c].txltr, config.pipeconfig[c].rxcapacity, config.pipeconfig[c].txcapacity, 
            config.pipeconfig[c].modetx, config.pipeconfig[c].onrx, config.pipeconfig[c].itemparser,
            embot::os::bitpos2event(2*c+1), embot::os::bitpos2event(2*c+2)                
        };
        
        _channels[c].initialise(cfg, this);
    }
        
    constexpr embot::core::relTime timeout {1000*embot::core::time1millisec};
    
    // RX thread
    embot::os::EventThread::Config rxCfg { 
        4096, //_config.thread.stacksize, 
        _config.rxpriority, // _config.thread.priority, 
        tRXstartup, this,
        timeout,
        tRXonevent,
        "tRXicc"
    };
       
    tRX = new embot::os::EventThread;          
    tRX->start(rxCfg, tRXicc);    

    // TX thread
    embot::os::EventThread::Config txCfg { 
        4096, //_config.thread.stacksize, 
        _config.txpriority, // _config.thread.priority, 
        tTXstartup, this,
        timeout,
        tTXonevent,
        "tTXicc"
    };
       
        
    // create the RX thread 
    tTX = new embot::os::EventThread;          
    // and start it
    tTX->start(txCfg, tTXicc);     
            
    _initted = true;
    return true;
}

bool embot::app::eth::icc::theICCservice::Impl::set(Pipe pipe, modeTX txm)
{
    if(false == _initted)
    {
        return false;
    }
    
    ChannelImpl *chnl = get(pipe);  

    chnl->_config.modetx = txm;  
    return true;
   
}

bool embot::app::eth::icc::theICCservice::Impl::set(Pipe pipe, const embot::core::Callback &onr)
{
    if(false == _initted)
    {
        return false;
    }  


    ChannelImpl *chnl = get(pipe);
    
    chnl->_config.onrx = onr;  
    return true;      
}

bool embot::app::eth::icc::theICCservice::Impl::set(Pipe pipe, ItemParser itmp)
{
    if(false == _initted)
    {
        return false;
    } 
    

    ChannelImpl *chnl = get(pipe);  

    chnl->itemparser = (nullptr != itmp) ? (itmp) : (ChannelImpl::defaultparser);
    return true;   
}


bool embot::app::eth::icc::theICCservice::Impl::put(Pipe pipe, const Item &item, embot::core::relTime timeout)
{ 
    if(false == _initted)
    {
        return false;
    }
    
    ChannelImpl *chnl = get(pipe); 
    
    // add in a buffer. do not trigger any tx yet
    bool r = embot::os::rtos::mutex_take(chnl->_TX_mtx, timeout);
    if(false == r)
    {
        return false;
    }
    
    if(chnl->_TX_fifo.size() >= chnl->_config.txcapacity)
    {
        chnl->_TX_fifo.erase(chnl->_TX_fifo.begin());
    }
    chnl->_TX_fifo.push_back(item);
    
    embot::os::rtos::mutex_release(chnl->_TX_mtx);  

    if(modeTX::instant == chnl->_config.modetx)
    {
        return flush(pipe, timeout);
    }
    
    return true;    
}

size_t embot::app::eth::icc::theICCservice::Impl::output(Pipe pipe) const
{
    if(false == _initted)
    {
        return 0;
    }
    
    size_t s {0};
    

    const ChannelImpl *chnl = getconst(pipe); 
    
    embot::os::rtos::mutex_take(chnl->_TX_mtx, embot::core::reltimeWaitForever);
    s = chnl->_TX_fifo.size();
    embot::os::rtos::mutex_release(chnl->_TX_mtx);    
    
    return s;
}

bool embot::app::eth::icc::theICCservice::Impl::flush(Pipe pipe, const embot::core::Callback &cbk)
{
    bool r {false};

    if(false == _initted)
    {
        return false;
    }
     
    ChannelImpl *chnl = get(pipe); 
    
    // we see if we have any to transmit    
    if(0 == output(pipe))
    {
        chnl->_flushed = true;
        r = true;
        cbk.execute();
        return r;
    }
    
    // but what if ... tyen tx thread has not received any ack yet becaus ethe otehr core is not responding?
    // think of it 
    chnl->cbkOnTXdone = (true == cbk.isvalid()) ? cbk : ChannelImpl::cbkDoNothing;
        
    tTX->setEvent(chnl->_config.eventTX);    

    return chnl->_flushed;     
  
}

bool embot::app::eth::icc::theICCservice::Impl::flush(Pipe pipe, embot::core::relTime timeout)
{
    bool r {false};

    if(false == _initted)
    {
        return false;
    }
      
    ChannelImpl *chnl = get(pipe); 

    // we see if we have any to transmit    
    if(0 == output(pipe))
    {
        chnl->_flushed = true;
        r = true;
        return r;
    }
    
    // what if the tx thread does not received any ack because the other core is stopped or is not responding?
    // the txThread just calls embot::hw::icc::ltr::post() and terminates its tick. however its 
    // callback is never called, the semaphore is never released and ... this function just returns 
    // false after the timeout
        
    chnl->cbkOnTXdone.clear();
    tTX->setEvent(chnl->_config.eventTX);    
    r = embot::os::rtos::semaphore_acquire(chnl->txsemaphore, timeout); 
    r = chnl->_flushed;    
    return r;     
  
}

bool embot::app::eth::icc::theICCservice::Impl::flushed(Pipe pipe) const
{
    if(false == _initted)
    {
        return false;
    } 
    
    const ChannelImpl *chnl = getconst(pipe);     
    return chnl->_flushed;   
}

size_t embot::app::eth::icc::theICCservice::Impl::input(Pipe pipe) const
{
    if(false == _initted)
    {
        return 0;
    }
    
    size_t s {0};
     
    const ChannelImpl *chnl = getconst(pipe); 
    
    embot::os::rtos::mutex_take(chnl->_RX_mtx, embot::core::reltimeWaitForever);
    s = chnl->_RX_fifo.size();
    embot::os::rtos::mutex_release(chnl->_RX_mtx);
    
    return s;
}

bool embot::app::eth::icc::theICCservice::Impl::get(Pipe pipe, Item &item, size_t &remaining)
{
    if(false == _initted)
    {
        return false;
    }  
 
    ChannelImpl *chnl = get(pipe); 

    bool r {false};
    Item rxi {};
    size_t s {0};
    
    embot::os::rtos::mutex_take(chnl->_RX_mtx, embot::core::reltimeWaitForever);
    s = chnl->_RX_fifo.size();
    if(s > 0)
    {
        rxi = chnl->_RX_fifo.front();
        chnl->_RX_fifo.erase(chnl->_RX_fifo.begin());
        remaining = chnl->_RX_fifo.size();
        r = true;
    }
    embot::os::rtos::mutex_release(chnl->_RX_mtx);     
        
    item = rxi;
        
    return r;    
}

bool embot::app::eth::icc::theICCservice::Impl::parse(Pipe pipe)
{
    if(false == _initted)
    {
        return false;
    }  

    ChannelImpl *chnl = get(pipe); 

    bool r {false};
    Item rxi {};
    size_t s {0};
    
    s = input(pipe);
        
    for(size_t i=0; i<s; i++)
    {
        size_t remaining {0};
        if(true == get(pipe, rxi, remaining))
        {
            chnl->itemparser(rxi);
            r = true;
        }
    }    
    
    return r;

}



// private members

void embot::app::eth::icc::theICCservice::Impl::todo()
{

}


void embot::app::eth::icc::theICCservice::Impl::tRXstartup(embot::os::Thread *t, void *p)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    
    for(size_t c=0; c<nchannels2use; c++)
    {
        impl->_channels[c].tRXstartup(t, &impl->_channels[c]);
    }    
    
}


void embot::app::eth::icc::theICCservice::Impl::tRXonevent(embot::os::Thread *t, os::EventMask eventmask, void *p)
{
    if(0 == eventmask)
    {   // timeout ...       
        return;
    }
    
    Impl *impl = reinterpret_cast<Impl*>(p);
    
    for(size_t c=0; c<nchannels2use; c++)
    {
        impl->_channels[c].tRXonevent(t, eventmask, &impl->_channels[c]);
    }   
    
}


void embot::app::eth::icc::theICCservice::Impl::tTXstartup(embot::os::Thread *t, void *p)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    
    for(size_t c=0; c<nchannels2use; c++)
    {
        impl->_channels[c].tTXstartup(t, &impl->_channels[c]);
    } 
}


void embot::app::eth::icc::theICCservice::Impl::tTXonevent(embot::os::Thread *t, os::EventMask eventmask, void *p)
{
    if(0 == eventmask)
    {   // timeout ...       
        return;
    }
    
    Impl *impl = reinterpret_cast<Impl*>(p);
    
    for(size_t c=0; c<nchannels2use; c++)
    {
        impl->_channels[c].tTXonevent(t, eventmask, &impl->_channels[c]);
    }       
}

// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::eth::icc::theICCservice& embot::app::eth::icc::theICCservice::getInstance()
{
    static theICCservice* p = new theICCservice();
    return *p;
}

embot::app::eth::icc::theICCservice::theICCservice()
{
    pImpl = std::make_unique<Impl>();
}

embot::app::eth::icc::theICCservice::~theICCservice() { }

bool embot::app::eth::icc::theICCservice::initialise(const Config &config)
{
    return pImpl->initialise(config);
}

bool embot::app::eth::icc::theICCservice::set(Pipe pipe, modeTX txm)
{
    return pImpl->set(pipe, txm);
}

bool embot::app::eth::icc::theICCservice::set(Pipe pipe, const embot::core::Callback &onr)
{
    return pImpl->set(pipe, onr);
}

bool embot::app::eth::icc::theICCservice::set(Pipe pipe, ItemParser itmp)
{
    return pImpl->set(pipe, itmp);
}


bool embot::app::eth::icc::theICCservice::put(Pipe pipe, const Item &item, embot::core::relTime timeout)
{
    return pImpl->put(pipe, item, timeout); 
}

size_t embot::app::eth::icc::theICCservice::output(Pipe pipe) const
{
    return pImpl->output(pipe);
}

bool embot::app::eth::icc::theICCservice::flush(Pipe pipe, const embot::core::Callback &cbk)
{
    return pImpl->flush(pipe, cbk);
}

bool embot::app::eth::icc::theICCservice::flush(Pipe pipe, embot::core::relTime timeout)
{
    return pImpl->flush(pipe, timeout);
}

bool embot::app::eth::icc::theICCservice::flushed(Pipe pipe) const
{
    return pImpl->flushed(pipe);    
}

size_t embot::app::eth::icc::theICCservice::input(Pipe pipe) const
{
    return pImpl->input(pipe);
}

bool embot::app::eth::icc::theICCservice::get(Pipe pipe, Item &item, size_t &remaining)
{
    return pImpl->get(pipe, item, remaining); 
}

bool embot::app::eth::icc::theICCservice::parse(Pipe pipe)
{
    return pImpl->parse(pipe);
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


