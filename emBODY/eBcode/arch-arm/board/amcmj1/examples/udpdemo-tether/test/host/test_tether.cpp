// test/host: TETHER (protocol 6) w/ the Dictionary of udpdemo-tether, on a PC, w/out sockets and threads.
// the frames go straight into theRPCmanager and theSTRmanager, and their replies are parsed w/ rop::Reader:
// - management: version, browse (BrowseInfo of 12 bytes, BrowseItem, BrowseField w/ 16-bit sizes)
// - the big values: ask of 60 / 268 / 400 bytes, set+ack of 400 bytes, run+ack w/ an argument of 400 bytes
// - the light ack: no data, but for the result of a run
// - the room of the reply: a ROP that expects a reply is not executed if its reply has no room
// - the malformed frames and the frames of version 5: no reply
// - the stream: the budget of the frame, a frame w/ the big values, the ping

#include "embot_svc_tether_ROP.h"
#include "embot_svc_tether_SharedVariables.h"
#include "embot_svc_tether_theRPCmanager.h"
#include "embot_svc_tether_theSTRmanager.h"
#include "embot_svc_theTETHERservice.h"     // the stub of test/host/stub
#include "dictionary.udpdemo.h"

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

// in udpdemo-tether.cpp, which is not compiled here
namespace udpdemo::echo {
    float readwave() { return 0.0f; }
    PWM readpwm() { return PWM {}; }
}

namespace {

    namespace rop = embot::svc::tether::rop;
    using rop::CMD;
    using rop::Head;
    using rop::OPTack;
    using rop::OPTnone;

    int passed {0};
    int failed {0};

    void check(bool ok, const char *what)
    {
        std::printf("  %s  %s\n", ok ? "PASS" : "FAIL", what);
        ok ? passed++ : failed++;
    }

    // a ROP of a reply, w/ a copy of its data
    struct Rx
    {
        Head head {};
        std::vector<uint8_t> data {};

        CMD cmd() const { return head.cmd(); }
        std::string text() const { return std::string(data.begin(), data.end()); }
        template<typename T> T value() const { T v {}; std::memcpy(&v, data.data(), (sizeof(T) < data.size()) ? sizeof(T) : data.size()); return v; }
    };

    struct Request
    {
        Head head {};
        std::vector<uint8_t> data {};
    };

    Request ask(rop::ID id, std::vector<uint8_t> arg = {}) { return Request {Head::request(CMD::ask, OPTnone, id, static_cast<uint16_t>(arg.size())), arg}; }
    Request setack(rop::ID id, const void *v, size_t s)
    {
        const uint8_t *p {static_cast<const uint8_t *>(v)};
        return Request {Head::request(CMD::set, OPTack, id, static_cast<uint16_t>(s)), std::vector<uint8_t>(p, p + s)};
    }
    Request runack(rop::ID id, const void *v, size_t s)
    {
        const uint8_t *p {static_cast<const uint8_t *>(v)};
        return Request {Head::request(CMD::run, OPTack, id, static_cast<uint16_t>(s)), std::vector<uint8_t>(p, p + s)};
    }

    std::vector<uint8_t> build(const std::vector<Request> &requests)
    {
        std::vector<uint8_t> b(rop::maxsizeofframe);
        rop::Writer w {b.data(), b.size()};
        for(const Request &r : requests)
        {
            w.add(r.head, r.data.empty() ? nullptr : r.data.data());
        }
        b.resize(w.close(0));
        return b;
    }

    std::vector<Rx> parse(const void *payload, size_t size)
    {
        std::vector<Rx> out;
        rop::Reader r {payload, size, rop::maxsizeofframe};
        rop::View v {};
        while(r.next(v))
        {
            out.push_back(Rx {v.head, std::vector<uint8_t>(v.data, v.data + v.size())});
        }
        return out;
    }

    const embot::net::eth::IPaddress host {10, 0, 1, 104};
    size_t lastsize {0};

