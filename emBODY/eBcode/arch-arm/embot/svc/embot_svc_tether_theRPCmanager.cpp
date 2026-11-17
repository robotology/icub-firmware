
/*
 * Copyright (C) 2026 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#include "embot_svc_tether_theRPCmanager.h"
#include <cstddef>
#include <cstring>


namespace {

    using embot::svc::tether::rop::BrowseItem;
    using SHV = embot::svc::tether::SharedVariables;

    // the standard texts of a nak: at most maxtextsize characters, w/out NUL on the wire
    namespace why
    {
        constexpr char unknownid[] = "unknown id";
        constexpr char readonly[] = "read only";
        constexpr char wrongsize[] = "wrong size";
        constexpr char wrongargsize[] = "wrong size of the argument";
        constexpr char notallowed[] = "command not allowed for this id";
        constexpr char failed[] = "operation failed";
        constexpr char noop[] = "no operation to run";
        constexpr char readfailed[] = "read failed";
        constexpr char noroom[] = "no room";
        constexpr char nostreamer[] = "no streamer";
        constexpr char cannotadd[] = "unknown variable, or stream full";
        constexpr char notstreamed[] = "not streamed";
        constexpr char badperiod[] = "period out of range";
        constexpr char cannotstop[] = "cannot stop";
        constexpr char unknownmg[] = "unknown management id";
        constexpr char noitem[] = "index out of range";
        constexpr char nofield[] = "no such field";

        static_assert(sizeof(cannotadd) - 1 <= embot::svc::tether::rop::maxtextsize &&
                      sizeof(notallowed) - 1 <= embot::svc::tether::rop::maxtextsize, "a text must fit a nak");
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


namespace embot::svc::tether::rop {

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

        // the biggest data that a single ROP can carry in a frame of this board
        const size_t room {config.maxframesize - sizeofframeheader - headsize};

        // the variables: in their range of IDs, small enough for a ROP and for a frame, w/ a valid name
        const size_t nv {config.sharedvariables->numberofvariables()};
        if(nv > (VARlast - VARfirst + 1u))
        {
            return false;
        }
        for(size_t i = 0; i < nv; i++)
        {
            SHV::Info info {};
            if((false == config.sharedvariables->info(i, info)) || (false == isvariable(info.id)) ||
               (info.size > maxdatasize) || (info.size > room) || (false == isvalidname(info.name)))
            {
                return false;
            }
        }

        // the operations: valid, not duplicated, w/ a valid name, w/ an argument that fits a frame
        std::array<int8_t, OPlast - OPfirst + 1> index {};
        index.fill(-1);
        if(config.numberofops > index.size())
        {
            return false;
        }
        for(size_t i = 0; i < config.numberofops; i++)
        {
            const OPentry &e = config.ops[i];
            if((false == e.isvalid()) || (false == isvalidname(e.name)) || (e.argsize > room) || (-1 != index[e.op - OPfirst]))
            {
                return false;
            }
            index[e.op - OPfirst] = static_cast<int8_t>(i);
        }

        _config = config;
        _opindex = index;
        _numberofvariables = static_cast<uint8_t>(nv);
        _writer = Writer {_txbuffer.data(), _config.maxframesize};

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
        _writer.reset();
        _txsize = 0;
        _initialised = true;
        return true;
    }

    Error theRPCmanager::parse(const void *payload, size_t size, const embot::net::eth::IPaddress &from)
    {
        _writer.reset();
        _txsize = 0;

        if(false == _initialised)
        {
            return Error::notinitialised;
        }

        _stats.rxframes++;

        // the whole frame is validated before any of its ROPs is executed
        Reader reader {payload, size, _config.maxframesize};
        if(Error::none != reader.error())
        {
            _stats.rxwrongframes++;
            return reader.error();
        }

        if(nullptr != _config.streamer)
        {
            _config.streamer->touch(from);
        }

        View rx {};
        while(reader.next(rx))
        {
            _stats.rxROPs++;
            execute(rx, from);
        }

        return Error::none;
    }

    void theRPCmanager::execute(const View &rx, const embot::net::eth::IPaddress &from)
    {
        // a ROP that expects a reply is executed only if any short reply (ack w/ a result, nak w/ a text) has room.
        // the say of an ask checks its own room later. a ping is not checked: it has no effects, and if its ack does not
        // fit it gets nak<"no room"> or it is dropped
        if(rx.wantsreply() && (false == _writer.fits(maxsizeofshortreply - headsize)))
        {
            nak(rx, why::noroom);
            return;
        }

        if(CMD::ping == rx.cmd())
        {
            ack(rx, 0, nullptr);
        }
        else if(isvariable(rx.id()))
        {
            executevariable(rx);
        }
        else if(isop(rx.id()))
        {
            executeop(rx, from);
        }
        else if(ismg(rx.id()))
        {
            executemg(rx, from);
        }
        else
        {   // IDnone
            refuse(rx);
        }
    }

    void theRPCmanager::executevariable(const View &rx)
    {
        SHV *vars {_config.sharedvariables};

        switch(rx.cmd())
        {
            case CMD::ask:
            {
                const uint16_t s {vars->size(rx.id())};
                if(0 == s)
                {   // unknown to the board, even if the host knows it
                    nak(rx, why::unknownid);
                    break;
                }
                // the value goes straight into the reply
                uint8_t *data {_writer.reserve(s)};
                if(nullptr == data)
                {
                    nak(rx, why::noroom);
                }
                else if(vars->get(rx.id(), data, s))
                {
                    _writer.commit(Head::request(CMD::say, OPTnone, rx.id(), s));
                }
                else
                {
                    nak(rx, why::readfailed);
                }
            } break;

            case CMD::set:
            case CMD::sig:
            {
                const bool ok {vars->set(rx.id(), rx.data, rx.size(), SHV::Origin::remote)};
                if((CMD::set == rx.cmd()) && rx.ackrequired())
                {
                    if(ok)
                    {   // light: it tells only that the value is written
                        ack(rx, 0, nullptr);
                    }
                    else if(0 == vars->size(rx.id()))
                    {
                        nak(rx, why::unknownid);
                    }
                    else if(rx.size() != vars->size(rx.id()))
                    {
                        nak(rx, why::wrongsize);
                    }
                    else if(SHV::Access::RW != vars->access(rx.id()))
                    {
                        nak(rx, why::readonly);
                    }
                    else
                    {
                        nak(rx, why::failed);
                    }
                }
            } break;

            default:
            {
                refuse(rx);
            } break;
        }
    }

    void theRPCmanager::executeop(const View &rx, const embot::net::eth::IPaddress &from)
    {
        if(CMD::run != rx.cmd())
        {
            refuse(rx);
            return;
        }

        fpRUN run {_config.fallback};
        void *param {_config.fallbackparam};
        bool sizeok {true};

        const int8_t i {_opindex[rx.id() - OPfirst]};
        if(-1 != i)
        {
            const OPentry &e = _config.ops[i];
            run = e.run;
            param = e.param;
            sizeok = (rx.size() == e.argsize);
        }

        // the argument stays inside the received frame. the result and the text are small: they stay on the stack
        OPcall call {rx.id(), rx.data, rx.size(), from};
        const bool ok {sizeok && (nullptr != run) && run(call, param)};

        if(rx.ackrequired())
        {
            if(ok && (call.resultsize <= maxresultsize))
            {
                ack(rx, call.resultsize, call.result);
            }
            else if(false == sizeok)
            {
                nak(rx, why::wrongargsize);
            }
            else if(nullptr == run)
            {
                nak(rx, why::noop);
            }
            else if(call.errorsize > 0)
            {   // the text given by the operation
                nak(rx, call.error, call.errorsize);
            }
            else
            {
                nak(rx, why::failed);
            }
        }
    }

    void theRPCmanager::executemg(const View &rx, const embot::net::eth::IPaddress &from)
    {
        theSTRmanager *str {_config.streamer};

        switch(rx.cmd())
        {
            case CMD::ask:
            {
                switch(rx.id())
                {
                    case MGstreamLIST:
                    {
                        if(nullptr == str) { nak(rx, why::nostreamer); break; }
                        say(rx, str->list());
                    } break;

                    case MGstreamSTART:
                    {
                        if(nullptr == str) { nak(rx, why::nostreamer); break; }
                        say(rx, str->periodms());
                    } break;

                    case MGstats:
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
                        say(rx, s);
                    } break;

                    case MGbrowseINFO:
                    {
                        const BrowseInfo info {_numberofvariables, static_cast<uint8_t>(_config.numberofops), FrameHeader::maxROPs,
                                               _config.maxframesize, maxdatasize, _crc32};
                        say(rx, info);
                    } break;

                    case MGbrowseITEM:
                    {
                        BrowseItem item {};
                        if((rx.size() >= 1) && describeitem(rx.data[0], item))
                        {
                            say(rx, item);
                        }
                        else
                        {
                            nak(rx, (rx.size() >= 1) ? why::noitem : why::wrongsize);
                        }
                    } break;

                    case MGbrowseFIELD:
                    {
                        BrowseField field {};
                        if((rx.size() >= 2) && describefield(rx.data[0], rx.data[1], field))
                        {
                            say(rx, field);
                        }
                        else
                        {
                            nak(rx, (rx.size() >= 2) ? why::nofield : why::wrongsize);
                        }
                    } break;

                    case MGversion:
                    {
                        const Version v {protocolversion, _config.applicationversion};
                        say(rx, v);
                    } break;

                    default:
                    {
                        nak(rx, why::unknownmg);
                    } break;
                }
            } break;

            case CMD::set:
            case CMD::sig:
            {
                bool ok {false};
                const char *reason {why::unknownmg};        // the text of the nak if not ok
                const bool isstream {(MGstreamADD == rx.id()) || (MGstreamREM == rx.id()) ||
                                     (MGstreamSTART == rx.id()) || (MGstreamSTOP == rx.id())};
                if(false == isstream)
                {
                    reason = why::unknownmg;
                }
                else if(nullptr == str)
                {
                    reason = why::nostreamer;
                }
                else if((MGstreamADD == rx.id()) && (sizeof(ID) == rx.size()))
                {
                    ok = str->add(rx.data[0]);
                    reason = why::cannotadd;
                }
                else if((MGstreamREM == rx.id()) && (sizeof(ID) == rx.size()))
                {
                    ok = str->rem(rx.data[0]);
                    reason = why::notstreamed;
                }
                else if((MGstreamSTART == rx.id()) && (sizeof(uint16_t) == rx.size()))
                {   // the stream goes to the sender of this ROP. the period applied: ask<MGstreamSTART>
                    ok = str->start(rx.value<uint16_t>(), from);
                    reason = why::badperiod;
                }
                else if((MGstreamSTOP == rx.id()) && (0 == rx.size()))
                {
                    ok = str->stop();
                    reason = why::cannotstop;
                }
                else
                {
                    reason = why::wrongsize;
                }
                if((CMD::set == rx.cmd()) && rx.ackrequired())
                {
                    if(ok)
                    {
                        ack(rx, 0, nullptr);
                    }
                    else
                    {
                        nak(rx, reason);
                    }
                }
            } break;

            default:
            {
                refuse(rx);
            } break;
        }
    }

    // ---- the replies

    void theRPCmanager::reply(const Head &head, const void *data)
    {
        if(nullptr != _writer.add(head, data))
        {
            return;
        }
        // no room: a nak w/ the reason, if at least that fits. a say is the reply to an ask
        const CMD referred {(CMD::say == head.cmd()) ? CMD::ask : head.referredcmd()};
        const Head n {Head::reply(CMD::nak, head.id, referred, static_cast<uint16_t>(sizeof(why::noroom) - 1))};
        if((CMD::nak == head.cmd()) || (nullptr == _writer.add(n, why::noroom)))
        {
            _stats.droppedROPs++;
        }
    }

    void theRPCmanager::ack(const View &rx, uint16_t size, const void *data)
    {
        reply(Head::reply(CMD::ack, rx.id(), rx.cmd(), size), data);
    }

    // text is NUL terminated: it is cut at maxtextsize characters
    void theRPCmanager::nak(const View &rx, const char *text)
    {
        nak(rx, text, std::strlen(text));
    }

    void theRPCmanager::nak(const View &rx, const char *text, size_t size)
    {
        const uint16_t n {static_cast<uint16_t>((size < maxtextsize) ? size : maxtextsize)};
        if(nullptr == _writer.add(Head::reply(CMD::nak, rx.id(), rx.cmd(), n), text))
        {
            _stats.droppedROPs++;
        }
    }

    void theRPCmanager::refuse(const View &rx)
    {
        if(rx.wantsreply())
        {
            nak(rx, why::notallowed);
        }
    }

    // ---- the browse

    bool theRPCmanager::describeitem(uint8_t index, BrowseItem &item) const
    {
        if(index < _numberofvariables)
        {
            SHV::Info i {};
            if(false == _config.sharedvariables->info(index, i))
            {
                return false;
            }
            item = BrowseItem {index, i.id, BrowseItem::kindVariable, static_cast<uint8_t>(i.access), static_cast<uint8_t>(i.type),
                               i.numberoffields, 0, BrowseItem::resultnone, i.size, 0, i.name};
            return true;
        }

        const size_t o {static_cast<size_t>(index) - _numberofvariables};
        if(o < _config.numberofops)
        {
            const OPentry &e = _config.ops[o];
            const uint8_t resultkind {(Type::structure == e.resulttype) ? BrowseItem::resultstruct :
                                      (SHV::isscalar(e.resulttype) ? BrowseItem::resultscalar : BrowseItem::resultnone)};
            item = BrowseItem {index, e.op, BrowseItem::kindOP, 0, static_cast<uint8_t>(e.argtype),
                               e.argfieldscount(), e.resultfieldscount(), resultkind, e.argsize, e.resultsize, e.name};
            return true;
        }

        return false;
    }

    bool theRPCmanager::getfield(ID id, uint8_t n, SHV::Field &field) const
    {
        if(isvariable(id))
        {
            return _config.sharedvariables->field(id, n, field);
        }
        if(isop(id))
        {
            const int8_t i {_opindex[id - OPfirst]};
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

    bool theRPCmanager::get(const void *&payload, size_t &size)
    {
        payload = nullptr;
        size = 0;
        if(0 == _writer.numberofROPs())
        {
            return false;
        }
        if(0 == _txsize)
        {
            _txsize = _writer.close(embot::core::now());
        }
        payload = _txbuffer.data();
        size = _txsize;
        return true;
    }

} // namespace embot::svc::tether::rop

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
