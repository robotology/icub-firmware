// host test of amcmj1_gwlink and amcmj1_motdriver: the rules of the Speedgoat link and of the macromotor, w/out the board
#include "amcmj1_gwlink.h"
#include "amcmj1_motdriver.h"
#include "amcmj1_posdriver.h"
#include "embot_app_eth_ROP.h"
#include <cstdio>
#include <cstring>
#include <vector>
#include <limits>
namespace amcmj1::cubedriver {
    int mits {0}, hbs {0};
    Result command(const amcmj1::cube::CUBEmit &, embot::core::Time) { mits++; return Result::sent; }
    bool heartbeat(embot::core::Time) { hbs++; return true; }
}
using namespace embot::app::eth::rop;
using namespace amcmj1;
static int fails = 0, total = 0;
static void check(const char *n, bool c) { total++; if(!c) fails++; std::printf("  %s  %s\n", c ? "PASS" : "FAIL", n); }

// a ROP to send: its head and its data
struct Out { Head head {}; std::vector<uint8_t> data {}; };
// a frame of protocol 6 w/ the given ROPs
static std::vector<uint8_t> frame(const std::vector<Out> &rops, uint64_t t)
{
    std::vector<uint8_t> f(maxsizeofframe, 0);
    Writer w {f.data(), f.size()};
    for(const auto &r : rops) w.add(r.head, r.data.empty() ? nullptr : r.data.data());
    f.resize(w.close(t));
    return f;
}
static Out bytes(CMD c, uint8_t id, const void *v, size_t s, uint8_t opt)
{
    const uint8_t *p {static_cast<const uint8_t *>(v)};
    return Out {Head::request(c, opt, id, static_cast<uint16_t>(s)), std::vector<uint8_t>(p, p + s)};
}
template<typename T> Out rop(CMD c, uint8_t id, const T &v, uint8_t opt = OPTnone) { return bytes(c, id, &v, sizeof(T), opt); }
// the ROPs of a frame. empty if the frame is not valid. the views point into f
static std::vector<View> views(const uint8_t *f, size_t n)
{
    std::vector<View> out;
    Reader r {f, n, maxsizeofframe};
    View v {};
    while(r.next(v)) out.push_back(v);
    return out;
}

// --golden: the state frame that host/speedgoat/simulink/tests/golden.txt keeps as state_cpp (make_golden.py --state-cpp)
static int golden()
{
    gateway::resetstats();
    const cube::CUBEmit m10 {0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 10};
    const cube::CUBEmit m13 {0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 13};
    cube::CUBEmit bad {0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 14};
    bad.vel = std::numeric_limits<float>::quiet_NaN();
    for(const auto &m : {m10, m13, bad})
    {
        auto f = frame({rop(CMD::set, gateway::ids::cubeMIT, m)}, 0);
        gateway::oncommands(f.data(), f.size(), 0);
    }
    cube::CUBEstatus cs {};
    cs.state = cube::State::running;
    cs.watchdogs = 1;
    cube::CUBEstate ce {};
    ce.pos = 1.5f;
    ce.counter = 7;
    gateway::PAXtorque pt {};
    pt.torque[0] = 1.0f; pt.torque[1] = -2.0f; pt.torque[2] = 3.0f;
    pt.counter = 9;
    mot::MOTORstate ms {};
    ms.current = 0.5f; ms.velocity = 90.0f; ms.position = 45.0f; ms.pwm = 12.5f; ms.faults = mot::faults::externalfault;
    ms.counter = 4; ms.controlmode = mot::ControlMode::speedvoltage; ms.configured = mot::configuredALL; ms.emulated = 1;
    pos::POSstate ps {};
    ps.position = -12.5f; ps.counter = 1000; ps.errors = 2; ps.raw = 0xF71C; ps.flags = pos::flags::valid | pos::flags::emulated;
    uint8_t st[gateway::statesize];
    size_t size {0};
    gateway::buildstate(st, sizeof(st), size, 777, cs, ce, pt, ms, ps);
    std::printf("state_cpp ");
    for(size_t i = 0; i < size; i++) std::printf("%02x", st[i]);
    std::printf("\n");
    return 0;
}