    // a frame to theRPCmanager: its reply, parsed. empty if there is no reply
    std::vector<Rx> rpc(const std::vector<Request> &requests)
    {
        rop::theRPCmanager &m {rop::theRPCmanager::getInstance()};
        const std::vector<uint8_t> f {build(requests)};
        m.parse(f.data(), f.size(), host);
        const void *p {nullptr};
        size_t s {0};
        lastsize = 0;
        if(false == m.get(p, s))
        {
            return {};
        }
        lastsize = s;
        return parse(p, s);
    }

    std::vector<Rx> rpcraw(const std::vector<uint8_t> &f)
    {
        rop::theRPCmanager &m {rop::theRPCmanager::getInstance()};
        m.parse(f.data(), f.size(), host);
        const void *p {nullptr};
        size_t s {0};
        return m.get(p, s) ? parse(p, s) : std::vector<Rx> {};
    }

    bool isnak(const Rx &r, rop::ID id, CMD referred, const char *text)
    {
        return (CMD::nak == r.cmd()) && (id == r.head.id) && (referred == r.head.referredcmd()) && (r.text() == text);
    }

    bool isack(const Rx &r, rop::ID id, CMD referred, uint16_t size)
    {
        return (CMD::ack == r.cmd()) && (id == r.head.id) && (referred == r.head.referredcmd()) && (size == r.head.size);
    }

    constexpr rop::ID IDticks {0x05};
    constexpr rop::ID IDgain {0x06};
    constexpr rop::ID IDflag {0x09};
    constexpr rop::ID IDechoSTATS {0x03};
    constexpr rop::ID IDclock {0x0A};
    constexpr rop::ID IDskinPATCH {0x0F};
    constexpr rop::ID IDhistogram {0x10};
    constexpr rop::ID IDtable {0x11};
    constexpr rop::ID OPdivide {0xA6};
    constexpr rop::ID OPechoRESET {0xA8};
    constexpr rop::ID OPloadTable {0xAA};
    constexpr rop::ID OPgetClock {0xA4};

    embot::svc::tether::SharedVariablesLUT *vars {nullptr};

    void setup()
    {
        std::printf("setup\n");
        const auto &d {udpdemo::dict::dictionary};
        vars = new embot::svc::tether::SharedVariablesLUT {d.variables, d.numberofvariables};
        check(vars->initialise(), "the SharedVariables of the Dictionary");
        embot::svc::theTETHERservice::getInstance().set(vars);

        // the stream w/ frames of 600 bytes only: so its budget can be tested w/ the variables of udpdemo
        rop::theSTRmanager &str {rop::theSTRmanager::getInstance()};
        const rop::theSTRmanager::Config sc {vars, 100 * embot::core::time1millisec, embot::core::time1millisec,
                                             10 * embot::core::time1second, 7777, 0, nullptr, nullptr, 600};
        check(str.initialise(sc), "theSTRmanager");

        rop::theRPCmanager &rpc {rop::theRPCmanager::getInstance()};
        const rop::theRPCmanager::Config small {vars, &str, d.operations, d.numberofoperations, nullptr, nullptr, 0x01020304, 256};
        check(false == rpc.initialise(small), "frames of 256 bytes are refused: IDtable (400 bytes) would not fit");
        const rop::theRPCmanager::Config cfg {vars, &str, d.operations, d.numberofoperations, nullptr, nullptr, 0x01020304, rop::maxsizeofframe};
        check(rpc.initialise(cfg), "theRPCmanager w/ frames of 1472 bytes");
    }

