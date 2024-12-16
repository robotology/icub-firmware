
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
    
struct embot::app::eth::theICCservice::Impl
{
    bool _initted {false};
    Config _config {};
    
    constexpr static const char _objname[] = "theICCservice";
    
    Impl() = default;
    
    bool initialise(const Config &config);
        
    bool set(modeTX txm);
    bool set(const embot::core::Callback &onr);
    bool set(ItemParser itmp);
    
    bool put(const Item &item, embot::core::relTime timeout);   
    size_t output() const;
    bool flush(const embot::core::Callback &cbk);
    bool flush(embot::core::relTime timeout);
    bool flushed() const;
    
    size_t input() const;
    bool get(Item &item, size_t &remaining); 
    bool parse();        

    bool ask(uint8_t id, uint8_t size, void *reply, embot::core::relTime timeout);
    bool say(uint8_t id, uint8_t size, void *value);
    bool set(uint8_t id, uint8_t size, void *value, embot::core::relTime timeout);
    bool ack(uint8_t id);
        
private:    
    
    // we exchange this data structure
    struct lrtData
    {
        size_t size {0};
        std::array<Item, Config::maxcapacity> items {};
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
    
//    static_assert(644 == sizeof(lrtData), "lrtData is too big to fit in LTR::one");
//    
//    static_assert(20 == sizeof(Item) , "lrtData is too big to fit in LTR::one");
//    
//    static constexpr embot::hw::icc::MEM m = embot::hw::icc::ltr::bsp::getBSP().getPROP(ltrA)->mem;
    
//    static_assert(embot::hw::icc::ltr::size(embot::hw::icc::LTR::one) > 1, "not 1024");
    
    void todo();
    
    static bool defaultparser(const Item &item) { return true; };
    
    ItemParser itemparser {defaultparser};

// what the Impl needs:
// 

    std::vector<Item> _TX_fifo {};
    embot::os::rtos::mutex_t *_TX_mtx {nullptr};
    
    std::vector<Item> _RX_fifo {};
    embot::os::rtos::mutex_t *_RX_mtx {nullptr};
    
    embot::os::EventThread *tRX {nullptr};
    static constexpr embot::os::Event evtRX = 1;
    // static void rxcbk(void *p) { Impl *i = reinterpret_cast<Impl*>(p); i->tRX->setEvent(evRX); }
    static void rxICCcbk(void *p) { 
        reinterpret_cast<Impl*>(p)->tRX->setEvent(evtRX); 
    }
    
    static void tRXstartup(embot::os::Thread *t, void *p);
    static void tRXonevent(embot::os::Thread *t, os::EventMask m, void *p);
    
    iccInfo _iccRX {embot::hw::icc::LTR::one};
    iccInfo _iccTX {embot::hw::icc::LTR::two};
    
    embot::os::EventThread *tTX {nullptr};
    static constexpr embot::os::Event evtTX = 2;
    volatile bool _TXacked {false};
    volatile bool _flushed {false}; // as _TXacked but true also when we doid not need to tx anything
    embot::os::rtos::semaphore_t *txsemaphore {nullptr};
    embot::core::Callback cbkOnTXdone {}; 
            
        
    static void tTXstartup(embot::os::Thread *t, void *p);
    static void tTXonevent(embot::os::Thread *t, os::EventMask m, void *p);

    static void donothing(void *p) {}
    constexpr static embot::core::Callback cbkDoNothing {donothing, nullptr};
        
