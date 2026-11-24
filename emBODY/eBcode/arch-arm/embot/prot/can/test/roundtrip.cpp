/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// roundtrip.cpp
// It verifies the new API of the PR (Sender / Receiver of motor polling and motor periodic):
// for every message it forms a frame from a known value (set<val>), parses it back (v = get<>) and checks that v == val.
// For the GET messages it does that for both the request and the reply.
// It also checks that the frames formed by the new API are identical to those formed by the legacy load() / reply() / get().
// It prints one line per check and a summary. The exit code is the number of failed checks.

#include "embot_prot_can.h"
#include "embot_prot_can_motor_polling.h"
#include "embot_prot_can_motor_periodic.h"

#include <cstdio>
#include <cstring>
#include <cmath>

using namespace embot::prot::can;
namespace mp = embot::prot::can::motor::polling;
namespace mpe = embot::prot::can::motor::periodic;
namespace mo = embot::prot::can::motor;

namespace {

    int nchecks {0};
    int nfailures {0};

    void check(const char *name, bool ok)
    {
        nchecks++;
        if(!ok) { nfailures++; }
        std::printf("%-58s %s\n", name, ok ? "OK" : "FAILED");
    }

    bool sameframe(const Frame &a, const Frame &b)
    {
        return (a.id == b.id) && (a.size == b.size) && (0 == std::memcmp(a.data, b.data, a.size));
    }

    constexpr Address tx {0};   // the address of who sends the request (e.g., the ETH board)
    constexpr Address rx {3};   // the address of who receives it (e.g., the motor board)

    bool eq(const mo::PID &a, const mo::PID &b) { return (a.kp == b.kp) && (a.ki == b.ki) && (a.kd == b.kd) && (a.ks == b.ks); }
    bool eq(const mo::PIDlimits &a, const mo::PIDlimits &b) { return (a.offset == b.offset) && (a.limitonoutput == b.limitonoutput) && (a.limitonintegral == b.limitonintegral); }
    bool eq(const mo::CurrentLimits &a, const mo::CurrentLimits &b) { return (a.x == b.x) && (a.nominal == b.nominal) && (a.peak == b.peak) && (a.overload == b.overload); }
    bool eq(const mo::MotorConfig &a, const mo::MotorConfig &b)
    {
        return (a.flags == b.flags) && (a.rotorEncoderResolution == b.rotorEncoderResolution) && (a.rotorIndexOffset == b.rotorIndexOffset) &&
               (a.motorPoles == b.motorPoles) && (a.rotEncTolerance == b.rotEncTolerance);
    }
    template<typename D> bool eqstream(const D &a, const D &b) { return 0 == std::memcmp(a.getstream(), b.getstream(), a.getsizeofstream()); }

    // a SET message: Sender::form() -> Receiver::parse(), and the legacy load() must decode the same frame in the same way
    template<typename M, typename EQ>
    void set(const char *name, const typename M::Info &val, EQ equal)
    {
        Frame f {};
        typename M::Info v {};
        char t[80];
        bool ok = M::Sender::form(f, val, rx, tx) && M::Receiver::parse(v, f) && equal(val, v);
        std::snprintf(t, sizeof(t), "%s: form -> parse", name); check(t, ok);
        M legacy {};
        ok = legacy.load(f) && equal(val, legacy.info);
        std::snprintf(t, sizeof(t), "%s: form -> legacy load()", name); check(t, ok);
    }

    // a GET message: request as above. then reply: Receiver::form() -> Sender::parse(), compared also w/ the legacy reply()
    template<typename M, typename EQ, typename EQR>
    void get(const char *name, const typename M::Info &req, const typename M::ReplyInfo &val, EQ equal, EQR equalreply)
    {
        set<M>(name, req, equal);
        Frame f {};
        typename M::ReplyInfo v {};
        char t[80];
        bool ok = M::Receiver::form(f, val, tx, rx) && M::Sender::parse(v, f) && equalreply(val, v);
        std::snprintf(t, sizeof(t), "%s: reply form -> parse", name); check(t, ok);
        // the legacy reply() needs a loaded request
        Frame fr {};
        M::Sender::form(fr, req, rx, tx);
        M legacy {};
        legacy.load(fr);
        Frame fl {};
        legacy.reply(fl, rx, val);
        std::snprintf(t, sizeof(t), "%s: reply form == legacy reply()", name); check(t, sameframe(f, fl));
    }