    void management()
    {
        std::printf("management\n");
        std::vector<Rx> r {rpc({ask(rop::MGversion), ask(rop::MGbrowseINFO)})};
        const rop::Version v {(2 == r.size()) ? r[0].value<rop::Version>() : rop::Version {}};
        check((2 == r.size()) && (CMD::say == r[0].cmd()) && (6 == v.protocol) && (0x01020304 == v.application), "MGversion: protocol 6");
        const rop::BrowseInfo bi {(2 == r.size()) ? r[1].value<rop::BrowseInfo>() : rop::BrowseInfo {}};
        check((2 == r.size()) && (12 == r[1].head.size) && (1472 == bi.maxframesize) && (512 == bi.maxdatasize) &&
              (15 == bi.numberofvariables) && (11 == bi.numberofops) && (32 == bi.maxrops), "MGbrowseINFO: 12 bytes w/ maxframesize, maxdatasize");

        // IDtable is item 14, OPloadTable is the last one
        r = rpc({ask(rop::MGbrowseITEM, {14}), ask(rop::MGbrowseITEM, {25}), ask(rop::MGbrowseITEM, {26})});
        const rop::BrowseItem t {r[0].value<rop::BrowseItem>()};
        check((40 == r[0].head.size) && (IDtable == t.id) && (400 == t.size) && (0 == t.resultsize) && (0 == std::strcmp(t.name, "IDtable")),
              "MGbrowseITEM: IDtable, 400 bytes");
        const rop::BrowseItem o {r[1].value<rop::BrowseItem>()};
        check((OPloadTable == o.id) && (400 == o.size) && (4 == o.resultsize) && (rop::BrowseItem::resultscalar == o.resultkind) &&
              (1 == o.numberoffields) && (1 == o.numberofresultfields), "MGbrowseITEM: OPloadTable, argument 400 bytes, result 4");
        check(isnak(r[2], rop::MGbrowseITEM, CMD::ask, "index out of range"), "MGbrowseITEM past the end: nak");

        r = rpc({ask(rop::MGbrowseFIELD, {IDhistogram, 3}), ask(rop::MGbrowseFIELD, {OPloadTable, 0})});
        const rop::BrowseField hb {r[0].value<rop::BrowseField>()};
        check((36 == r[0].head.size) && (128 == hb.count) && (12 == hb.offset) && (0 == std::strcmp(hb.name, "bins")),
              "MGbrowseFIELD: Histogram::bins, 128 elements at offset 12");
        const rop::BrowseField lv {r[1].value<rop::BrowseField>()};
        check((100 == lv.count) && (0 == std::strcmp(lv.name, "values")), "MGbrowseFIELD: the argument of OPloadTable, 100 floats");
    }

    void bigvalues()
    {
        std::printf("big values\n");
        std::vector<Rx> r {rpc({ask(IDskinPATCH), ask(IDhistogram)})};
        check((2 == r.size()) && (CMD::say == r[0].cmd()) && (60 == r[0].data.size()) && (268 == r[1].data.size()),
              "ask: say of 60 and of 268 bytes");
        check((16 + 64 + 272) == lastsize, "the reply is 16 + (4 + 60) + (4 + 268) bytes");
        const udpdemo::types::Histogram h {r[1].value<udpdemo::types::Histogram>()};
        uint32_t sum {0};
        for(auto b : h.bins) { sum += b; }
        check((sum == h.samples) && (sum > 0), "the histogram is consistent");

        float table[100] {};
        for(int i = 0; i < 100; i++) { table[i] = 0.5f * i; }
        r = rpc({setack(IDtable, table, sizeof(table)), ask(IDtable)});
        check((2 == r.size()) && isack(r[0], IDtable, CMD::set, 0), "set+ack of 400 bytes: an ack w/out data");
        check((400 == r[1].data.size()) && (0 == std::memcmp(r[1].data.data(), table, 400)), "ask gives back the 400 bytes");

        r = rpc({setack(IDtable, table, 396)});
        check(isnak(r[0], IDtable, CMD::set, "wrong size"), "set+ack of 396 bytes: nak wrong size");
        r = rpc({setack(IDskinPATCH, table, 60)});
        check(isnak(r[0], IDskinPATCH, CMD::set, "read only"), "set+ack of a RO variable: nak read only");

        float values[100] {};
        for(int i = 0; i < 100; i++) { values[i] = static_cast<float>(i + 1); }
        r = rpc({runack(OPloadTable, values, sizeof(values)), ask(IDtable)});
        check((2 == r.size()) && isack(r[0], OPloadTable, CMD::run, 4) && (5050.0f == r[0].value<float>()),
              "run+ack w/ an argument of 400 bytes: ack w/ its result, sum = 5050");
        check(0 == std::memcmp(r[1].data.data(), values, 400), "the operation has written IDtable");

        r = rpc({runack(OPloadTable, values, 40)});
        check(isnak(r[0], OPloadTable, CMD::run, "wrong size of the argument"), "run+ack w/ a wrong argument: nak");

        const int16_t ops[2] {17, 0};
        r = rpc({runack(OPdivide, ops, sizeof(ops)), runack(OPgetClock, nullptr, 0)});
        check(isnak(r[0], OPdivide, CMD::run, "division by zero"), "the text of the operation in the nak");
        check(isack(r[1], OPgetClock, CMD::run, 16), "a struct result of 16 bytes in the ack");

        const uint32_t ticks {7};
        r = rpc({Request {Head::request(CMD::set, OPTnone, IDticks, 4), std::vector<uint8_t>(4, 0)}, setack(IDticks, &ticks, 4)});
        check((1 == r.size()) && isack(r[0], IDticks, CMD::set, 0), "set w/out ack: no reply. set+ack: ack");

        r = rpc({Request {Head::request(CMD::run, OPTack, IDticks, 0), {}}, Request {Head::request(CMD::ping, OPTnone, 0x33, 0), {}}});
        check(isnak(r[0], IDticks, CMD::run, "command not allowed for this id") && isack(r[1], 0x33, CMD::ping, 0), "run on a variable: nak. ping: ack");
    }