    static void txackcbk(void *p)
    {
        Impl *impl = reinterpret_cast<Impl*>(p);
        
        impl->_flushed = impl->_TXacked = embot::hw::icc::ltr::acked(impl->_iccTX.ltr, true);
        
        if(true == impl->cbkOnTXdone.isvalid())
        {
            impl->cbkOnTXdone.execute();
        }
        else
        {
            embot::os::rtos::semaphore_release(impl->txsemaphore);
        }
        
    }
};


bool embot::app::eth::theICCservice::Impl::initialise(const Config &config)
{
    if(true == _initted)
    {
        return true;
    }
        
    // embot::core::print("embot::app::eth::theICCservice::Impl::initialise()");
     
    // ok, do something
    _config = config;
    

    _iccRX.ltr = _config.rxltr;    
    _iccTX.ltr = _config.txltr;

//    // this object must be used by the two cores (A an B)
//    // the LRT that TX in one core must be the same that receives in the other one 
//    switch(_config.direction)
//    {
//        default:
//        case Direction::A2B:
//        {
//            _iccRX.ltr = embot::hw::icc::LTR::one;
//            _iccTX.ltr = embot::hw::icc::LTR::two;
//            
//        } break;
//        
//        case Direction::B2A:
//        {
//            _iccRX.ltr = embot::hw::icc::LTR::two;
//            _iccTX.ltr = embot::hw::icc::LTR::one;            
//        } break;        
//    }    

    // we now check if the two letters have enough capacity to hold our data
    
    size_t rxs = embot::hw::icc::ltr::size(_iccRX.ltr);
    size_t txs = embot::hw::icc::ltr::size(_iccTX.ltr);
    
    if(embot::hw::icc::ltr::size(_iccRX.ltr) < sizeof(lrtData))
    {
        embot::core::print("embot::app::eth::theICCservice::Impl::initialise(): the RX ltr cannot hold the Config::maxcapacity");
        return false;
    }
    
    if(embot::hw::icc::ltr::size(_iccTX.ltr) < sizeof(lrtData))
    {
        embot::core::print("embot::app::eth::theICCservice::Impl::initialise(): the TX ltr cannot hold the Config::maxcapacity");
        return false;
    }
    
    _TX_fifo.reserve(_config.txcapacity);
    _TX_fifo.clear();
    _TX_mtx = embot::os::rtos::mutex_new();
    
    _RX_fifo.reserve(_config.rxcapacity);
    _RX_fifo.clear();
    _RX_mtx = embot::os::rtos::mutex_new();
    
    
    itemparser = (nullptr != _config.itemparser) ? (_config.itemparser) : (defaultparser);
    
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
        _config.rxpriority, // _config.thread.priority, 
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

bool embot::app::eth::theICCservice::Impl::set(modeTX txm)
{
    if(false == _initted)
    {
        return false;
    }
    
    _config.modetx = txm;
    return true;
}

bool embot::app::eth::theICCservice::Impl::set(const embot::core::Callback &onr)
{
    if(false == _initted)
    {
        return false;
    }  
    
    _config.onrx = onr;
    return true;    
}

bool embot::app::eth::theICCservice::Impl::set(ItemParser itmp)
{
    if(false == _initted)
    {
        return false;
    } 
    
    itemparser = (nullptr != itmp) ? (itmp) : (defaultparser);
    return true;    
}


bool embot::app::eth::theICCservice::Impl::put(const Item &item, embot::core::relTime timeout)
{ 
    if(false == _initted)
    {
        return false;
    }
    
    // add in a buffer. do not trigger any tx yet
    bool r = embot::os::rtos::mutex_take(_TX_mtx, timeout);
    if(false == r)
    {
        return false;
    }
    
    if(_TX_fifo.size() >= _config.txcapacity)
    {
        _TX_fifo.erase(_TX_fifo.begin());
    }
    _TX_fifo.push_back(item);
    
    embot::os::rtos::mutex_release(_TX_mtx);  

    if(modeTX::instant == _config.modetx)
    {
        return flush(timeout);
    }
    
    return true;  
}

size_t embot::app::eth::theICCservice::Impl::output() const
{
    if(false == _initted)
    {
        return 0;
    }
    
    size_t s {0};
    
    embot::os::rtos::mutex_take(_TX_mtx, embot::core::reltimeWaitForever);
    s = _TX_fifo.size();
    embot::os::rtos::mutex_release(_TX_mtx);
    
    return s;
}

bool embot::app::eth::theICCservice::Impl::flush(const embot::core::Callback &cbk)
{
    bool r {false};

    if(false == _initted)
    {
        return false;
    }
    
    // we see if we have any to transmit    
    if(0 == output())
    {
        _flushed = true;
        r = true;
        cbk.execute();
        return r;
    }
    
    // but what if ... tyen tx thread has not received any ack yet becaus ethe otehr core is not responding?
    // think of it 
    cbkOnTXdone = (true == cbk.isvalid()) ? cbk : cbkDoNothing;
        
    tTX->setEvent(evtTX);    

    return _flushed; 
}

bool embot::app::eth::theICCservice::Impl::flush(embot::core::relTime timeout)
{
    bool r {false};

    if(false == _initted)
    {
        return false;
    }
    
    // we see if we have any to transmit    
    if(0 == output())
    {
        _flushed = true;
        r = true;
        return r;
    }
    
    // what if the tx thread does not received any ack because the other core is stopped or is not responding?
    // the txThread just calls embot::hw::icc::ltr::post() and terminates its tick. however its 
    // callback is never called, the semaphore is never released and ... this function just returns 
    // false after the timeout
        
    cbkOnTXdone.clear();
    tTX->setEvent(evtTX);    
    r = embot::os::rtos::semaphore_acquire(txsemaphore, timeout); 
    r = _flushed;    
    return r; 
}

bool embot::app::eth::theICCservice::Impl::flushed() const
{
    if(false == _initted)
    {
        return false;
    }    
    
    return _flushed;
}

size_t embot::app::eth::theICCservice::Impl::input() const
{
    if(false == _initted)
    {
        return 0;
    }
    
    size_t s {0};
    
    embot::os::rtos::mutex_take(_RX_mtx, embot::core::reltimeWaitForever);
    s = _RX_fifo.size();
    embot::os::rtos::mutex_release(_RX_mtx);
    
    return s;
}

bool embot::app::eth::theICCservice::Impl::get(Item &item, size_t &remaining)
{
    if(false == _initted)
    {
        return false;
    }    
    
    bool r {false};
    Item rxi {};
    size_t s {0};
    
    embot::os::rtos::mutex_take(_RX_mtx, embot::core::reltimeWaitForever);
    s = _RX_fifo.size();
    if(s > 0)
    {
        rxi = _RX_fifo.front();
        _RX_fifo.erase(_RX_fifo.begin());
        remaining = _RX_fifo.size();
        r = true;
    }
    embot::os::rtos::mutex_release(_RX_mtx);     
        
    item = rxi;
        
    return r; 
}

bool embot::app::eth::theICCservice::Impl::parse()
{
    if(false == _initted)
    {
        return false;
    }    
    
#if 0
    bool r {false};
    Item rxi {};
    size_t s {0};
    
    embot::os::rtos::mutex_take(_RX_mtx, embot::core::reltimeWaitForever);
    s = _RX_fifo.size();
    for(size_t i=0; i<s; i++)
    {
        rxi = _RX_fifo[i];
        itemparser(rxi);
    }
    _RX_fifo.clear();
    embot::os::rtos::mutex_release(_RX_mtx);     
        
    r = true;
    return r;     
#else
    // the above is ok but keeps the rx fifo locked for too long, so the ack towards the other core is delayed. 
    // the quicker response is if we extract items one by one because we keep the rx queue available during parsing
    // on the other hand, this solution takes and releases a mutex and operations over svc ... take time
    
    bool r {false};
    Item rxi {};
    size_t s {0};
    
    s = input();
    
    
    for(size_t i=0; i<s; i++)
    {
        size_t remaining {0};
        if(true == get(rxi, remaining))
        {
            itemparser(rxi);
            r = true;
        }
    }    
    
    return r;
    
#endif
}


bool embot::app::eth::theICCservice::Impl::ask(uint8_t id, uint8_t size, void *reply, embot::core::relTime timeout)
{
    return true;
}

bool embot::app::eth::theICCservice::Impl::say(uint8_t id, uint8_t size, void *value)
{
    return true;    
}

bool embot::app::eth::theICCservice::Impl::set(uint8_t id, uint8_t size, void *value, embot::core::relTime timeout)
{
    return true;
}

bool embot::app::eth::theICCservice::Impl::ack(uint8_t id)
{
    return true;    
}



// private members

void embot::app::eth::theICCservice::Impl::todo()
{

}


void embot::app::eth::theICCservice::Impl::tRXstartup(embot::os::Thread *t, void *p)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    volatile embot::hw::result_t r {embot::hw::resNOK};
    r = embot::hw::icc::ltr::init(impl->_iccRX.ltr, {embot::hw::icc::DIR::rx});
    bool b = embot::hw::icc::ltr::subscribe(impl->_iccRX.ltr, {{rxICCcbk, impl}, embot::hw::Subscription::MODE::permanent});
    r = r;
    b = b;
}


void embot::app::eth::theICCservice::Impl::tRXonevent(embot::os::Thread *t, os::EventMask eventmask, void *p)
{
    if(0 == eventmask)
    {   // timeout ...       
        return;
    }
    
    Impl *impl = reinterpret_cast<Impl*>(p);
    
    if(true == embot::core::binary::mask::check(eventmask, evtRX))
    {
        impl->_iccRX.ltrdata.clear();
        
        embot::hw::icc::ltr::read(impl->_iccRX.ltr, impl->_iccRX.dd);
        
        // now i use _iccRX.dd, or rather i use _iccRX.ltrdata
        // take mtx
        // copy _iccRX.ltrdata
        // release mtx
        size_t num = std::min(impl->_iccRX.ltrdata.size, impl->_iccRX.ltrdata.items.size());
        
        embot::os::rtos::mutex_take(impl->_RX_mtx, embot::core::reltimeWaitForever);
        for(size_t i=0; i<num; i++)
        {
            Item item = impl->_iccRX.ltrdata.items[i];
            if(impl->_RX_fifo.size() >= impl->_config.txcapacity)
            {
                impl->_RX_fifo.erase(impl->_RX_fifo.begin());
            }
            impl->_RX_fifo.push_back(item);
        }
        embot::os::rtos::mutex_release(impl->_RX_mtx); 
        // question: shall we put it out of the mutex? to be ok it should be called
        // in a protected way using a dedicated mutex that is also used when ...
        // we change _config.onrx
        // the same applies for the 
        impl->_config.onrx.execute();        
    }
    
}



void embot::app::eth::theICCservice::Impl::tTXstartup(embot::os::Thread *t, void *p)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    
    volatile embot::hw::result_t r {embot::hw::resNOK};
    r = embot::hw::icc::ltr::init(impl->_iccTX.ltr, {embot::hw::icc::DIR::tx});
    r = r;
    impl->txsemaphore = embot::os::rtos::semaphore_new(2, 0);
    impl->cbkOnTXdone.clear();
    impl->_flushed = impl->_TXacked = false;
}