int main(int argc, char **argv)
{
    if((argc > 1) && (0 == std::strcmp(argv[1], "--golden")))
    {
        return golden();
    }
    using mot::ControlMode;
    embot::core::Time t {1000000};
    auto run = [&](uint8_t id, const void *arg, uint8_t size) {
        auto f = frame({bytes(CMD::run, id, arg, size, OPTack)}, t);
        return gateway::onrpc(f.data(), f.size());
    };
    // 1. a mode before the config is refused
    uint8_t m {0x0A};
    gateway::Rpc r {run(gateway::ids::OPmotorMODE, &m, 1)};
    check("rpc OPmotorMODE parsed", gateway::Request::motormode == r.request && ControlMode::speedvoltage == r.mode);
    check("mode w/out config: notconfigured", motdriver::Result::notconfigured == motdriver::setmode(r.mode, t));
    // 2. pids and setup
    const mot::MOTORpids pids {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
    r = run(gateway::ids::OPmotorPIDS, &pids, sizeof(pids));
    check("rpc OPmotorPIDS parsed", gateway::Request::motorpids == r.request && 6.0f == r.pids.velkd);
    check("pids accepted", motdriver::Result::ok == motdriver::setpids(r.pids));
    const mot::MOTORsetup setup {1.0f, 2.0f, 3.0f, mot::flags::hasQE | mot::flags::hasHALL, 8, 1, 4096, -100};
    r = run(gateway::ids::OPmotorSETUP, &setup, sizeof(setup));
    check("rpc OPmotorSETUP parsed", gateway::Request::motorsetup == r.request && 4096 == r.setup.rotencres && -100 == r.setup.rotindexoff && 8 == r.setup.poles);
    check("setup accepted", motdriver::Result::ok == motdriver::setsetup(r.setup));
    r = run(gateway::ids::OPmotorPIDS, &m, 1);
    check("rpc w/ the wrong size: badargument", gateway::Request::badargument == r.request);
    // 3. the mode, then the targets in the command frames, w/ the MIT too
    check("mode speedvoltage accepted", motdriver::Result::ok == motdriver::setmode(ControlMode::speedvoltage, t));
    check("pids refused while not idle", motdriver::Result::notidle == motdriver::setpids(pids));
    const cube::CUBEmit mit {0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 7};
    for(int k = 0; k < 500; k++)
    {
        t += 1000;
        const mot::MOTORtarget tg {90.0f, ControlMode::speedvoltage, static_cast<uint32_t>(k)};
        auto f = frame({rop(CMD::set, gateway::ids::cubeMIT, mit), rop(CMD::set, gateway::ids::motorTARGET, tg)}, t);
        gateway::oncommands(f.data(), f.size(), t);
    }
    check("the command frame w/ MIT and target: 16 + 28 + 16 = 60 bytes",
          60 == frame({rop(CMD::set, gateway::ids::cubeMIT, mit), rop(CMD::set, gateway::ids::motorTARGET, mot::MOTORtarget {})}, t).size()
          && 44 == frame({rop(CMD::set, gateway::ids::cubeMIT, mit)}, t).size());
    check("500 targets used, the MIT too", 500 == motdriver::status().targets && 500 == cubedriver::mits);
    const mot::MOTORstate s1 {motdriver::state()};
    check("emulation: velocity near 90 deg/s, mode 0x0A", s1.velocity > 89.0f && s1.velocity < 90.01f && ControlMode::speedvoltage == s1.controlmode && 3 == s1.configured);
    // 4. a target for another mode is rejected
    {
        const mot::MOTORtarget tg {1.0f, ControlMode::current, 1};
        auto f = frame({rop(CMD::set, gateway::ids::motorTARGET, tg)}, t);
        gateway::oncommands(f.data(), f.size(), t);
        check("target of another mode: rejected, heartbeat to the cube", 1 == motdriver::status().rejected && 1 == cubedriver::hbs);
    }
    // 5. the state frame carries the new MOTORstate
    uint8_t st[gateway::statesize];
    size_t size {0};
    posdriver::init();
    posdriver::tick(2500000);                       // 2.5 s of the 10 s sine: +30 deg
    gateway::buildstate(st, sizeof(st), size, t, {}, {}, {}, motdriver::state(), posdriver::state());
    const std::vector<View> sv {views(st, size)};
    const View r3 {(6 == sv.size()) ? sv[3] : View {}};
    const View r5 {(6 == sv.size()) ? sv[5] : View {}};
    check("state frame: sig<IDmotorSTATE> of 28 bytes", CMD::sig == r3.cmd() && gateway::ids::motorSTATE == r3.id() && 28 == r3.size() &&
          r3.value<mot::MOTORstate>().velocity == s1.velocity);
    const pos::POSstate p5 {r5.value<pos::POSstate>()};
    check("state frame: 6 ROPs in 168 bytes, sig<IDposSTATE> of 16 bytes, emulated +30 deg", 6 == sv.size() && 168 == size &&
          gateway::ids::posSTATE == r5.id() && 16 == r5.size() && p5.position > 29.99f && p5.position < 30.01f && 1 == p5.counter &&
          (pos::flags::valid | pos::flags::emulated) == p5.flags);
    check("state frame: the sizes 12, 20, 16, 28, 36, 16", 12 == sv[0].size() && 20 == sv[1].size() && 16 == sv[2].size() &&
          36 == sv[4].size() && gateway::ids::gwSTATS == sv[4].id());
    check("state frame: too small a buffer is refused", false == gateway::buildstate(st, gateway::statesize - 1, size, t, {}, {}, {}, {}, {}));
    // the replies of the rpc: a light ack, a nak w/ its text
    {
        gateway::Rpc q {};
        q.request = gateway::Request::enable; q.ackrequired = true; q.id = gateway::ids::OPcubeENABLE;
        uint8_t rb[64];
        size_t rs {0};
        gateway::buildreply(rb, sizeof(rb), rs, t, q, true, nullptr);
        const std::vector<View> a {views(rb, rs)};
        check("rpc reply: ack<OPcubeENABLE, run> w/out data, 20 bytes", 20 == rs && 1 == a.size() && CMD::ack == a[0].cmd() &&
              CMD::run == a[0].referredcmd() && 0 == a[0].size());
        gateway::buildreply(rb, sizeof(rb), rs, t, q, false, "not idle");
        const std::vector<View> n {views(rb, rs)};
        check("rpc reply: nak<OPcubeENABLE, run, \"not idle\">", 1 == n.size() && CMD::nak == n[0].cmd() && 8 == n[0].size() &&
              0 == std::memcmp(n[0].data, "not idle", 8));
        auto bad = frame({rop(CMD::set, gateway::ids::cubeMIT, mit)}, t);
        bad.pop_back();
        const uint32_t b0 {gateway::stats().badframes};
        check("a command frame one byte short: bad, counted", gateway::Frame::bad == gateway::oncommands(bad.data(), bad.size(), t) &&
              b0 + 1 == gateway::stats().badframes);
        bad = frame({rop(CMD::set, gateway::ids::cubeMIT, mit)}, t);
        bad[4] = 32;
        check("a command frame of version 5: bad", gateway::Frame::bad == gateway::oncommands(bad.data(), bad.size(), t));
    }
    // 6. the watchdog
    motdriver::tick(t + 50000);
    check("no watchdog after 50 ms", ControlMode::speedvoltage == motdriver::state().controlmode);
    motdriver::tick(t + 150000);
    check("watchdog after 150 ms: idle", ControlMode::idle == motdriver::state().controlmode && 1 == motdriver::status().watchdogs);
    check("hwfault cannot be asked", motdriver::Result::badvalue == motdriver::setmode(ControlMode::hwfault, t));
    // 7. openloop clamps, current clamps to peak
    motdriver::setmode(ControlMode::openloop, t + 200000);
    motdriver::target(mot::MOTORtarget {150.0f, ControlMode::openloop, 1}, t + 201000);
    check("openloop clamped to 100 %", 100.0f == motdriver::lasttarget().value);
    motdriver::setmode(ControlMode::current, t + 202000);
    motdriver::target(mot::MOTORtarget {-5.0f, ControlMode::current, 2}, t + 203000);
    check("current clamped to -peak", -2.0f == motdriver::lasttarget().value);
    // 8. OPgwFAULT: held by the gateway, no MIT and no target used, the state frame tells it
    {
        uint8_t on {1};
        r = run(gateway::ids::OPgwFAULT, &on, 1);
        check("rpc OPgwFAULT 1 parsed", gateway::Request::gwfault == r.request && true == r.fault);
        gateway::setfault(r.fault);
        const int mits0 {cubedriver::mits};
        const gateway::GWstats g0 {gateway::stats()};
        const uint32_t tg0 {motdriver::status().targets};
        motdriver::setmode(ControlMode::current, t + 300000);
        const mot::MOTORtarget tg {0.5f, ControlMode::current, 99};
        const cube::CUBEmit mf {0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 900};
        auto f = frame({rop(CMD::set, gateway::ids::cubeMIT, mf), rop(CMD::set, gateway::ids::motorTARGET, tg)}, t + 301000);
        gateway::oncommands(f.data(), f.size(), t + 301000);
        uint8_t sf[gateway::statesize];
        size_t n {0};
        gateway::buildstate(sf, sizeof(sf), n, t, {}, {}, {}, motdriver::state(), posdriver::state());
        const std::vector<View> vf {views(sf, n)};
        const View rm {(6 == vf.size()) ? vf[3] : View {}};
        check("EXTFAULT: MIT and target not used, counted; MOTORstate::gwfault 1", mits0 == cubedriver::mits
              && tg0 == motdriver::status().targets && g0.notrunning + 1 == gateway::stats().notrunning
              && g0.rejected + 1 == gateway::stats().rejected && 1 == rm.value<mot::MOTORstate>().gwfault);
        uint8_t off {0};
        r = run(gateway::ids::OPgwFAULT, &off, 1);
        gateway::setfault(r.fault);
        gateway::oncommands(f.data(), f.size(), t + 302000);
        check("OPgwFAULT 0: the MIT and the target used again", false == gateway::fault() && mits0 + 1 == cubedriver::mits
              && tg0 + 1 == motdriver::status().targets);
    }
    std::printf("%d/%d gateway checks passed\n", total - fails, total);
    return fails;
}