    void room()
    {
        std::printf("the room of the reply\n");
        // four ask of 400 bytes: 16 + 3 * 404 = 1228, the fourth does not fit 1472 -> nak<"no room">
        std::vector<Rx> r {rpc({ask(IDtable), ask(IDtable), ask(IDtable), ask(IDtable)})};
        check((4 == r.size()) && (CMD::say == r[2].cmd()) && isnak(r[3], IDtable, CMD::ask, "no room"), "the fourth ask of 400 bytes: nak no room");

        // fill the reply up to 40 bytes from the end: 16 + 3 * 404 + 64 + 7 * 20 = 1432
        float gain {0.0f};
        vars->read(udpdemo::vars::vGain, gain);
        bool flag {false};
        vars->read(udpdemo::vars::vFlag, flag);
        const uint32_t dropped {rop::theRPCmanager::getInstance().stats().droppedROPs};
        const float newgain {gain + 1.0f};
        const bool newflag {!flag};
        std::vector<Request> q {ask(IDtable), ask(IDtable), ask(IDtable), ask(IDskinPATCH)};
        for(int i = 0; i < 7; i++) { q.push_back(ask(IDechoSTATS)); }
        q.push_back(setack(IDgain, &newgain, 4));               // 40 bytes left: executed, ack of 4 bytes
        q.push_back(setack(IDflag, &newflag, 1));               // 36 left: not executed, nak no room (12)
        q.push_back(runack(OPechoRESET, nullptr, 0));           // 24 left: not executed, nak no room
        q.push_back(Request {Head::request(CMD::ping, OPTnone, 1, 0), {}});    // 12 left: a ping has no effects, its ack fits
        q.push_back(ask(IDticks));                              // 8 left: not even a nak fits, dropped
        r = rpc(q);
        float g {0.0f};
        vars->read(udpdemo::vars::vGain, g);
        bool f {false};
        vars->read(udpdemo::vars::vFlag, f);
        check((15 == r.size()) && (1464 == lastsize), "15 replies in 1464 bytes");
        check(isack(r[11], IDgain, CMD::set, 0) && (newgain == g), "set+ack w/ 40 bytes of room: executed");
        check(isnak(r[12], IDflag, CMD::set, "no room") && (flag == f), "set+ack w/ 36 bytes of room: not executed, nak no room");
        check(isnak(r[13], OPechoRESET, CMD::run, "no room") && isack(r[14], 1, CMD::ping, 0), "run: nak no room. ping: its ack still fits");
        check((dropped + 1) == rop::theRPCmanager::getInstance().stats().droppedROPs, "the last one is dropped and counted");
    }