    // a periodic message: Sender::form() -> Receiver::parse(), and the frame must be equal to the one of the legacy get()
    template<typename M, typename EQ>
    void stream(const char *name, const typename M::Info &val, EQ equal)
    {
        Frame f {};
        typename M::Info v {};
        char t[80];
        bool ok = M::Sender::form(f, val, val.canaddress) && M::Receiver::parse(v, f) && equal(val, v);
        std::snprintf(t, sizeof(t), "%s: form -> parse", name); check(t, ok);
        M legacy {};
        legacy.load(val);
        Frame fl {};
        legacy.get(fl);
        std::snprintf(t, sizeof(t), "%s: form == legacy get()", name); check(t, sameframe(f, fl));
    }
}


void polling()
{
    for(mp::MotIndex mi : {mp::MotIndex::one, mp::MotIndex::two})
    {
        std::printf("\n### motor polling, MotIndex = %d\n", static_cast<int>(mi));

        auto eqmi = [](const auto &a, const auto &b) { return a.motorindex == b.motorindex; };

        for(mo::ControlMode cm : {mo::ControlMode::Idle, mo::ControlMode::Position, mo::ControlMode::ForceIdle, mo::ControlMode::Current,
                                  mo::ControlMode::SpeedVoltage, mo::ControlMode::SpeedCurrent, mo::ControlMode::OpenLoop,
                                  mo::ControlMode::NotConfigured, mo::ControlMode::HWfault})
        {
            auto eqcm = [](const auto &a, const auto &b) { return (a.motorindex == b.motorindex) && (a.controlmode == b.controlmode); };
            mp::Message_SET_CONTROL_MODE::Info si {}; si.motorindex = mi; si.controlmode = cm;
            set<mp::Message_SET_CONTROL_MODE>("SET_CONTROL_MODE", si, eqcm);
            mp::Message_GET_CONTROL_MODE::Info gi {}; gi.motorindex = mi;
            mp::Message_GET_CONTROL_MODE::ReplyInfo gr {}; gr.motorindex = mi; gr.controlmode = cm;
            get<mp::Message_GET_CONTROL_MODE>("GET_CONTROL_MODE", gi, gr, eqmi, eqcm);
        }

        mo::CurrentLimits cl {}; cl.x = 2; cl.nominal = 1500; cl.peak = 3000; cl.overload = -4000;
        { mp::Message_SET_CURRENT_LIMIT::Info si {}; si.motorindex = mi; si.currents = cl;
          set<mp::Message_SET_CURRENT_LIMIT>("SET_CURRENT_LIMIT", si, [](const auto &a, const auto &b) { return (a.motorindex == b.motorindex) && eq(a.currents, b.currents); });
          mp::Message_GET_CURRENT_LIMIT::Info gi {}; gi.motorindex = mi; mp::Message_GET_CURRENT_LIMIT::ReplyInfo gr {}; gr.motorindex = mi; gr.currents = cl;
          get<mp::Message_GET_CURRENT_LIMIT>("GET_CURRENT_LIMIT", gi, gr, eqmi, [](const auto &a, const auto &b) { return (a.motorindex == b.motorindex) && eq(a.currents, b.currents); }); }

        mo::PID pid {}; pid.kp = -1234; pid.ki = 56; pid.kd = -7; pid.ks = 9;
        auto eqpid = [](const auto &a, const auto &b) { return (a.motorindex == b.motorindex) && eq(a.pid, b.pid); };
        { mp::Message_SET_POS_PID::Info si {}; si.motorindex = mi; si.pid = pid; set<mp::Message_SET_POS_PID>("SET_POS_PID", si, eqpid);
          mp::Message_GET_POS_PID::Info gi {}; gi.motorindex = mi; mp::Message_GET_POS_PID::ReplyInfo gr {}; gr.motorindex = mi; gr.pid = pid;
          get<mp::Message_GET_POS_PID>("GET_POS_PID", gi, gr, eqmi, eqpid); }
        { mp::Message_SET_CURRENT_PID::Info si {}; si.motorindex = mi; si.pid = pid; set<mp::Message_SET_CURRENT_PID>("SET_CURRENT_PID", si, eqpid);
          mp::Message_GET_CURRENT_PID::Info gi {}; gi.motorindex = mi; mp::Message_GET_CURRENT_PID::ReplyInfo gr {}; gr.motorindex = mi; gr.pid = pid;
          get<mp::Message_GET_CURRENT_PID>("GET_CURRENT_PID", gi, gr, eqmi, eqpid); }
        { mp::Message_SET_VELOCITY_PID::Info si {}; si.motorindex = mi; si.pid = pid; set<mp::Message_SET_VELOCITY_PID>("SET_VELOCITY_PID", si, eqpid);
          mp::Message_GET_VELOCITY_PID::Info gi {}; gi.motorindex = mi; mp::Message_GET_VELOCITY_PID::ReplyInfo gr {}; gr.motorindex = mi; gr.pid = pid;
          get<mp::Message_GET_VELOCITY_PID>("GET_VELOCITY_PID", gi, gr, eqmi, eqpid); }

        mo::PIDlimits pl {}; pl.offset = -300; pl.limitonoutput = 2000; pl.limitonintegral = 1000;
        auto eqpl = [](const auto &a, const auto &b) { return (a.motorindex == b.motorindex) && eq(a.pidlimits, b.pidlimits); };
        { mp::Message_SET_POS_PIDLIMITS::Info si {}; si.motorindex = mi; si.pidlimits = pl; set<mp::Message_SET_POS_PIDLIMITS>("SET_POS_PIDLIMITS", si, eqpl);
          mp::Message_GET_POS_PIDLIMITS::Info gi {}; gi.motorindex = mi; mp::Message_GET_POS_PIDLIMITS::ReplyInfo gr {}; gr.motorindex = mi; gr.pidlimits = pl;
          get<mp::Message_GET_POS_PIDLIMITS>("GET_POS_PIDLIMITS", gi, gr, eqmi, eqpl); }
        { mp::Message_SET_CURRENT_PIDLIMITS::Info si {}; si.motorindex = mi; si.pidlimits = pl; set<mp::Message_SET_CURRENT_PIDLIMITS>("SET_CURRENT_PIDLIMITS", si, eqpl);
          mp::Message_GET_CURRENT_PIDLIMITS::Info gi {}; gi.motorindex = mi; mp::Message_GET_CURRENT_PIDLIMITS::ReplyInfo gr {}; gr.motorindex = mi; gr.pidlimits = pl;
          get<mp::Message_GET_CURRENT_PIDLIMITS>("GET_CURRENT_PIDLIMITS", gi, gr, eqmi, eqpl); }
        { mp::Message_SET_VELOCITY_PIDLIMITS::Info si {}; si.motorindex = mi; si.pidlimits = pl; set<mp::Message_SET_VELOCITY_PIDLIMITS>("SET_VELOCITY_PIDLIMITS", si, eqpl);
          mp::Message_GET_VELOCITY_PIDLIMITS::Info gi {}; gi.motorindex = mi; mp::Message_GET_VELOCITY_PIDLIMITS::ReplyInfo gr {}; gr.motorindex = mi; gr.pidlimits = pl;
          get<mp::Message_GET_VELOCITY_PIDLIMITS>("GET_VELOCITY_PIDLIMITS", gi, gr, eqmi, eqpl); }

        mo::MotorConfig mc {}; mc.flags = 0x3D; mc.rotorEncoderResolution = -4096; mc.rotorIndexOffset = 123; mc.motorPoles = 14; mc.rotEncTolerance = 7;
        auto eqmc = [](const auto &a, const auto &b) { return (a.motorindex == b.motorindex) && eq(a.config, b.config); };
        { mp::Message_SET_MOTOR_CONFIG::Info si {}; si.motorindex = mi; si.config = mc; set<mp::Message_SET_MOTOR_CONFIG>("SET_MOTOR_CONFIG", si, eqmc);
          mp::Message_GET_MOTOR_CONFIG::Info gi {}; gi.motorindex = mi; mp::Message_GET_MOTOR_CONFIG::ReplyInfo gr {}; gr.motorindex = mi; gr.config = mc;
          get<mp::Message_GET_MOTOR_CONFIG>("GET_MOTOR_CONFIG", gi, gr, eqmi, eqmc); }

        auto eqtl = [](const auto &a, const auto &b) { return (a.motorindex == b.motorindex) && (a.hardwarelimit == b.hardwarelimit); };
        { mp::Message_SET_TEMPERATURE_LIMIT::Info si {}; si.motorindex = mi; si.hardwarelimit = -77; set<mp::Message_SET_TEMPERATURE_LIMIT>("SET_TEMPERATURE_LIMIT", si, eqtl);
          mp::Message_GET_TEMPERATURE_LIMIT::Info gi {}; gi.motorindex = mi; mp::Message_GET_TEMPERATURE_LIMIT::ReplyInfo gr {}; gr.motorindex = mi; gr.hardwarelimit = 3500;
          get<mp::Message_GET_TEMPERATURE_LIMIT>("GET_TEMPERATURE_LIMIT", gi, gr, eqmi, eqtl); }

        // SET_PID / GET_PID w/ every type and param
        for(mo::pid::Type ty : {mo::pid::Type::CURR, mo::pid::Type::VEL, mo::pid::Type::POS, mo::pid::Type::VEL_CURR})
            for(mo::pid::Param pa : {mo::pid::Param::LOCK, mo::pid::Param::UNLOCK, mo::pid::Param::KP, mo::pid::Param::KI, mo::pid::Param::KD,
                                     mo::pid::Param::LIMINTEGRAL, mo::pid::Param::LIMOUTPUT, mo::pid::Param::KFF})
            {
                mo::pid::Descriptor d {ty, pa, -3.25f};
                auto eqd = [](const auto &a, const auto &b) { return (a.motorindex == b.motorindex) && eqstream(a.descriptor, b.descriptor) &&
                                                                     (a.descriptor.getValue() == b.descriptor.getValue()); };
                mp::Message_SET_PID::Info si {}; si.motorindex = mi; si.descriptor = d; set<mp::Message_SET_PID>("SET_PID", si, eqd);
                mp::Message_GET_PID::Info gi {}; gi.motorindex = mi; gi.type = ty; gi.param = pa;
                mp::Message_GET_PID::ReplyInfo gr {}; gr.motorindex = mi; gr.descriptor = d;
                get<mp::Message_GET_PID>("GET_PID", gi, gr, [](const auto &a, const auto &b) { return (a.motorindex == b.motorindex) && (a.type == b.type) && (a.param == b.param); }, eqd);
            }

        // SET_MOTOR_PARAM / GET_MOTOR_PARAM w/ every ID
        {
            mo::motorparam::Data<mo::motorparam::vBEMF> bemf {}; bemf.value().kbemf = 0.0125f;
            mo::motorparam::Data<mo::motorparam::vHALL> hall {}; hall.value().offset = -30; hall.value().swapmode = mo::motorparam::vHALL::SwapMode::bc;
            mo::motorparam::Data<mo::motorparam::vELECTR_VMAX> vmax {}; vmax.value().vmax = 48.0f;
            mo::motorparam::Data<mo::motorparam::vGENERIC> gen {}; for(int k=0; k<6; k++) { gen.value().generic[k] = 0xA0+k; }
            struct P { mo::motorparam::ID id; const std::uint8_t *data; };
            for(P p : {P{mo::motorparam::ID::GENERIC, gen.serialize()}, P{mo::motorparam::ID::BEMF, bemf.serialize()},
                       P{mo::motorparam::ID::HALL, hall.serialize()}, P{mo::motorparam::ID::ELECTR_VMAX, vmax.serialize()}})
            {
                mo::motorparam::Descriptor d {p.id, p.data};
                auto eqd = [](const auto &a, const auto &b) { return (a.motorindex == b.motorindex) && eqstream(a.descriptor, b.descriptor); };
                mp::Message_SET_MOTOR_PARAM::Info si {}; si.motorindex = mi; si.descriptor = d; set<mp::Message_SET_MOTOR_PARAM>("SET_MOTOR_PARAM", si, eqd);
                mp::Message_GET_MOTOR_PARAM::Info gi {}; gi.motorindex = mi; gi.id = p.id;
                mp::Message_GET_MOTOR_PARAM::ReplyInfo gr {}; gr.motorindex = mi; gr.descriptor = d;
                get<mp::Message_GET_MOTOR_PARAM>("GET_MOTOR_PARAM", gi, gr, [](const auto &a, const auto &b) { return (a.motorindex == b.motorindex) && (a.id == b.id); }, eqd);
            }
            // and the decoding of HALL: the offset is an int16_t
            mo::motorparam::Data<mo::motorparam::vHALL> back {mo::motorparam::Descriptor {mo::motorparam::ID::HALL, hall.serialize()}.getdata()};
            check("motorparam::vHALL offset -30 survives the round trip", (-30 == back.value().offset) && (mo::motorparam::vHALL::SwapMode::bc == back.value().swapmode));
        }

        // SET / GET w/ every ID
        {
            mo::generic::Data<mo::generic::vTargetPOS> tp {}; tp.value().position = -45.5f; tp.value().withvelocity = static_cast<_Float16>(10.0f);
            mo::generic::Data<mo::generic::vGENERIC> gen {}; for(int k=0; k<6; k++) { gen.value().generic[k] = 0xB0+k; }
            struct P { mo::generic::ID id; const std::uint8_t *data; };
            for(P p : {P{mo::generic::ID::GENERIC, gen.serialize()}, P{mo::generic::ID::TargetPOS, tp.serialize()}})
            {
                mo::generic::Descriptor d {p.id, p.data};
                auto eqd = [](const auto &a, const auto &b) { return (a.motorindex == b.motorindex) && eqstream(a.descriptor, b.descriptor); };
                mp::Message_SET::Info si {}; si.motorindex = mi; si.descriptor = d; set<mp::Message_SET>("SET", si, eqd);
                mp::Message_GET::Info gi {}; gi.motorindex = mi; gi.id = p.id;
                mp::Message_GET::ReplyInfo gr {}; gr.motorindex = mi; gr.descriptor = d;
                get<mp::Message_GET>("GET", gi, gr, [](const auto &a, const auto &b) { return (a.motorindex == b.motorindex) && (a.id == b.id); }, eqd);
            }
        }
    }

    std::printf("\n### motor polling, general management\n");
    {
        mp::Message_GET_FIRMWARE_VERSION::Info req {}; req.requiredprotocol.major = 2; req.requiredprotocol.minor = 0;
        Frame f {}; mp::Message_GET_FIRMWARE_VERSION::Info v {};
        check("GET_FIRMWARE_VERSION: form -> parse", mp::Message_GET_FIRMWARE_VERSION::Sender::form(f, req, rx, tx) && mp::Message_GET_FIRMWARE_VERSION::Receiver::parse(v, f) &&
                                                     (v.requiredprotocol.major == 2) && (v.requiredprotocol.minor == 0));
        mp::Message_GET_FIRMWARE_VERSION::ReplyInfo rep {}; rep.board = Board::amcbldc; rep.firmware.major = 1; rep.firmware.minor = 2; rep.firmware.build = 3; rep.protocol.major = 2; rep.protocol.minor = 1;
        Frame fr {}; mp::Message_GET_FIRMWARE_VERSION::ReplyInfo vr {}; bool compatible {false};
        check("GET_FIRMWARE_VERSION: reply form -> parse", mp::Message_GET_FIRMWARE_VERSION::Receiver::form(fr, rep, req, tx, rx) && mp::Message_GET_FIRMWARE_VERSION::Sender::parse(vr, compatible, fr) &&
                                                           (vr.board == rep.board) && (vr.firmware.major == 1) && (vr.firmware.minor == 2) && (vr.firmware.build == 3) &&
                                                           (vr.protocol.major == 2) && (vr.protocol.minor == 1) && compatible);
        mp::Message_GET_FIRMWARE_VERSION legacy {}; legacy.load(f); Frame fl {}; legacy.reply(fl, rx, rep);
        check("GET_FIRMWARE_VERSION: reply form == legacy reply()", sameframe(fr, fl));

        mp::Message_SET_BOARD_ID::Info si {}; si.address = 7; Frame fs {}; mp::Message_SET_BOARD_ID::Info vs {};
        check("SET_BOARD_ID: form -> parse", mp::Message_SET_BOARD_ID::Sender::form(fs, si, rx, tx) && mp::Message_SET_BOARD_ID::Receiver::parse(vs, fs) && (7 == vs.address));
    }
}


