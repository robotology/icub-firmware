
/*
 * Copyright (C) 2026 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#include "embot_svc_tether_theSTRmanager.h"
#include <cstddef>
#include <cstring>


namespace {

    // where the timestamp sits inside the frame: the only part of the header that changes at every get()
    constexpr size_t timeoffset {offsetof(embot::svc::tether::rop::FrameHeader, timeinmicroseconds)};
    static_assert(8 == timeoffset, "timeinmicroseconds must be at offset 8");

    // the time in ms on 32 bits: it wraps after 49 days, the differences w/ unsigned arithmetic stay correct
    uint32_t nowms()
    {
        return static_cast<uint32_t>(embot::core::now() / embot::core::time1millisec);
    }

}


namespace embot::svc::tether::rop {

    static_assert(std::atomic<embot::net::eth::IPaddress>::is_always_lock_free, "the host address of the stream must be lock-free");

    theSTRmanager &theSTRmanager::getInstance()
    {
        static theSTRmanager *p = new theSTRmanager();
        return *p;
    }

    bool theSTRmanager::initialise(const Config &config)
    {
        if((false == config.isvalid()) || (nullptr != _mtx))
        {
            return false;
        }
        _config = config;
        _mtx = embot::os::rtos::mutex_new();
        return nullptr != _mtx;
    }

    // ---- rpc thread

    bool theSTRmanager::add(ID id)
    {
        if(nullptr == _mtx)
        {
            return false;
        }

        const uint16_t s {_config.sharedvariables->size(id)};
        if((0 == s) || (s > maxdatasize))
        {
            return false;
        }

        embot::os::rtos::Lock lock {_mtx};
        // the budget of the frame: the header, then 4 + pad4(size) per variable, w/ the new one
        size_t bytes {sizeofframeheader + sizeofROP(s)};
        for(uint8_t i = 0; i < _list.count; i++)
        {
            if(id == _list.ids[i])
            {
                return true;
            }
            bytes += sizeofROP(_config.sharedvariables->size(_list.ids[i]));
        }
        if((_list.count >= FrameHeader::maxROPs) || (bytes > _config.maxframesize))
        {
            return false;
        }
        _list.ids[_list.count++] = id;
        _version.fetch_add(1, std::memory_order_release);
        return true;
    }

    bool theSTRmanager::rem(ID id)
    {
        if(nullptr == _mtx)
        {
            return false;
        }

        embot::os::rtos::Lock lock {_mtx};
        for(uint8_t i = 0; i < _list.count; i++)
        {
            if(id == _list.ids[i])
            {   // keep the order of the others
                std::memmove(&_list.ids[i], &_list.ids[i + 1], _list.count - i - 1);
                _list.count--;
                _list.ids[_list.count] = 0;
                _version.fetch_add(1, std::memory_order_release);
                return true;
            }
        }
        return false;
    }

    void theSTRmanager::clear()
    {
        if(nullptr == _mtx)
        {
            return;
        }
        embot::os::rtos::Lock lock {_mtx};
        _list = StreamList {};
        _version.fetch_add(1, std::memory_order_release);
    }

    StreamList theSTRmanager::list() const
    {
        if(nullptr == _mtx)
        {
            return StreamList {};
        }
        embot::os::rtos::Lock lock {_mtx};
        return _list;
    }

    bool theSTRmanager::start(uint16_t ms, const embot::net::eth::IPaddress &host)
    {
        if((nullptr == _mtx) || (0 == host.value()))
        {
            return false;
        }

        constexpr embot::core::relTime unit {embot::core::time1millisec};
        const uint16_t period {static_cast<uint16_t>((0 == ms) ? (_config.defaultperiod / unit) : ms)};
        if((period < (_config.minperiod / unit)) || (period > (_config.maxperiod / unit)))
        {
            return false;
        }

        {   // under the mutex, so that it cannot interleave w/ the stop of an expired keepalive.
            // host and time first, then the release of _periodms publishes them to the streaming thread
            embot::os::rtos::Lock lock {_mtx};
            _host.store(host, std::memory_order_relaxed);
            _lastseenms.store(nowms(), std::memory_order_relaxed);
            _periodms.store(period, std::memory_order_release);
        }
        changed();
        return true;
    }

    bool theSTRmanager::stop()
    {
        if(nullptr == _mtx)
        {
            return false;
        }
        {
            embot::os::rtos::Lock lock {_mtx};
            _periodms.store(0, std::memory_order_release);
        }
        changed();
        return true;
    }

    void theSTRmanager::changed() const
    {
        if(nullptr != _config.onchange)
        {
            _config.onchange(_config.onchangeparam);
        }
    }

    theSTRmanager::Destination theSTRmanager::destination() const
    {
        return Destination {_host.load(std::memory_order_acquire), _config.hostport};
    }

    // ---- any thread

    void theSTRmanager::touch(const embot::net::eth::IPaddress &from)
    {
        if((0 != from.value()) && running() && (from.value() == _host.load(std::memory_order_acquire).value()))
        {
            _lastseenms.store(nowms(), std::memory_order_release);
        }
    }

    bool theSTRmanager::running() const
    {
        return 0 != periodms();
    }

    uint16_t theSTRmanager::periodms() const
    {
        return _periodms.load(std::memory_order_acquire);
    }

    embot::core::relTime theSTRmanager::period() const
    {
        return periodms() * embot::core::time1millisec;
    }

    // ---- streaming thread

    bool theSTRmanager::expired()
    {
        if(0 == _config.keepalive)
        {
            return false;
        }

        const uint32_t limit {static_cast<uint32_t>(_config.keepalive / embot::core::time1millisec)};
        if((nowms() - _lastseenms.load(std::memory_order_acquire)) <= limit)
        {
            return false;
        }

        // rare path: stop under the mutex, unless a start() or a touch() has arrived meanwhile
        {
            embot::os::rtos::Lock lock {_mtx};
            if((nowms() - _lastseenms.load(std::memory_order_acquire)) <= limit)
            {
                return false;
            }
            _periodms.store(0, std::memory_order_release);
        }
        _keepalivestops.fetch_add(1, std::memory_order_relaxed);
        changed();
        return true;
    }

    void theSTRmanager::rebuild()
    {
        // a copy of the list taken under the mutex. the version is read inside the lock, so that a change happening
        // right now is either inside this copy or triggers a further rebuild at the next get()
        StreamList l {};
        uint32_t version {0};
        {
            embot::os::rtos::Lock lock {_mtx};
            l = _list;
            version = _version.load(std::memory_order_acquire);
        }

        // the ROPs one after the other, w/ their head fixed and zero data. the items point straight into the data
        // fields. add() has checked the budget, so all of them fit: a variable that does not fit is skipped
        Writer writer {_txbuffer.data(), _config.maxframesize};
        _count = 0;
        for(uint8_t i = 0; i < l.count; i++)
        {
            const ID id {l.ids[i]};
            const uint16_t s {_config.sharedvariables->size(id)};
            uint8_t *data {writer.add(Head::request(CMD::sig, OPTnone, id, s), nullptr)};
            if(nullptr != data)
            {
                _items[_count++] = Item {id, data, s};
            }
        }

        // the header: everything but the timestamp stays fixed until the next rebuild
        _txsize = writer.close(0);

        _builtversion = version;
        _built = true;
        _rebuilds.fetch_add(1, std::memory_order_relaxed);
    }

    bool theSTRmanager::get(const void *&payload, size_t &size, Destination &destination)
    {
        payload = nullptr;
        size = 0;
        destination = Destination {};

        if((nullptr == _mtx) || (false == running()) || expired())
        {
            return false;
        }

        // rare path: the list has changed since the last frame
        const uint32_t v {_version.load(std::memory_order_acquire)};
        if((false == _built) || (v != _builtversion))
        {
            rebuild();
        }

        if(0 == _count)
        {
            return false;
        }

        // hot path: the values go straight into the data fields of the ROPs (one critical section for all),
        // then the timestamp. nothing else of the frame is touched
        const bool allread {_config.sharedvariables->get(_items.data(), _count)};
        const uint64_t t {embot::core::now()};
        std::memcpy(_txbuffer.data() + timeoffset, &t, sizeof(t));

        _txframes.fetch_add(1, std::memory_order_relaxed);
        if(false == allread)
        {   // a variable that could not be read keeps its previous value in the frame
            _readfailures.fetch_add(1, std::memory_order_relaxed);
        }

        payload = _txbuffer.data();
        size = _txsize;
        destination = Destination {_host.load(std::memory_order_acquire), _config.hostport};
        return true;
    }

    bool theSTRmanager::onrx(const void *payload, size_t size, const embot::net::eth::IPaddress &from, const void *&reply, size_t &replysize)
    {
        reply = nullptr;
        replysize = 0;
        _rxframes.fetch_add(1, std::memory_order_relaxed);

        Reader reader {payload, size, _config.maxframesize};
        if(Error::none != reader.error())
        {
            _rxdiscarded.fetch_add(1, std::memory_order_relaxed);
            return false;
        }

        touch(from);

        Writer writer {_pingbuffer.data(), _pingbuffer.size()};
        View rx {};
        while(reader.next(rx))
        {
            if(CMD::ping == rx.cmd())
            {
                writer.add(Head::reply(CMD::ack, rx.id(), CMD::ping, 0), nullptr);
                _rxpings.fetch_add(1, std::memory_order_relaxed);
            }
            else
            {
                _rxdiscarded.fetch_add(1, std::memory_order_relaxed);
            }
        }

        if(0 == writer.numberofROPs())
        {
            return false;
        }

        reply = _pingbuffer.data();
        replysize = writer.close(embot::core::now());
        return true;
    }

    // ---- anyone

    theSTRmanager::Stats theSTRmanager::stats() const
    {
        Stats s {};
        s.txframes = _txframes.load(std::memory_order_relaxed);
        s.readfailures = _readfailures.load(std::memory_order_relaxed);
        s.rebuilds = _rebuilds.load(std::memory_order_relaxed);
        s.rxframes = _rxframes.load(std::memory_order_relaxed);
        s.rxpings = _rxpings.load(std::memory_order_relaxed);
        s.rxdiscarded = _rxdiscarded.load(std::memory_order_relaxed);
        s.keepalivestops = _keepalivestops.load(std::memory_order_relaxed);
        return s;
    }

} // namespace embot::svc::tether::rop

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