    void malformed()
    {
        std::printf("malformed frames\n");
        const uint32_t wrong {rop::theRPCmanager::getInstance().stats().rxwrongframes};
        std::vector<uint8_t> f {build({ask(IDticks), ask(IDgain)})};
        f.pop_back();
        check(rpcraw(f).empty(), "a frame one byte short: no reply, nothing executed");
        f = build({ask(IDticks)});
        f[4] = 8;
        check(rpcraw(f).empty(), "a frame of version 5 (layout 8): no reply");
        std::vector<uint8_t> big(1473, 0);
        check(rpcraw(big).empty(), "a frame of 1473 bytes: no reply");
        check((wrong + 3) == rop::theRPCmanager::getInstance().stats().rxwrongframes, "counted in rxwrongframes");
    }

    void stream()
    {
        std::printf("stream (frames of 600 bytes)\n");
        rop::theSTRmanager &str {rop::theSTRmanager::getInstance()};
        const uint8_t a {IDtable};
        const uint8_t b {IDhistogram};
        const uint8_t c {IDskinPATCH};
        const uint8_t d {IDclock};
        std::vector<Rx> r {rpc({setack(rop::MGstreamADD, &a, 1), setack(rop::MGstreamADD, &b, 1), setack(rop::MGstreamADD, &c, 1),
                                setack(rop::MGstreamADD, &d, 1)})};
        // 16 + 404 = 420; + 272 = 692 > 600: refused; + 64 = 484; + 20 = 504
        check(isack(r[0], rop::MGstreamADD, CMD::set, 0) && isnak(r[1], rop::MGstreamADD, CMD::set, "unknown variable, or stream full") &&
              isack(r[2], rop::MGstreamADD, CMD::set, 0) && isack(r[3], rop::MGstreamADD, CMD::set, 0),
              "MGstreamADD: the byte budget refuses IDhistogram, the others fit");

        const uint16_t period {50};
        r = rpc({setack(rop::MGstreamSTART, &period, 2), ask(rop::MGstreamSTART), ask(rop::MGstreamLIST)});
        check(isack(r[0], rop::MGstreamSTART, CMD::set, 0) && (50 == r[1].value<uint16_t>()), "MGstreamSTART: empty ack, the period w/ ask");
        const rop::StreamList l {r[2].value<rop::StreamList>()};
        check((3 == l.count) && (IDtable == l.ids[0]) && (IDskinPATCH == l.ids[1]) && (IDclock == l.ids[2]), "MGstreamLIST");

        embot::core::testclock() = 123456;
        const void *p {nullptr};
        size_t s {0};
        rop::theSTRmanager::Destination dst {};
        check(str.get(p, s, dst) && (504 == s) && (7777 == dst.port) && (host.value() == dst.ip.value()), "a frame of 504 bytes to the host");
        r = parse(p, s);
        float table[100] {};
        vars->read(udpdemo::vars::vTable, table);
        check((3 == r.size()) && (CMD::sig == r[0].cmd()) && (400 == r[0].data.size()) && (0 == std::memcmp(r[0].data.data(), table, 400)) &&
              (60 == r[1].data.size()) && (16 == r[2].data.size()), "sig<IDtable> w/ its 400 bytes, sig<IDskinPATCH>, sig<IDclock>");
        rop::FrameHeader h {};
        std::memcpy(&h, p, 16);
        check((123456 == h.timeinmicroseconds) && (0 == h.layout), "the header: time, layout 0");

        const uint8_t rem {IDskinPATCH};
        rpc({setack(rop::MGstreamREM, &rem, 1)});
        check(str.get(p, s, dst) && (16 + 404 + 20 == s), "after MGstreamREM: the frame is rebuilt, 440 bytes");

        const std::vector<uint8_t> ping {build({Request {Head::request(CMD::ping, OPTnone, 0, 0), {}}, ask(IDtable)})};
        const void *reply {nullptr};
        size_t rs {0};
        check(str.onrx(ping.data(), ping.size(), host, reply, rs) && (20 == rs), "stream socket: ping -> a frame w/ one ack of 4 bytes");
        r = parse(reply, rs);
        check((1 == r.size()) && isack(r[0], 0, CMD::ping, 0), "ack<ping>, the ask is discarded");
    }

}

int main()
{
    setup();
    management();
    bigvalues();
    room();
    malformed();
    stream();
    std::printf("%d/%d TETHER checks passed\n", passed, passed + failed);
    return (0 == failed) ? 0 : 1;
}