void periodic()
{
    std::printf("\n### motor periodic\n");

    { mpe::Message_FOC::Info i {}; i.canaddress = 3; i.current = -1234; i.velocity = -55; i.position = -100000;
      stream<mpe::Message_FOC>("FOC", i, [](const auto &a, const auto &b) { return (a.canaddress == b.canaddress) && (a.current == b.current) && (a.velocity == b.velocity) && (a.position == b.position); }); }
    { mpe::Message_STATUS::Info i {}; i.canaddress = 2; i.controlmode = 0x06; i.quadencoderstate = 0x05; i.pwmfeedback = -16000; i.motorfaultstate = 0xDEADBEEF;
      stream<mpe::Message_STATUS>("STATUS", i, [](const auto &a, const auto &b) { return (a.canaddress == b.canaddress) && (a.controlmode == b.controlmode) && (a.quadencoderstate == b.quadencoderstate) &&
                                                                                     (a.pwmfeedback == b.pwmfeedback) && (a.motorfaultstate == b.motorfaultstate); }); }
    { mpe::Message_ADDITIONAL_STATUS::Info i {}; i.canaddress = 1; i.tbd[0] = 7; i.tbd[1] = 8; i.temperature = -42;
      stream<mpe::Message_ADDITIONAL_STATUS>("ADDITIONAL_STATUS", i, [](const auto &a, const auto &b) { return (a.canaddress == b.canaddress) && (a.tbd[0] == b.tbd[0]) && (a.tbd[1] == b.tbd[1]) && (a.temperature == b.temperature); }); }
    { mpe::Message_DEBUG::Info i {}; i.canaddress = 5; i.payload = 0x1122334455667788ULL;
      stream<mpe::Message_DEBUG>("DEBUG", i, [](const auto &a, const auto &b) { return (a.canaddress == b.canaddress) && (a.payload == b.payload); }); }
    { mpe::Message_EMSTO2FOC_DESIRED_CURRENT::Info i {}; i.canaddress = 0; i.target[0] = 100; i.target[1] = -200; i.target[2] = 300; i.target[3] = -32000;
      Frame f {}; mpe::Message_EMSTO2FOC_DESIRED_CURRENT::Info v {};
      check("EMSTO2FOC_DESIRED_CURRENT: form -> parse", mpe::Message_EMSTO2FOC_DESIRED_CURRENT::Sender::form(f, i, 0) && mpe::Message_EMSTO2FOC_DESIRED_CURRENT::Receiver::parse(v, f) &&
                                                        (v.target[0] == 100) && (v.target[1] == -200) && (v.target[2] == 300) && (v.target[3] == -32000));
      mpe::Message_EMSTO2FOC_DESIRED_CURRENT legacy {}; check("EMSTO2FOC_DESIRED_CURRENT: form -> legacy load()", legacy.load(f) && (legacy.info.target[3] == -32000)); }

    // PRINT: every length from 1 to 30 chars, form all the frames, parse them and reassemble the text
    {
        bool allok {true};
        bool alleq {true};
        char text[31] {0};
        for(int len=1; len<=30; len++)
        {
            for(int k=0; k<len; k++) { text[k] = static_cast<char>('A' + (k % 26)); }
            text[len] = 0;
            mpe::Message_PRINT::Info i {}; i.canaddress = 4; std::memcpy(i.text, text, len+1);
            mpe::Message_PRINT legacy {}; legacy.load(i);
            mpe::Message_PRINT::Receiver::Assembler assembler {};
            mpe::Message_PRINT::Info v {};
            bool done {false};
            const std::uint8_t n = mpe::Message_PRINT::Sender::numberofframes(i);
            // the legacy load() increments its static textID before forming the frames, so we read it back from the first legacy frame
            Frame fl {}; legacy.get(fl);
            const std::uint8_t textID = fl.data[1] >> 4;
            for(std::uint8_t k=0; k<n; k++)
            {
                Frame f {};
                mpe::Message_PRINT::Fragment frag {};
                allok = allok && mpe::Message_PRINT::Sender::form(f, i, k, textID, i.canaddress) && mpe::Message_PRINT::Receiver::parse(frag, f);
                if(k > 0) { legacy.get(fl); }
                alleq = alleq && sameframe(f, fl);
                done = assembler.add(frag, v);
            }
            allok = allok && done && (0 == std::strcmp(v.text, text)) && (v.canaddress == 4);
        }
        check("PRINT: 1..30 chars, form -> parse -> Assembler", allok);
        check("PRINT: 1..30 chars, form == legacy get()", alleq);
    }
}


int main()
{
    polling();
    periodic();
    std::printf("\n%d checks, %d failed\n", nchecks, nfailures);
    return nfailures;
}
