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

// a frame w/ the given ROPs, all w/ the same slot (the biggest)
static std::vector<uint8_t> frame(const std::vector<ROP> &rops, uint64_t t)
{
    size_t slot {ROP::minsizeofROP};
    for(const auto &r : rops) { size_t s {ROP::slotsize(r.size)}; if(s > slot) slot = s; }
    std::vector<uint8_t> f(sizeof(FrameHeader) + slot * rops.size(), 0);
    FrameHeader h {static_cast<uint8_t>(slot), static_cast<uint8_t>(rops.size()), t};
    std::memcpy(f.data(), &h, sizeof(h));
    for(size_t n = 0; n < rops.size(); n++) std::memcpy(f.data() + sizeof(h) + n * slot, &rops[n], slot);
    return f;
}
template<typename T> ROP rop(ROP::CMD c, uint8_t id, const T &v, uint8_t opt = ROP::OPTnone) { return ROP(c, opt, id, sizeof(T), &v, ROP::CMD::none); }

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
        auto f = frame({rop(ROP::CMD::set, gateway::ids::cubeMIT, m)}, 0);
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
        ROP r(ROP::CMD::run, ROP::OPTack, id, size, arg, ROP::CMD::none);
        auto f = frame({r}, t);
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
        auto f = frame({rop(ROP::CMD::set, gateway::ids::cubeMIT, mit), rop(ROP::CMD::set, gateway::ids::motorTARGET, tg)}, t);
        gateway::oncommands(f.data(), f.size(), t);
    }
    check("500 targets used, the MIT too", 500 == motdriver::status().targets && 500 == cubedriver::mits);
    const mot::MOTORstate s1 {motdriver::state()};
    check("emulation: velocity near 90 deg/s, mode 0x0A", s1.velocity > 89.0f && s1.velocity < 90.01f && ControlMode::speedvoltage == s1.controlmode && 3 == s1.configured);
    // 4. a target for another mode is rejected
    {
        const mot::MOTORtarget tg {1.0f, ControlMode::current, 1};
        auto f = frame({rop(ROP::CMD::set, gateway::ids::motorTARGET, tg)}, t);
        gateway::oncommands(f.data(), f.size(), t);
        check("target of another mode: rejected, heartbeat to the cube", 1 == motdriver::status().rejected && 1 == cubedriver::hbs);
    }
    // 5. the state frame carries the new MOTORstate
    uint8_t st[gateway::statesize];
    size_t size {0};
    posdriver::init();
    posdriver::tick(2500000);                       // 2.5 s of the 10 s sine: +30 deg
    gateway::buildstate(st, sizeof(st), size, t, {}, {}, {}, motdriver::state(), posdriver::state());
    ROP r3 {};
    ROP r5 {};
    FrameHeader h {};
    checkframe(st, size, h);
    extract(st, h, 3, r3);
    extract(st, h, 5, r5);
    check("state frame: sig<IDmotorSTATE> of 28 bytes", gateway::ids::motorSTATE == r3.id && 28 == r3.size && r3.value<mot::MOTORstate>().velocity == s1.velocity);
    const pos::POSstate p5 {r5.value<pos::POSstate>()};
    check("state frame: 6 slots, sig<IDposSTATE> of 16 bytes, emulated +30 deg", 6 == h.numOfROPs && 256 == size &&
          gateway::ids::posSTATE == r5.id && 16 == r5.size && p5.position > 29.99f && p5.position < 30.01f && 1 == p5.counter &&
          (pos::flags::valid | pos::flags::emulated) == p5.flags);
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
        auto f = frame({rop(ROP::CMD::set, gateway::ids::cubeMIT, mf), rop(ROP::CMD::set, gateway::ids::motorTARGET, tg)}, t + 301000);
        gateway::oncommands(f.data(), f.size(), t + 301000);
        uint8_t sf[gateway::statesize];
        size_t n {0};
        gateway::buildstate(sf, sizeof(sf), n, t, {}, {}, {}, motdriver::state(), posdriver::state());
        FrameHeader hf {};
        ROP rm {};
        checkframe(sf, n, hf);
        extract(sf, hf, 3, rm);
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