void embot::app::eth::theICCservice::Impl::tTXonevent(embot::os::Thread *t, os::EventMask eventmask, void *p)
{
    if(0 == eventmask)
    {   // timeout ...       
        return;
    }
    
    Impl *impl = reinterpret_cast<Impl*>(p);
    
    if(true == embot::core::binary::mask::check(eventmask, evtTX))
    {
        // note: i cannot flush more items than what the max size of the letter
        // so, we just ... lose some if we have a txfifo w/ capacity larger than max size of the
        // letter ... but we checkd about that in the initialise() ....
               
        impl->_iccTX.ltrdata.clear();
        
        embot::os::rtos::mutex_take(impl->_TX_mtx, embot::core::reltimeWaitForever);
        
        // we checked that in the initialise(), so this min() is redundant. important but useless in here
        size_t num = std::min(impl->_TX_fifo.size(), impl->_iccTX.ltrdata.items.size());
                
        impl->_iccTX.ltrdata.size = num;
        
        for(size_t i=0; i<num; i++)
        {
            impl->_iccTX.ltrdata.items[i] = impl->_TX_fifo[i];          
        }
                
        impl->_TX_fifo.clear();
        
        embot::os::rtos::mutex_release(impl->_TX_mtx);  
#if 0
        // and now we send over the ICC
        embot::hw::icc::ltr::post(impl->_iccTX.ltr, impl->_iccTX.dd, embot::core::time1millisec);
        impl->_TXacked = embot::hw::icc::ltr::acked(impl->_iccTX.ltr, true);
#else
        // embot::hw::icc::ltr::post(impl->_iccTX.ltr, impl->_iccTX.dd, embot::core::time1millisec);
        // impl->txsemaphore
        
//        mettere qualcosa tipo:
//        il flush() chiama il sem.get() after it sends the tx event e si blocca con timeout
//        il thread chiama la ltr con uan callbacl che unlocka il semaforo. e poi esce.
//        in questo modo nessun thread esegue
        impl->_flushed = impl->_TXacked = false;
        embot::core::Callback cbk {txackcbk, p};
        embot::hw::icc::ltr::post(impl->_iccTX.ltr, impl->_iccTX.dd, cbk);
#endif        
    }
    
}

// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::eth::theICCservice& embot::app::eth::theICCservice::getInstance()
{
    static theICCservice* p = new theICCservice();
    return *p;
}

embot::app::eth::theICCservice::theICCservice()
{
    pImpl = std::make_unique<Impl>();
}

embot::app::eth::theICCservice::~theICCservice() { }

bool embot::app::eth::theICCservice::initialise(const Config &config)
{
    return pImpl->initialise(config);
}

bool embot::app::eth::theICCservice::set(modeTX txm)
{
    return pImpl->set(txm);
}

bool embot::app::eth::theICCservice::set(const embot::core::Callback &onr)
{
    return pImpl->set(onr);
}

bool embot::app::eth::theICCservice::set(ItemParser itmp)
{
    return pImpl->set(itmp);
}


bool embot::app::eth::theICCservice::put(const Item &item, embot::core::relTime timeout)
{
    return pImpl->put(item, timeout); 
}

size_t embot::app::eth::theICCservice::output() const
{
    return pImpl->output();
}

bool embot::app::eth::theICCservice::flush(const embot::core::Callback &cbk)
{
    return pImpl->flush(cbk);
}

bool embot::app::eth::theICCservice::flush(embot::core::relTime timeout)
{
    return pImpl->flush(timeout);
}

bool embot::app::eth::theICCservice::flushed() const
{
    return pImpl->flushed();    
}

size_t embot::app::eth::theICCservice::input() const
{
    return pImpl->input();
}

bool embot::app::eth::theICCservice::get(Item &item, size_t &remaining)
{
    return pImpl->get(item, remaining); 
}

bool embot::app::eth::theICCservice::parse()
{
    return pImpl->parse();
}

bool embot::app::eth::theICCservice::ask(uint8_t id, uint8_t size, void *reply, embot::core::relTime timeout)
{
    return pImpl->ask(id, size, reply, timeout);
}

bool embot::app::eth::theICCservice::say(uint8_t id, uint8_t size, void *value)
{
    return pImpl->say(id, size, value);
}

bool embot::app::eth::theICCservice::set(uint8_t id, uint8_t size, void *value, embot::core::relTime timeout)
{
    return pImpl->set(id, size, value, timeout);
}

bool embot::app::eth::theICCservice::ack(uint8_t id)
{
    return pImpl->ack(id);
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


