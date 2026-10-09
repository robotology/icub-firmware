
/*
 * Copyright (C) 2026 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#include "embot_app_eth_theRPCmanager.h"
#include <cstddef>
#include <cstring>


namespace {

    using embot::app::eth::rop::ROP;
    using embot::app::eth::rop::BrowseItem;
    using SHV = embot::app::eth::SharedVariables;

    // the replies

    void ack(const ROP &rx, ROP &reply, uint8_t size, const void *data)
    {
        reply.load(ROP::CMD::ack, ROP::OPTnone, rx.id, size, data, rx.cmd());
    }

    // the standard texts of a nak: at most ROP::datasize characters, w/out NUL on the wire
    namespace why
    {
        constexpr char unknownid[] = "unknown id";
        constexpr char readonly[] = "read only";
        constexpr char wrongsize[] = "wrong size";
        constexpr char wrongargsize[] = "wrong size of the argument";
        constexpr char malformed[] = "malformed rop";
        constexpr char notallowed[] = "command not allowed for this id";
        constexpr char failed[] = "operation failed";
        constexpr char noop[] = "no operation to run";
        constexpr char readfailed[] = "read failed";
        constexpr char nostreamer[] = "no streamer";
        constexpr char cannotadd[] = "unknown variable, or stream full";
        constexpr char notstreamed[] = "not streamed";
        constexpr char badperiod[] = "period out of range";
        constexpr char cannotstop[] = "cannot stop";
        constexpr char unknownmg[] = "unknown management id";
        constexpr char noitem[] = "index out of range";
        constexpr char nofield[] = "no such field";

        static_assert(sizeof(cannotadd) - 1 <= ROP::datasize && sizeof(notallowed) - 1 <= ROP::datasize, "a text must fit a ROP");
    }

    // text is NUL terminated: it is cut at ROP::datasize characters
    void nak(const ROP &rx, ROP &reply, const char *text, size_t size)
    {
        const size_t n {(size < ROP::datasize) ? size : ROP::datasize};
        reply.load(ROP::CMD::nak, ROP::OPTnone, rx.id, static_cast<uint8_t>(n), text, rx.cmd());
    }

    void nak(const ROP &rx, ROP &reply, const char *text)
    {
        nak(rx, reply, text, std::strlen(text));
    }

    void say(const ROP &rx, ROP &reply, uint8_t size, const void *data)
    {
        reply.load(ROP::CMD::say, ROP::OPTnone, rx.id, size, data, ROP::CMD::none);
    }

    // a CMD not valid for the ID: a nak only if the sender expects a reply
    void refuse(const ROP &rx, ROP &reply)
    {
        if((ROP::CMD::ask == rx.cmd()) || rx.ackrequired())
        {
            nak(rx, reply, why::notallowed);
        }
    }

    // the standard CRC-32 (as zlib.crc32), bitwise: it runs only at initialisation
    uint32_t crc32update(uint32_t crc, const void *data, size_t size)
    {
        const uint8_t *p {static_cast<const uint8_t *>(data)};
        for(size_t i = 0; i < size; i++)
        {
            crc ^= p[i];
            for(uint8_t k = 0; k < 8; k++)
            {
                crc = (crc >> 1) ^ (0xEDB88320u & (0u - (crc & 1u)));
            }
        }
        return crc;
    }

}


namespace embot::app::eth::rop {

    theRPCmanager &theRPCmanager::getInstance()
    {
        static theRPCmanager *p = new theRPCmanager();
        return *p;
    }

    bool theRPCmanager::initialise(const Config &config)
    {
        _initialised = false;
        if(false == config.isvalid())
        {
            return false;
        }

        // the variables: in their range of IDs, small enough for a ROP, w/ a valid name
        const size_t nv {config.sharedvariables->numberofvariables()};
        if(nv > (ROP::VARlast - ROP::VARfirst + 1u))
        {
            return false;
        }
        for(size_t i = 0; i < nv; i++)
        {
            SHV::Info info {};
            if((false == config.sharedvariables->info(i, info)) || (false == ROP::isvariable(info.id)) ||
               (info.size > ROP::datasize) || (false == isvalidname(info.name)))
            {
                return false;
            }
        }

        // the operations: valid, not duplicated, w/ a valid name
        std::array<int8_t, ROP::OPlast - ROP::OPfirst + 1> index {};
        index.fill(-1);
        if(config.numberofops > index.size())
        {
            return false;
        }
        for(size_t i = 0; i < config.numberofops; i++)
        {
            const OPentry &e = config.ops[i];
            if((false == e.isvalid()) || (false == isvalidname(e.name)) || (-1 != index[e.op - ROP::OPfirst]))
            {
                return false;
            }
            index[e.op - ROP::OPfirst] = static_cast<int8_t>(i);
        }

        _config = config;
        _opindex = index;
        _numberofvariables = static_cast<uint8_t>(nv);

        // the names are the keys of the host: no duplicates among variables and operations
        const size_t total {nv + config.numberofops};
        for(size_t i = 0; i < total; i++)
        {
            BrowseItem a {};
            describeitem(static_cast<uint8_t>(i), a);
            for(size_t j = i + 1; j < total; j++)
            {
                BrowseItem b {};
                describeitem(static_cast<uint8_t>(j), b);
                if(0 == std::memcmp(a.name, b.name, BrowseItem::namesize))
                {
                    return false;
                }
            }
        }

        // the fields of the structs and the arguments and results of the operations: valid names, no duplicates in
        // the same group (the fields of a variable, the argument, the result). they are keys of the host too
        for(size_t i = 0; i < total; i++)
        {
            BrowseItem item {};
            describeitem(static_cast<uint8_t>(i), item);
            for(uint8_t n = 0; n < item.totalfields(); n++)
            {
                SHV::Field f {};
                if((false == getfield(item.id, n, f)) || (false == isvalidname(f.name)))
                {
                    return false;
                }
                for(uint8_t m = (n < item.numberoffields) ? 0 : item.numberoffields; m < n; m++)
                {
                    SHV::Field g {};
                    getfield(item.id, m, g);
                    if(samename(f.name, g.name))
                    {
                        return false;
                    }
                }
            }
        }

        _crc32 = computecrc32();
        _stats = Stats {};
        _numberofROPs = 0;
        _compacted = false;
        _txsize = 0;
        _initialised = true;
        return true;
    }

    Error theRPCmanager::parse(const void *payload, size_t size, const embot::net::eth::IPaddress &from)
    {
        _numberofROPs = 0;
        _compacted = false;
        _txsize = 0;

        if(false == _initialised)
        {
            return Error::notinitialised;
        }

        _stats.rxframes++;

        FrameHeader header {};
        const Error e {checkframe(payload, size, header)};
        if(Error::none != e)
        {
            _stats.rxwrongframes++;
            return e;
        }

        if(nullptr != _config.streamer)
        {
            _config.streamer->touch(from);
        }

        const size_t capacity {header.sizeofROP - ROP::headersize};
        ROP rx {};
        ROP reply {};

        for(uint8_t n = 0; n < header.numOfROPs; n++)
        {
            extract(payload, header, n, rx);
            _stats.rxROPs++;
            reply.clear();

            if(rx.size > capacity)
            {   // malformed: its data does not fit its slot
                nak(rx, reply, why::malformed);
            }
            else if(ROP::CMD::ping == rx.cmd())
            {
                ack(rx, reply, 0, nullptr);
            }
            else if(ROP::isvariable(rx.id))
            {
                executevariable(rx, reply);
            }
            else if(ROP::isop(rx.id))
            {
                executeop(rx, reply, from);
            }
            else if(ROP::ismg(rx.id))
            {
                executemg(rx, reply, from);
            }
            else
            {   // ROP::IDnone
                refuse(rx, reply);
            }

            if((ROP::CMD::none != reply.cmd()) && (false == add(reply)))
            {
                _stats.droppedROPs++;
            }
        }

        return Error::none;
    }

    void theRPCmanager::executevariable(const ROP &rx, ROP &reply)
    {
        SHV *vars {_config.sharedvariables};
        uint8_t value[ROP::datasize] {0};

        switch(rx.cmd())
        {
            case ROP::CMD::ask:
            {
                const uint8_t s {vars->size(rx.id)};
                if((s > 0) && vars->get(rx.id, value, sizeof(value)))
                {
                    say(rx, reply, s, value);
                }
                else if(0 == s)
                {   // unknown to the board, even if the host knows it
                    nak(rx, reply, why::unknownid);
                }
                else
                {
                    nak(rx, reply, why::readfailed);
                }
            } break;

            case ROP::CMD::set:
            case ROP::CMD::sig:
            {
                const bool ok {vars->set(rx.id, rx.data, rx.size, SHV::Origin::remote)};
                if((ROP::CMD::set == rx.cmd()) && rx.ackrequired())
                {
                    if(ok && vars->get(rx.id, value, sizeof(value)))
                    {   // the ack carries the value now held by the variable
                        ack(rx, reply, vars->size(rx.id), value);
                    }
                    else if(0 == vars->size(rx.id))
                    {
                        nak(rx, reply, why::unknownid);
                    }
                    else if(rx.size != vars->size(rx.id))
                    {
                        nak(rx, reply, why::wrongsize);
                    }
                    else if(SHV::Access::RW != vars->access(rx.id))
                    {
                        nak(rx, reply, why::readonly);
                    }
                    else
                    {
                        nak(rx, reply, why::failed);
                    }
                }
            } break;

            default:
            {
                refuse(rx, reply);
            } break;
        }
    }

    void theRPCmanager::executeop(const ROP &rx, ROP &reply, const embot::net::eth::IPaddress &from)
    {
        if(ROP::CMD::run != rx.cmd())
        {
            refuse(rx, reply);
            return;
        }

        fpRUN run {_config.fallback};
        void *param {_config.fallbackparam};
        bool sizeok {true};

        const int8_t i {_opindex[rx.id - ROP::OPfirst]};
        if(-1 != i)
        {
            const OPentry &e = _config.ops[i];
            run = e.run;
            param = e.param;
            sizeok = (rx.size == e.argsize);
        }

        OPcall call {rx.id, rx.data, rx.size, from};
        const bool ok {sizeok && (nullptr != run) && run(call, param)};

        if(rx.ackrequired())
        {
            if(ok && (call.resultsize <= ROP::datasize))
            {
                ack(rx, reply, call.resultsize, call.result);
            }
            else if(false == sizeok)
            {
                nak(rx, reply, why::wrongargsize);
            }
            else if(nullptr == run)
            {
                nak(rx, reply, why::noop);
            }
            else if(call.errorsize > 0)
            {   // the text given by the operation
                nak(rx, reply, call.error, call.errorsize);
            }
            else
            {
                nak(rx, reply, why::failed);
            }
        }
    }

    void theRPCmanager::executemg(const ROP &rx, ROP &reply, const embot::net::eth::IPaddress &from)
    {
        theSTRmanager *str {_config.streamer};

        switch(rx.cmd())
        {
            case ROP::CMD::ask:
            {
                switch(rx.id)
                {
                    case ROP::MGstreamLIST:
                    {
                        if(nullptr == str) { nak(rx, reply, why::nostreamer); break; }
                        const StreamList l {str->list()};
                        say(rx, reply, sizeof(l), &l);
                    } break;

                    case ROP::MGstreamSTART:
                    {
                        if(nullptr == str) { nak(rx, reply, why::nostreamer); break; }
                        const uint16_t ms {str->periodms()};
                        say(rx, reply, sizeof(ms), &ms);
                    } break;

                    case ROP::MGstats:
                    {
                        Statistics s {};
                        s.rpcrxframes = _stats.rxframes;
                        s.rpcrxwrongframes = _stats.rxwrongframes;
                        s.rpcrxROPs = _stats.rxROPs;
                        s.rpcdroppedROPs = _stats.droppedROPs;
                        if(nullptr != str)
                        {
                            const theSTRmanager::Stats ss {str->stats()};
                            s.strtxframes = ss.txframes;
                            s.strreadfailures = ss.readfailures;
                            s.strrxpings = ss.rxpings;
                            s.strrxdiscarded = ss.rxdiscarded;
                            s.strkeepalivestops = ss.keepalivestops;
                        }
                        say(rx, reply, sizeof(s), &s);
                    } break;

                    case ROP::MGbrowseINFO:
                    {
                        const BrowseInfo info {_numberofvariables, static_cast<uint8_t>(_config.numberofops), FrameHeader::maxROPs, _crc32};
                        say(rx, reply, sizeof(info), &info);
                    } break;

                    case ROP::MGbrowseITEM:
                    {
                        BrowseItem item {};
                        if((rx.size >= 1) && describeitem(rx.data[0], item))
                        {
                            say(rx, reply, sizeof(item), &item);
                        }
                        else
                        {
                            nak(rx, reply, (rx.size >= 1) ? why::noitem : why::wrongsize);
                        }
                    } break;

                    case ROP::MGbrowseFIELD:
                    {
                        BrowseField field {};
                        if((rx.size >= 2) && describefield(rx.data[0], rx.data[1], field))
                        {
                            say(rx, reply, sizeof(field), &field);
                        }
                        else
                        {
                            nak(rx, reply, (rx.size >= 2) ? why::nofield : why::wrongsize);
                        }
                    } break;

                    case ROP::MGversion:
                    {
                        const Version v {protocolversion, _config.applicationversion};
                        say(rx, reply, sizeof(v), &v);
                    } break;

                    default:
                    {
                        nak(rx, reply, why::unknownmg);
                    } break;
                }
            } break;

            case ROP::CMD::set:
            case ROP::CMD::sig:
            {
                bool ok {false};
                const char *reason {why::unknownmg};        // the text of the nak if not ok
                // the ack echoes the value, but for MGstreamSTART it carries the period applied
                uint8_t ackvalue[ROP::datasize] {0};
                uint8_t acksize {rx.size};
                std::memcpy(ackvalue, rx.data, rx.size);
                const bool isstream {(ROP::MGstreamADD == rx.id) || (ROP::MGstreamREM == rx.id) ||
                                     (ROP::MGstreamSTART == rx.id) || (ROP::MGstreamSTOP == rx.id)};
                if(false == isstream)
                {
                    reason = why::unknownmg;
                }
                else if(nullptr == str)
                {
                    reason = why::nostreamer;
                }
                else if((ROP::MGstreamADD == rx.id) && (sizeof(ID) == rx.size))
                {
                    ok = str->add(rx.data[0]);
                    reason = why::cannotadd;
                }
                else if((ROP::MGstreamREM == rx.id) && (sizeof(ID) == rx.size))
                {
                    ok = str->rem(rx.data[0]);
                    reason = why::notstreamed;
                }
                else if((ROP::MGstreamSTART == rx.id) && (sizeof(uint16_t) == rx.size))
                {   // the stream goes to the sender of this ROP
                    ok = str->start(rx.value<uint16_t>(), from);
                    reason = why::badperiod;
                    const uint16_t applied {str->periodms()};
                    std::memcpy(ackvalue, &applied, sizeof(applied));
                }
                else if((ROP::MGstreamSTOP == rx.id) && (0 == rx.size))
                {
                    ok = str->stop();
                    reason = why::cannotstop;
                }
                else
                {
                    reason = why::wrongsize;
                }
                if((ROP::CMD::set == rx.cmd()) && rx.ackrequired())
                {
                    if(ok)
                    {
                        ack(rx, reply, acksize, ackvalue);
                    }
                    else
                    {
                        nak(rx, reply, reason, std::strlen(reason));
                    }
                }
            } break;

            default:
            {
                refuse(rx, reply);
            } break;
        }
    }

    bool theRPCmanager::describeitem(uint8_t index, BrowseItem &item) const
    {
        if(index < _numberofvariables)
        {
            SHV::Info i {};
            if(false == _config.sharedvariables->info(index, i))
            {
                return false;
            }
            item = BrowseItem {index, i.id, BrowseItem::kindVariable, i.size, static_cast<uint8_t>(i.access),
                               static_cast<uint8_t>(i.type), i.numberoffields, 0, i.name};
            return true;
        }

        const size_t o {static_cast<size_t>(index) - _numberofvariables};
        if(o < _config.numberofops)
        {
            const OPentry &e = _config.ops[o];
            const uint8_t resultkind {(Type::structure == e.resulttype) ? BrowseItem::resultstruct :
                                      (SHV::isscalar(e.resulttype) ? BrowseItem::resultscalar : BrowseItem::resultnone)};
            item = BrowseItem {index, e.op, BrowseItem::kindOP, e.argsize, BrowseItem::resultinfo(e.resultsize, resultkind),
                               static_cast<uint8_t>(e.argtype), e.argfieldscount(), e.resultfieldscount(), e.name};
            return true;
        }

        return false;
    }

    bool theRPCmanager::getfield(ID id, uint8_t n, SHV::Field &field) const
    {
        if(ROP::isvariable(id))
        {
            return _config.sharedvariables->field(id, n, field);
        }
        if(ROP::isop(id))
        {
            const int8_t i {_opindex[id - ROP::OPfirst]};
            return (i >= 0) && _config.ops[static_cast<size_t>(i)].field(n, field);
        }
        return false;
    }

    bool theRPCmanager::describefield(ID id, uint8_t n, BrowseField &field) const
    {
        SHV::Field f {};
        if(false == getfield(id, n, f))
        {
            return false;
        }
        field = BrowseField {id, n, static_cast<uint8_t>(f.type), f.count, f.offset, f.name};
        return true;
    }

    uint32_t theRPCmanager::computecrc32() const
    {
        // all the items in order of index, then the fields of each item (variable or operation) in the same order
        uint32_t crc {0xFFFFFFFFu};
        const size_t total {_numberofvariables + _config.numberofops};
        for(size_t i = 0; i < total; i++)
        {
            BrowseItem item {};
            describeitem(static_cast<uint8_t>(i), item);
            crc = crc32update(crc, &item, sizeof(item));
        }
        for(size_t i = 0; i < total; i++)
        {
            BrowseItem item {};
            describeitem(static_cast<uint8_t>(i), item);
            for(uint8_t n = 0; n < item.totalfields(); n++)
            {
                BrowseField field {};
                describefield(item.id, n, field);
                crc = crc32update(crc, &field, sizeof(field));
            }
        }
        return crc ^ 0xFFFFFFFFu;
    }

    bool theRPCmanager::add(const ROP &rop)
    {
        if(_numberofROPs >= FrameHeader::maxROPs)
        {
            return false;
        }
        std::memcpy(_txbuffer.data() + sizeof(FrameHeader) + _numberofROPs * ROP::maxsizeofROP, &rop, ROP::maxsizeofROP);
        _numberofROPs++;
        return true;
    }

    bool theRPCmanager::get(const void *&payload, size_t &size)
    {
        payload = nullptr;
        size = 0;
        if(0 == _numberofROPs)
        {
            return false;
        }

        if(false == _compacted)
        {
            // the smallest slot that holds the biggest reply
            uint8_t *base {_txbuffer.data() + sizeof(FrameHeader)};
            uint8_t biggest {0};
            for(uint8_t i = 0; i < _numberofROPs; i++)
            {
                const uint8_t s {base[i * ROP::maxsizeofROP + offsetof(ROP, size)]};
                biggest = (s > biggest) ? s : biggest;
            }
            const uint8_t slot {ROP::slotsize(biggest)};

            // compact in place: slot i moves down from i * 40 to i * slot, so it never overwrites a slot still to move
            for(uint8_t i = 1; i < _numberofROPs; i++)
            {
                std::memmove(base + i * slot, base + i * ROP::maxsizeofROP, slot);
            }

            const FrameHeader header {slot, _numberofROPs, embot::core::now()};
            std::memcpy(_txbuffer.data(), &header, sizeof(FrameHeader));
            _txsize = sizeof(FrameHeader) + _numberofROPs * static_cast<size_t>(slot);
            _compacted = true;
        }

        payload = _txbuffer.data();
        size = _txsize;
        return true;
    }

} // namespace embot::app::eth::rop

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
