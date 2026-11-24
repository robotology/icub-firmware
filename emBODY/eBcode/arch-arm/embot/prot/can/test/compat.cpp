/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// compat.cpp
// It uses only the API that exists in robotology/icub-firmware devel (legacy load() / reply() / get() and
// assignment of the fields of Info / ReplyInfo) to exercise every message of embot::prot::can.
// It prints the decoded fields of every received frame and the bytes of every formed frame.
// Build it once against robotology devel and once against the PR: the two outputs must be identical,
// apart from the intentional changes listed in the PR.

#include "embot_prot_can.h"
#include "embot_prot_can_motor_polling.h"
#include "embot_prot_can_motor_periodic.h"
#include "embot_prot_can_analog_polling.h"
#include "embot_prot_can_analog_periodic.h"
#include "embot_prot_can_inertial_periodic.h"
#include "embot_prot_can_skin_periodic.h"
#include "embot_prot_can_bootloader.h"

#include <cstdio>
#include <cstring>
#include <initializer_list>

using namespace embot::prot::can;

namespace {

    template<typename T> long long I(T v) { return static_cast<long long>(v); }

    void dump(const char *tag, const Frame &f)
    {
        std::printf("%-40s id=%03X size=%u data=", tag, static_cast<unsigned>(f.id), static_cast<unsigned>(f.size));
        // only the first size bytes: they are the ones that go on the bus
        for(int i=0; i<f.size; i++) { std::printf("%02X%s", f.data[i], (i<f.size-1) ? ":" : ""); }
        std::printf("\n");
    }

    // a frame from address 0 to address 'to' of a polling class: data[0] = (mcindex << 7) | cmd, then the bytes
    Frame mk(Clas c, std::uint8_t cmd, std::uint8_t to, std::initializer_list<std::uint8_t> d, std::uint8_t mcindex = 0)
    {
        Frame f {};
        std::uint8_t b[8] {};
        std::uint8_t n = 0;
        for(auto v : d) { b[n++] = v; }
        frame_set_sender(f, 0);
        frame_set_clascmddestinationdata(f, c, cmd, to, b, n, mcindex);
        frame_set_size(f, n+1);
        return f;
    }

    // a frame of class bootloader: data[0] = cmd, then the bytes
    Frame mkbl(std::uint8_t cmd, std::uint8_t to, std::initializer_list<std::uint8_t> d) { return mk(Clas::bootloader, cmd, to, d, 0); }

    constexpr std::uint8_t mcindexes[2] = {0, 1};
}


void motorpolling()
{
    namespace mp = embot::prot::can::motor::polling;
    namespace mo = embot::prot::can::motor;
    const Clas c = Clas::pollingMotorControl;
    Frame out {};
    char t[64];

    for(auto mi : mcindexes)
    {
        std::printf("\n### motor polling, mcindex = %d\n", mi);

        { mp::Message_SET_CONTROL_MODE m {}; bool r = m.load(mk(c, 9, 3, {0x50}, mi));
          std::printf("SET_CONTROL_MODE load=%d mi=%lld cm=0x%02llX\n", r, I(m.info.motorindex), I(m.info.controlmode)); }

        { mp::Message_GET_CONTROL_MODE m {}; bool r = m.load(mk(c, 7, 3, {}, mi));
          mp::Message_GET_CONTROL_MODE::ReplyInfo ri {}; ri.motorindex = m.info.motorindex; ri.controlmode = mo::ControlMode::Current;
          m.reply(out, 3, ri); std::snprintf(t, sizeof(t), "GET_CONTROL_MODE load=%d mi=%lld", r, I(m.info.motorindex)); dump(t, out); }

        { mp::Message_SET_CURRENT_LIMIT m {}; bool r = m.load(mk(c, 72, 3, {0x02, 0xDC, 0x05, 0xB8, 0x0B, 0x60, 0xF0}, mi));
          std::printf("SET_CURRENT_LIMIT load=%d mi=%lld x=%lld nom=%lld peak=%lld over=%lld\n", r, I(m.info.motorindex),
                      I(m.info.currents.x), I(m.info.currents.nominal), I(m.info.currents.peak), I(m.info.currents.overload)); }

        { mp::Message_GET_CURRENT_LIMIT m {}; bool r = m.load(mk(c, 124, 3, {}, mi));
          mp::Message_GET_CURRENT_LIMIT::ReplyInfo ri {}; ri.motorindex = m.info.motorindex;
          ri.currents.x = 2; ri.currents.nominal = 1500; ri.currents.peak = 3000; ri.currents.overload = -4000;
          m.reply(out, 3, ri); std::snprintf(t, sizeof(t), "GET_CURRENT_LIMIT load=%d", r); dump(t, out); }

        // the three PIDs: SET (cmd) and GET (cmd+1)
        struct PIDcmd { const char *name; std::uint8_t set; };
        for(auto p : {PIDcmd{"POS_PID", 82}, PIDcmd{"CURRENT_PID", 101}, PIDcmd{"VELOCITY_PID", 105}})
        {
            Frame f = mk(c, p.set, 3, {0x2E, 0xFB, 0x38, 0x00, 0xF9, 0xFF, 0x09}, mi);
            if(82 == p.set) { mp::Message_SET_POS_PID m {}; bool r = m.load(f); std::printf("SET_%s load=%d mi=%lld kp=%lld ki=%lld kd=%lld ks=%lld\n", p.name, r, I(m.info.motorindex), I(m.info.pid.kp), I(m.info.pid.ki), I(m.info.pid.kd), I(m.info.pid.ks)); }
            if(101 == p.set) { mp::Message_SET_CURRENT_PID m {}; bool r = m.load(f); std::printf("SET_%s load=%d mi=%lld kp=%lld ki=%lld kd=%lld ks=%lld\n", p.name, r, I(m.info.motorindex), I(m.info.pid.kp), I(m.info.pid.ki), I(m.info.pid.kd), I(m.info.pid.ks)); }
            if(105 == p.set) { mp::Message_SET_VELOCITY_PID m {}; bool r = m.load(f); std::printf("SET_%s load=%d mi=%lld kp=%lld ki=%lld kd=%lld ks=%lld\n", p.name, r, I(m.info.motorindex), I(m.info.pid.kp), I(m.info.pid.ki), I(m.info.pid.kd), I(m.info.pid.ks)); }
        }
        { mp::Message_GET_POS_PID m {}; bool r = m.load(mk(c, 83, 3, {}, mi)); mp::Message_GET_POS_PID::ReplyInfo ri {}; ri.motorindex = m.info.motorindex;
          ri.pid.kp = -1234; ri.pid.ki = 56; ri.pid.kd = -7; ri.pid.ks = 9; m.reply(out, 3, ri); std::snprintf(t, sizeof(t), "GET_POS_PID load=%d", r); dump(t, out); }
        { mp::Message_GET_CURRENT_PID m {}; bool r = m.load(mk(c, 102, 3, {}, mi)); mp::Message_GET_CURRENT_PID::ReplyInfo ri {}; ri.motorindex = m.info.motorindex;
          ri.pid.kp = -1234; ri.pid.ki = 56; ri.pid.kd = -7; ri.pid.ks = 9; m.reply(out, 3, ri); std::snprintf(t, sizeof(t), "GET_CURRENT_PID load=%d", r); dump(t, out); }
        { mp::Message_GET_VELOCITY_PID m {}; bool r = m.load(mk(c, 106, 3, {}, mi)); mp::Message_GET_VELOCITY_PID::ReplyInfo ri {}; ri.motorindex = m.info.motorindex;
          ri.pid.kp = -1234; ri.pid.ki = 56; ri.pid.kd = -7; ri.pid.ks = 9; m.reply(out, 3, ri); std::snprintf(t, sizeof(t), "GET_VELOCITY_PID load=%d", r); dump(t, out); }

        // the three PIDLIMITS
        { mp::Message_SET_POS_PIDLIMITS m {}; bool r = m.load(mk(c, 84, 3, {0xD4, 0xFE, 0xD0, 0x07, 0xE8, 0x03}, mi));
          std::printf("SET_POS_PIDLIMITS load=%d mi=%lld o=%lld lo=%lld li=%lld\n", r, I(m.info.motorindex), I(m.info.pidlimits.offset), I(m.info.pidlimits.limitonoutput), I(m.info.pidlimits.limitonintegral)); }
        { mp::Message_SET_CURRENT_PIDLIMITS m {}; bool r = m.load(mk(c, 103, 3, {0xD4, 0xFE, 0xD0, 0x07, 0xE8, 0x03}, mi));
          std::printf("SET_CURRENT_PIDLIMITS load=%d mi=%lld o=%lld lo=%lld li=%lld\n", r, I(m.info.motorindex), I(m.info.pidlimits.offset), I(m.info.pidlimits.limitonoutput), I(m.info.pidlimits.limitonintegral)); }
        { mp::Message_SET_VELOCITY_PIDLIMITS m {}; bool r = m.load(mk(c, 107, 3, {0xD4, 0xFE, 0xD0, 0x07, 0xE8, 0x03}, mi));
          std::printf("SET_VELOCITY_PIDLIMITS load=%d mi=%lld o=%lld lo=%lld li=%lld\n", r, I(m.info.motorindex), I(m.info.pidlimits.offset), I(m.info.pidlimits.limitonoutput), I(m.info.pidlimits.limitonintegral)); }
        { mp::Message_GET_POS_PIDLIMITS m {}; bool r = m.load(mk(c, 85, 3, {}, mi)); mp::Message_GET_POS_PIDLIMITS::ReplyInfo ri {}; ri.motorindex = m.info.motorindex;
          ri.pidlimits.offset = -300; ri.pidlimits.limitonoutput = 2000; ri.pidlimits.limitonintegral = 1000; m.reply(out, 3, ri); std::snprintf(t, sizeof(t), "GET_POS_PIDLIMITS load=%d", r); dump(t, out); }
        { mp::Message_GET_CURRENT_PIDLIMITS m {}; bool r = m.load(mk(c, 104, 3, {}, mi)); mp::Message_GET_CURRENT_PIDLIMITS::ReplyInfo ri {}; ri.motorindex = m.info.motorindex;
          ri.pidlimits.offset = -300; ri.pidlimits.limitonoutput = 2000; ri.pidlimits.limitonintegral = 1000; m.reply(out, 3, ri); std::snprintf(t, sizeof(t), "GET_CURRENT_PIDLIMITS load=%d", r); dump(t, out); }
        { mp::Message_GET_VELOCITY_PIDLIMITS m {}; bool r = m.load(mk(c, 108, 3, {}, mi)); mp::Message_GET_VELOCITY_PIDLIMITS::ReplyInfo ri {}; ri.motorindex = m.info.motorindex;
          ri.pidlimits.offset = -300; ri.pidlimits.limitonoutput = 2000; ri.pidlimits.limitonintegral = 1000; m.reply(out, 3, ri); std::snprintf(t, sizeof(t), "GET_VELOCITY_PIDLIMITS load=%d", r); dump(t, out); }

        { mp::Message_SET_MOTOR_CONFIG m {}; bool r = m.load(mk(c, 119, 3, {0x3D, 0x00, 0xF0, 0x7B, 0x00, 0x0E, 0x07}, mi));
          std::printf("SET_MOTOR_CONFIG load=%d mi=%lld flags=0x%02llX res=%lld off=%lld poles=%lld tol=%lld\n", r, I(m.info.motorindex), I(m.info.config.flags),
                      I(m.info.config.rotorEncoderResolution), I(m.info.config.rotorIndexOffset), I(m.info.config.motorPoles), I(m.info.config.rotEncTolerance)); }
        { mp::Message_GET_MOTOR_CONFIG m {}; bool r = m.load(mk(c, 123, 3, {}, mi)); mp::Message_GET_MOTOR_CONFIG::ReplyInfo ri {}; ri.motorindex = m.info.motorindex;
          ri.config.flags = 0x3D; ri.config.rotorEncoderResolution = -4096; ri.config.rotorIndexOffset = 123; ri.config.motorPoles = 14; ri.config.rotEncTolerance = 7;
          m.reply(out, 3, ri); std::snprintf(t, sizeof(t), "GET_MOTOR_CONFIG load=%d", r); dump(t, out); }

        { mp::Message_SET_TEMPERATURE_LIMIT m {}; bool r = m.load(mk(c, 121, 3, {0xB3, 0xFF}, mi));
          std::printf("SET_TEMPERATURE_LIMIT load=%d mi=%lld hw=%lld\n", r, I(m.info.motorindex), I(m.info.hardwarelimit)); }
        { mp::Message_GET_TEMPERATURE_LIMIT m {}; bool r = m.load(mk(c, 122, 3, {}, mi)); mp::Message_GET_TEMPERATURE_LIMIT::ReplyInfo ri {}; ri.motorindex = m.info.motorindex;
          ri.hardwarelimit = -77; m.reply(out, 3, ri); std::snprintf(t, sizeof(t), "GET_TEMPERATURE_LIMIT load=%d", r); dump(t, out); }

        // SET_PID / GET_PID: [type][param][ffu][float32]
        { mp::Message_SET_PID m {}; bool r = m.load(mk(c, 125, 3, {0x01, 0x02, 0x00, 0x33, 0x33, 0x53, 0x40}, mi));
          std::printf("SET_PID load=%d mi=%lld type=%lld param=%lld value=%f\n", r, I(m.info.motorindex), I(m.info.descriptor.getType()), I(m.info.descriptor.getParam()), m.info.descriptor.getValue()); }
        { mp::Message_GET_PID m {}; bool r = m.load(mk(c, 126, 3, {0x00, 0x07}, mi));
          std::printf("GET_PID load=%d mi=%lld type=%lld param=%lld\n", r, I(m.info.motorindex), I(m.info.type), I(m.info.param));
          mp::Message_GET_PID::ReplyInfo ri {}; ri.motorindex = m.info.motorindex; ri.descriptor.load(mo::pid::Type::CURR, mo::pid::Param::KFF, 3.3f);
          m.reply(out, 3, ri); dump("GET_PID reply", out); }

        // SET_MOTOR_PARAM / GET_MOTOR_PARAM and SET / GET: [id][6 bytes]
        { mp::Message_SET_MOTOR_PARAM m {}; bool r = m.load(mk(c, 1, 3, {0x02, 0xE2, 0xFF, 0x01, 0x00, 0x00, 0x00}, mi));
          const std::uint8_t *s = m.info.descriptor.getstream();
          std::printf("SET_MOTOR_PARAM load=%d mi=%lld stream=%02X %02X %02X %02X %02X %02X %02X\n", r, I(m.info.motorindex), s[0], s[1], s[2], s[3], s[4], s[5], s[6]); }
        { mp::Message_GET_MOTOR_PARAM m {}; bool r = m.load(mk(c, 2, 3, {0x01}, mi));
          std::printf("GET_MOTOR_PARAM load=%d mi=%lld id=%lld\n", r, I(m.info.motorindex), I(m.info.id));
          const std::uint8_t d[6] = {0x00, 0x00, 0x20, 0x40, 0x00, 0x00};
          mp::Message_GET_MOTOR_PARAM::ReplyInfo ri {}; ri.motorindex = m.info.motorindex; ri.descriptor.load(mo::motorparam::ID::BEMF, d);
          m.reply(out, 3, ri); dump("GET_MOTOR_PARAM reply", out); }
        { mp::Message_SET m {}; bool r = m.load(mk(c, 5, 3, {0x01, 0x00, 0x00, 0x40, 0x40, 0x00, 0x3C}, mi));
          const std::uint8_t *s = m.info.descriptor.getstream();
          std::printf("SET load=%d mi=%lld stream=%02X %02X %02X %02X %02X %02X %02X\n", r, I(m.info.motorindex), s[0], s[1], s[2], s[3], s[4], s[5], s[6]); }
        { mp::Message_GET m {}; bool r = m.load(mk(c, 6, 3, {0x01}, mi));
          std::printf("GET load=%d mi=%lld id=%lld\n", r, I(m.info.motorindex), I(m.info.id));
          const std::uint8_t d[6] = {0x00, 0x00, 0x40, 0x40, 0x00, 0x3C};
          mp::Message_GET::ReplyInfo ri {}; ri.motorindex = m.info.motorindex; ri.descriptor.load(mo::generic::ID::TargetPOS, d);
          m.reply(out, 3, ri); dump("GET reply", out); }
    }

    std::printf("\n### motor polling, general management\n");
    { mp::Message_GET_FIRMWARE_VERSION m {}; bool r = m.load(mk(c, 91, 3, {2, 0}));
      std::printf("GET_FIRMWARE_VERSION load=%d req=%lld.%lld\n", r, I(m.info.requiredprotocol.major), I(m.info.requiredprotocol.minor));
      mp::Message_GET_FIRMWARE_VERSION::ReplyInfo ri {}; ri.board = Board::amcbldc; ri.firmware.major = 1; ri.firmware.minor = 2; ri.firmware.build = 3; ri.protocol.major = 2; ri.protocol.minor = 0;
      m.reply(out, 3, ri); dump("GET_FIRMWARE_VERSION reply", out); }
    { mp::Message_SET_BOARD_ID m {}; bool r = m.load(mk(c, 50, 3, {7}));
      std::printf("SET_BOARD_ID load=%d address=%lld\n", r, I(m.info.address)); }
}


void converter()
{
    namespace mo = embot::prot::can::motor;
    using C = mo::Converter;
    std::printf("\n### motor Converter (values within range)\n");
    for(float a : {-12.5f, -0.001f, 0.0f, 0.75f, 30.0f}) { std::printf("to_can_current(%g)=%lld to_ampere=%g\n", a, I(C::to_can_current(a)), C::to_ampere(C::to_can_current(a))); }
    for(float d : {-179.9f, -1.0f, 0.0f, 45.5f, 179.9f}) { std::printf("to_can_position(%g)=%lld back=%g\n", d, I(C::to_can_position(d)), C::position_to_degree(C::to_can_position(d))); }
    for(float d : {-10000.0f, -1.0f, 0.0f, 360.0f, 10000.0f}) { std::printf("to_can_fullposition(%g)=%lld back=%g\n", d, I(C::to_can_fullposition(d)), C::fullposition_to_degree(C::to_can_fullposition(d))); }
    for(float v : {-1000.0f, -1.0f, 0.0f, 10.0f, 1000.0f}) { std::printf("to_can_velocity(%g)=%lld back=%g\n", v, I(C::to_can_velocity(v)), C::to_degreepersecond(C::to_can_velocity(v))); }
    // to_can_percentage: the sign is fixed in the PR, hence we print only to_percentage()
    for(int p : {-32000, -16000, 0, 16000, 32000}) { std::printf("to_percentage(%d)=%g\n", p, C::to_percentage(static_cast<mo::PWM>(p))); }
}


void motorperiodic()
{
    namespace mpe = embot::prot::can::motor::periodic;
    Frame out {};
    std::printf("\n### motor periodic\n");

    { mpe::Message_FOC m {}; mpe::Message_FOC::Info i {}; i.canaddress = 3; i.current = -1234; i.velocity = -55; i.position = -100000; m.load(i); m.get(out); dump("FOC", out); }
    { mpe::Message_STATUS m {}; mpe::Message_STATUS::Info i {}; i.canaddress = 2; i.controlmode = 0x06; i.quadencoderstate = 0x05; i.pwmfeedback = -16000; i.motorfaultstate = 0xDEADBEEF;
      m.load(i); m.get(out); dump("STATUS", out); }
    { mpe::Message_ADDITIONAL_STATUS m {}; mpe::Message_ADDITIONAL_STATUS::Info i {}; i.canaddress = 1; i.tbd[0] = 7; i.tbd[1] = 8; i.temperature = -42; m.load(i); m.get(out); dump("ADDITIONAL_STATUS", out); }
    { mpe::Message_DEBUG m {}; mpe::Message_DEBUG::Info i {}; i.canaddress = 5; i.payload = 0x1122334455667788ULL; m.load(i); m.get(out); dump("DEBUG", out); }
    { Frame f {}; std::uint8_t d[8] = {0x64, 0x00, 0x38, 0xFF, 0x2C, 0x01, 0x00, 0x83};
      frame_set_sender(f, 0); frame_set_clascmddestinationdata(f, Clas::periodicMotorControl, 15, 0xF, d, 8); frame_set_size(f, 8);
      mpe::Message_EMSTO2FOC_DESIRED_CURRENT m {}; bool r = m.load(f);
      std::printf("EMSTO2FOC load=%d targets=%lld %lld %lld %lld\n", r, I(m.info.target[0]), I(m.info.target[1]), I(m.info.target[2]), I(m.info.target[3])); }
    for(const char *s : {"a", "abcdef", "abcdefg", "0123456789abcdefghijklmnopqrst"})
    {
        mpe::Message_PRINT m {}; mpe::Message_PRINT::Info i {}; i.canaddress = 4; std::snprintf(i.text, sizeof(i.text), "%s", s);
        m.load(i); const std::uint8_t n = m.numberofframes();
        for(std::uint8_t k=0; k<n; k++) { m.get(out); char t[64]; std::snprintf(t, sizeof(t), "PRINT[%.10s] #%d of %d", s, k, n); dump(t, out); }
    }
}


void analogpolling()
{
    namespace ap = embot::prot::can::analog::polling;
    const Clas c = Clas::pollingAnalogSensor;
    Frame out {};
    char t[64];
    std::printf("\n### analog polling\n");

    { ap::Message_GET_FIRMWARE_VERSION m {}; bool r = m.load(mk(c, 0x1C, 3, {2, 0}));
      std::printf("GET_FIRMWARE_VERSION load=%d req=%lld.%lld\n", r, I(m.info.requiredprotocol.major), I(m.info.requiredprotocol.minor));
      ap::Message_GET_FIRMWARE_VERSION::ReplyInfo ri {}; ri.board = Board::strain2; ri.firmware.major = 2; ri.firmware.minor = 3; ri.firmware.build = 4; ri.protocol.major = 2; ri.protocol.minor = 1;
      m.reply(out, 3, ri); dump("GET_FIRMWARE_VERSION reply", out); }
    { ap::Message_SET_BOARD_ADX m {}; bool r = m.load(mk(c, 0x32, 3, {9})); std::printf("SET_BOARD_ADX load=%d address=%lld\n", r, I(m.info.address)); }

    for(Board b : {Board::strain, Board::strain2, Board::mtb, Board::mtb4})
        for(std::uint8_t v : {0, 1, 3, 4, 5})
        { ap::Message_SET_TXMODE m {b}; bool r = m.load(mk(c, 0x07, 3, {v}));
          std::printf("SET_TXMODE board=%lld v=%d load=%d transmit=%d strainmode=%lld\n", I(b), v, r, m.info.transmit, I(m.info.strainmode)); }

    { ap::Message_SET_CANDATARATE m {}; bool r = m.load(mk(c, 0x08, 3, {25})); std::printf("SET_CANDATARATE load=%d txperiod=%lld\n", r, I(m.info.txperiod)); }

    { ap::Message_SET_FULL_SCALES m {}; bool r = m.load(mk(c, 0x17, 3, {0x25, 0x12, 0x34}));
      std::printf("SET_FULL_SCALES load=%d rs=%lld ch=%lld fs=%lld\n", r, I(m.info.regulationset), I(m.info.channel), I(m.info.fullscale)); }
    { ap::Message_GET_FULL_SCALES m {}; bool r = m.load(mk(c, 0x18, 3, {0x14})); ap::Message_GET_FULL_SCALES::ReplyInfo ri {};
      ri.regulationset = m.info.regulationset; ri.channel = m.info.channel; ri.fullscale = 0xABCD; m.reply(out, 3, ri);
      std::snprintf(t, sizeof(t), "GET_FULL_SCALES load=%d", r); dump(t, out); }

    { ap::Message_SET_CH_DAC m {}; bool r = m.load(mk(c, 0x04, 3, {0x25, 0x12, 0x34}));
      std::printf("SET_CH_DAC load=%d rs=%lld ch=%lld offset=%lld\n", r, I(m.info.regulationset), I(m.info.channel), I(m.info.offset)); }
    { ap::Message_GET_CH_DAC m {}; bool r = m.load(mk(c, 0x0B, 3, {0x14})); ap::Message_GET_CH_DAC::ReplyInfo ri {};
      ri.regulationset = m.info.regulationset; ri.channel = m.info.channel; ri.offset = 0x8765; m.reply(out, 3, ri);
      std::snprintf(t, sizeof(t), "GET_CH_DAC load=%d", r); dump(t, out); }

    { ap::Message_SET_MATRIX_RC m {}; bool r = m.load(mk(c, 0x03, 3, {0x23, 0x04, 0x7F, 0xFE}));
      std::printf("SET_MATRIX_RC load=%d rs=%lld r=%lld c=%lld v=%lld\n", r, I(m.info.regulationset), I(m.info.row), I(m.info.col), I(m.info.value)); }
    { ap::Message_GET_MATRIX_RC m {}; bool r = m.load(mk(c, 0x0A, 3, {0x23, 0x04})); ap::Message_GET_MATRIX_RC::ReplyInfo ri {};
      ri.regulationset = m.info.regulationset; ri.row = m.info.row; ri.col = m.info.col; ri.value = 0x7FFE; m.reply(out, 3, ri);
      std::snprintf(t, sizeof(t), "GET_MATRIX_RC load=%d", r); dump(t, out); }

    { ap::Message_SET_MATRIX_G m {}; bool r = m.load(mk(c, 0x11, 3, {0x33})); std::printf("SET_MATRIX_G load=%d rs=%lld g=%lld\n", r, I(m.info.regulationset), I(m.info.gain)); }
    { ap::Message_GET_MATRIX_G m {}; bool r = m.load(mk(c, 0x12, 3, {})); ap::Message_GET_MATRIX_G::ReplyInfo ri {}; ri.gain = 0x44; m.reply(out, 3, ri);
      std::snprintf(t, sizeof(t), "GET_MATRIX_G load=%d", r); dump(t, out); }

    // SET_CALIB_TARE: the three modes. only mode 2 uses RS / CH / VALUE
    for(std::uint8_t mode : {0, 1, 2})
    { ap::Message_SET_CALIB_TARE m {}; bool r = m.load(mk(c, 0x13, 3, {mode, 0x03, 0x12, 0x34}));
      std::printf("SET_CALIB_TARE mode=%d load=%d rs=%lld ch=%lld v=%lld\n", mode, r, I(m.info.regulationset), I(m.info.channel), I(m.info.value)); }
    { ap::Message_GET_CALIB_TARE m {}; bool r = m.load(mk(c, 0x14, 3, {0x15})); ap::Message_GET_CALIB_TARE::ReplyInfo ri {};
      ri.regulationset = m.info.regulationset; ri.channel = m.info.channel; ri.value = 0x1234; m.reply(out, 3, ri);
      std::snprintf(t, sizeof(t), "GET_CALIB_TARE load=%d", r); dump(t, out); }

    // SET / GET_CURR_TARE: in robotology devel the channel is the whole byte. we use RS = 0 as it is done today
    for(std::uint8_t mode : {0, 1, 2})
    { ap::Message_SET_CURR_TARE m {}; bool r = m.load(mk(c, 0x15, 3, {mode, 0x03, 0x12, 0x34}));
      std::printf("SET_CURR_TARE mode=%d load=%d ch=%lld v=%lld\n", mode, r, I(m.info.channel), I(m.info.value)); }
    { ap::Message_GET_CURR_TARE m {}; bool r = m.load(mk(c, 0x16, 3, {0x05})); ap::Message_GET_CURR_TARE::ReplyInfo ri {};
      ri.channel = m.info.channel; ri.value = 0xFEDC; m.reply(out, 3, ri);
      std::snprintf(t, sizeof(t), "GET_CURR_TARE load=%d ch=%lld", r, I(m.info.channel)); dump(t, out); }

    // GET_CH_ADC: one channel
    { ap::Message_GET_CH_ADC m {}; bool r = m.load(mk(c, 0x0C, 3, {0x02, 0x01}));
      std::printf("GET_CH_ADC load=%d ch=%lld calib=%d\n", r, I(m.info.channel), m.info.getcalibrated);
      ap::Message_GET_CH_ADC::ReplyInfo ri {}; ri.channel = m.info.channel; ri.valueiscalibrated = m.info.getcalibrated; ri.valueindex = 0; ri.adcvalues[0] = 0x1234;
      m.reply(out, 3, ri); dump("GET_CH_ADC reply", out); }

    { ap::Message_SAVE2EE m {}; bool r = m.load(mk(c, 0x09, 3, {})); ap::Message_SAVE2EE::ReplyInfo ri {}; ri.ok = true; m.reply(out, 3, ri);
      std::snprintf(t, sizeof(t), "SAVE2EE load=%d", r); dump(t, out); }
    { ap::Message_GET_EEPROM_STATUS m {}; bool r = m.load(mk(c, 0x1B, 3, {})); ap::Message_GET_EEPROM_STATUS::ReplyInfo ri {}; ri.saved = true; m.reply(out, 3, ri);
      std::snprintf(t, sizeof(t), "GET_EEPROM_STATUS load=%d", r); dump(t, out); }

    // SET_SERIAL_NO with 7 chars (the only case where devel does not read beyond the DLC)
    { ap::Message_SET_SERIAL_NO m {}; bool r = m.load(mk(c, 0x19, 3, {'S', 'N', '0', '0', '1', '2', '3'}));
      std::printf("SET_SERIAL_NO load=%d serial=%.7s\n", r, m.info.serial); }
    { ap::Message_GET_SERIAL_NO m {}; bool r = m.load(mk(c, 0x1A, 3, {})); ap::Message_GET_SERIAL_NO::ReplyInfo ri {};
      std::memcpy(ri.serial, "SN00123", 7); m.reply(out, 3, ri); std::snprintf(t, sizeof(t), "GET_SERIAL_NO load=%d", r); dump(t, out); }

    { ap::Message_SKIN_SET_BRD_CFG m {}; bool r = m.load(mk(c, 0x4D, 3, {0x08, 40, 245}));
      std::printf("SKIN_SET_BRD_CFG load=%d type=%lld txperiod=%lld noload=%lld\n", r, I(m.info.skintype), I(m.info.txperiod), I(m.info.noload)); }
    { ap::Message_SKIN_SET_TRIANG_CFG m {}; bool r = m.load(mk(c, 0x50, 3, {0, 15, 2, 1, 0x00, 0x22}));
      std::printf("SKIN_SET_TRIANG_CFG load=%d start=%lld end=%lld shift=%lld ena=%d cdc=0x%04llX\n", r, I(m.info.trgStart), I(m.info.trgEnd), I(m.info.shift), m.info.enabled, I(m.info.cdcOffset)); }
    { ap::Message_SKIN_OBSOLETE_TACT_SETUP m {}; bool r = m.load(mk(c, 0x4C, 3, {0x01, 0x01, 0x01, 0x00, 0x22, 0x00, 0x00}));
      std::printf("SKIN_OBSOLETE_TACT_SETUP load=%d cdc=0x%04llX txperiod=%lld\n", r, I(m.info.cdcOffset), I(m.info.txperiod)); }
    { ap::Message_ACC_GYRO_SETUP m {}; bool r = m.load(mk(c, 0x4F, 3, {0x06, 10}));
      std::printf("ACC_GYRO_SETUP load=%d mask=0x%02llX txperiod=%lld\n", r, I(m.info.maskoftypes), I(m.info.txperiod)); }

    { ap::Message_AMPLIFIER_RESET m {}; bool r = m.load(mk(c, 0x1D, 3, {0x2F})); std::printf("AMPLIFIER_RESET load=%d rs=%lld ch=%lld\n", r, I(m.info.regulationset), I(m.info.channel)); }
    { ap::Message_AMPLIFIER_RANGE_OF_GAIN_GET m {}; bool r = m.load(mk(c, 0x1E, 3, {0x13})); ap::Message_AMPLIFIER_RANGE_OF_GAIN_GET::ReplyInfo ri {};
      ri.regulationset = m.info.regulationset; ri.channel = m.info.channel; ri.lowest = 100; ri.highest = 25000; m.reply(out, 3, ri);
      std::snprintf(t, sizeof(t), "AMPLIFIER_RANGE_OF_GAIN_GET load=%d", r); dump(t, out); }
    { ap::Message_AMPLIFIER_RANGE_OF_OFFSET_GET m {}; bool r = m.load(mk(c, 0x1F, 3, {0x13})); ap::Message_AMPLIFIER_RANGE_OF_OFFSET_GET::ReplyInfo ri {};
      ri.regulationset = m.info.regulationset; ri.channel = m.info.channel; ri.lowest = 0; ri.highest = 65535; m.reply(out, 3, ri);
      std::snprintf(t, sizeof(t), "AMPLIFIER_RANGE_OF_OFFSET_GET load=%d", r); dump(t, out); }
    { ap::Message_AMPLIFIER_GAINOFFSET_GET m {}; bool r = m.load(mk(c, 0x20, 3, {0x13})); ap::Message_AMPLIFIER_GAINOFFSET_GET::ReplyInfo ri {};
      ri.regulationset = m.info.regulationset; ri.channel = m.info.channel; ri.gain = 4800; ri.offset = 32000; m.reply(out, 3, ri);
      std::snprintf(t, sizeof(t), "AMPLIFIER_GAINOFFSET_GET load=%d", r); dump(t, out); }
    { ap::Message_AMPLIFIER_GAINOFFSET_SET m {}; bool r = m.load(mk(c, 0x21, 3, {0x13, 0x00, 0xC0, 0x12, 0x00, 0x7D}));
      std::printf("AMPLIFIER_GAINOFFSET_SET load=%d rs=%lld ch=%lld mode=%lld gain=%lld offset=%lld\n", r, I(m.info.regulationset), I(m.info.channel), I(m.info.mode), I(m.info.gain), I(m.info.offset)); }
    { ap::Message_AMPLIFIER_OFFSET_AUTOCALIB m {}; bool r = m.load(mk(c, 0x22, 3, {0x0F, 0x00, 0x00, 0x80, 0x10, 0x00, 4}));
      std::printf("AMPLIFIER_OFFSET_AUTOCALIB load=%d rs=%lld ch=%lld target=%lld tol=%lld nsam=%lld\n", r, I(m.info.regulationset), I(m.info.channel), I(m.info.target), I(m.info.tolerance), I(m.info.samples2average));
      ap::Message_AMPLIFIER_OFFSET_AUTOCALIB::ReplyInfo ri {}; ri.regulationset = m.info.regulationset; ri.channel = m.info.channel;
      ri.noisychannelmask = 0x81; ri.algorithmOKmask = 0x3F; ri.finalmeasureOKmask = 0x3E; ri.mae = 0x1234; m.reply(out, 3, ri); dump("AMPLIFIER_OFFSET_AUTOCALIB reply", out); }
    { ap::Message_AMPLIFIER_PGA308_CFG1_SET m {}; bool r = m.load(mk(c, 0x2B, 3, {0x12, 0x00, 0x40, 0x4E, 0x20, 0x00, 0x80}));
      std::printf("AMPLIFIER_PGA308_CFG1_SET load=%d rs=%lld ch=%lld GD=%lld GI=%lld S=%lld GO=%lld Vc=%lld Vz=%lld\n", r, I(m.info.regulationset), I(m.info.channel),
                  I(m.info.cfg1.GD), I(m.info.cfg1.GI), I(m.info.cfg1.S), I(m.info.cfg1.GO), I(m.info.cfg1.Voffsetcoarse), I(m.info.cfg1.Vzerodac)); }
    { ap::Message_AMPLIFIER_PGA308_CFG1_GET m {}; bool r = m.load(mk(c, 0x2A, 3, {0x12})); ap::Message_AMPLIFIER_PGA308_CFG1_GET::ReplyInfo ri {};
      ri.regulationset = m.info.regulationset; ri.channel = m.info.channel;
      ri.cfg1.GD = 0x4000; ri.cfg1.GI = 4; ri.cfg1.S = 1; ri.cfg1.GO = 6; ri.cfg1.Voffsetcoarse = 0x20; ri.cfg1.Vzerodac = 0x8000; m.reply(out, 3, ri);
      std::snprintf(t, sizeof(t), "AMPLIFIER_PGA308_CFG1_GET load=%d", r); dump(t, out); }

    { ap::Message_REGULATIONSET_SET m {}; bool r = m.load(mk(c, 0x3D, 3, {0x21})); std::printf("REGULATIONSET_SET load=%d rs=%lld mode=%lld\n", r, I(m.info.regulationset), I(m.info.mode)); }
    { ap::Message_REGULATIONSET_GET m {}; bool r = m.load(mk(c, 0x3E, 3, {0x01})); ap::Message_REGULATIONSET_GET::ReplyInfo ri {};
      ri.regulationset = 3; ri.mode = m.info.mode; m.reply(out, 3, ri); std::snprintf(t, sizeof(t), "REGULATIONSET_GET load=%d", r); dump(t, out); }

    { ap::Message_IMU_CONFIG_SET m {}; bool r = m.load(mk(c, 0x34, 3, {0x05, 0x00, 0x82, 0x81, 0xAA, 0x34, 0x12}));
      std::printf("IMU_CONFIG_SET load=%d mask=0x%04llX ffu08=%lld ffu16=%lld\n", r, I(m.info.sensormask), I(m.info.ffu08), I(m.info.ffu16));
      ap::Message_IMU_CONFIG_GET g {}; bool r2 = g.load(mk(c, 0x33, 3, {})); ap::Message_IMU_CONFIG_GET::ReplyInfo ri {};
      ri.sensormask = m.info.sensormask; ri.mode = m.info.mode; ri.orientation = m.info.orientation; ri.ffu08 = m.info.ffu08; ri.ffu16 = m.info.ffu16;
      g.reply(out, 3, ri); std::snprintf(t, sizeof(t), "IMU_CONFIG_GET load=%d", r2); dump(t, out); }
    { ap::Message_IMU_TRANSMIT m {}; bool r = m.load(mk(c, 0x35, 3, {20})); std::printf("IMU_TRANSMIT load=%d tx=%d txperiod=%lld\n", r, m.info.transmit, I(m.info.txperiod)); }

    { ap::Message_THERMOMETER_CONFIG_SET m {}; bool r = m.load(mk(c, 0x39, 3, {0x01})); std::printf("THERMOMETER_CONFIG_SET load=%d mask=%lld\n", r, I(m.info.sensormask)); }
    { ap::Message_THERMOMETER_CONFIG_GET m {}; bool r = m.load(mk(c, 0x38, 3, {})); ap::Message_THERMOMETER_CONFIG_GET::ReplyInfo ri {}; ri.sensormask = 1; m.reply(out, 3, ri);
      std::snprintf(t, sizeof(t), "THERMOMETER_CONFIG_GET load=%d", r); dump(t, out); }
    { ap::Message_THERMOMETER_TRANSMIT m {}; bool r = m.load(mk(c, 0x3A, 3, {5})); std::printf("THERMOMETER_TRANSMIT load=%d tx=%d txperiod=%lld\n", r, m.info.transmit, I(m.info.txperiod)); }

    { ap::Message_POS_CONFIG_SET m {}; bool r = m.load(mk(c, 0x52, 3, {0x00, 0x81, 0x10, 0x00, 0xA2, 0xF0, 0xFF}));
      std::printf("POS_CONFIG_SET load=%d type=%lld id=%lld\n", r, I(m.info.type), I(m.info.id));
      ap::Message_POS_CONFIG_GET g {}; bool r2 = g.load(mk(c, 0x51, 3, {0x00})); ap::Message_POS_CONFIG_GET::ReplyInfo ri {};
      ri.type = m.info.type; ri.descriptor[0] = m.info.descriptor[0]; ri.descriptor[1] = m.info.descriptor[1]; g.reply(out, 3, ri);
      std::snprintf(t, sizeof(t), "POS_CONFIG_GET load=%d", r2); dump(t, out); }
    { ap::Message_POS_TRANSMIT m {}; bool r = m.load(mk(c, 0x53, 3, {10})); std::printf("POS_TRANSMIT load=%d tx=%d txperiod=%lld\n", r, m.info.transmit, I(m.info.txperiod)); }
}


void sensorperiodic()
{
    namespace ape = embot::prot::can::analog::periodic;
    namespace ip = embot::prot::can::inertial::periodic;
    namespace sp = embot::prot::can::skin::periodic;
    Frame out {};
    std::printf("\n### analog / inertial / skin periodic\n");

    for(bool sat : {false, true})
    {
        ape::ADCsaturationInfo si {}; si.thereissaturation = sat; si.channel[0] = ape::ADCsaturation::LOW; si.channel[2] = ape::ADCsaturation::HIGH;
        { ape::Message_FORCE_VECTOR m {}; ape::Message_FORCE_VECTOR::Info i {}; i.canaddress = 2; i.x = 0x1234; i.y = 0xFEDC; i.z = 7; i.adcsaturationispresent = sat; i.adcsaturationinfo = si; m.load(i); m.get(out); dump("FORCE_VECTOR", out); }
        { ape::Message_TORQUE_VECTOR m {}; ape::Message_TORQUE_VECTOR::Info i {}; i.canaddress = 2; i.x = 1; i.y = 2; i.z = 3; i.adcsaturationispresent = sat; i.adcsaturationinfo = si; m.load(i); m.get(out); dump("TORQUE_VECTOR", out); }
        { ape::Message_UNCALIBFORCE_VECTOR_DEBUGMODE m {}; ape::Message_UNCALIBFORCE_VECTOR_DEBUGMODE::Info i {}; i.canaddress = 2; i.x = 9; i.y = 8; i.z = 7; i.adcsaturationispresent = sat; i.adcsaturationinfo = si; m.load(i); m.get(out); dump("UNCALIBFORCE_VECTOR_DEBUGMODE", out); }
        { ape::Message_UNCALIBTORQUE_VECTOR_DEBUGMODE m {}; ape::Message_UNCALIBTORQUE_VECTOR_DEBUGMODE::Info i {}; i.canaddress = 2; i.x = 6; i.y = 5; i.z = 4; i.adcsaturationispresent = sat; i.adcsaturationinfo = si; m.load(i); m.get(out); dump("UNCALIBTORQUE_VECTOR_DEBUGMODE", out); }
    }
    for(std::uint8_t mask : {0, 1, 2, 3}) { ape::Message_THERMOMETER_MEASURE m {}; ape::Message_THERMOMETER_MEASURE::Info i {}; i.canaddress = 4; i.mask = mask; i.temp0 = -123; i.temp1 = 456; m.load(i); m.get(out); dump("THERMOMETER_MEASURE", out); }
    { ape::Message_USERDEF m {}; ape::Message_USERDEF::Info i {}; i.canaddress = 1; for(int k=0; k<8; k++) { i.data[k] = 0x10+k; } m.load(i); m.get(out); dump("USERDEF", out); }
    { ape::Message_POS m {}; ape::Message_POS::Info i {}; i.canaddress = 5; i.loadDeciDeg(analog::posLABEL::two, 3, {-1, 1800, 3599}); m.load(i); m.get(out); dump("POS", out); }

    { ip::Message_DIGITAL_GYROSCOPE m {}; ip::Message_DIGITAL_GYROSCOPE::Info i {}; i.canaddress = 6; i.x = -1; i.y = 2; i.z = -3; m.load(i); m.get(out); dump("DIGITAL_GYROSCOPE", out); }
    { ip::Message_DIGITAL_ACCELEROMETER m {}; ip::Message_DIGITAL_ACCELEROMETER::Info i {}; i.canaddress = 6; i.x = 100; i.y = -200; i.z = 300; m.load(i); m.get(out); dump("DIGITAL_ACCELEROMETER", out); }
    { ip::Message_IMU_TRIPLE m {}; ip::Message_IMU_TRIPLE::Info i {}; i.canaddress = 6; i.seqnumber = 9; i.sensor = analog::imuSensor::gyr; i.value.x = -1; i.value.y = 2; i.value.z = -300; m.load(i); m.get(out); dump("IMU_TRIPLE", out); }
    { ip::Message_IMU_QUATERNION m {}; ip::Message_IMU_QUATERNION::Info i {}; i.canaddress = 6; i.value.w = 16384; i.value.x = -1; i.value.y = 2; i.value.z = -3; m.load(i); m.get(out); dump("IMU_QUATERNION", out); }
    { ip::Message_IMU_STATUS m {}; ip::Message_IMU_STATUS::Info i {}; i.canaddress = 6; i.seqnumber = 9; i.acquisitiontime = 0x12345678; m.load(i); m.get(out); dump("IMU_STATUS", out); }

    { sp::Message_TRG m {}; sp::Message_TRG::Info i {}; i.canaddress = 1; i.trianglenum = 7; for(int k=0; k<12; k++) { i.the12s[k] = 0xA0+k; }
      i.outofrangemaskofthe12s = 0x0ABC; i.notackmaskofthe12s = 1; i.notconnectedmaskofthe12s = 0x0FFF;
      Frame f0 {}, f1 {}; m.load(i); m.get(f0, f1); dump("TRG frame 0", f0); dump("TRG frame 1", f1); }
}


void bootloadermessages()
{
    namespace bl = embot::prot::can::bootloader;
    Frame out {};
    char t[64];
    std::printf("\n### bootloader\n");

    { bl::Message_BROADCAST m {}; bool r = m.load(mkbl(0xFF, 0xF, {})); bl::Message_BROADCAST::ReplyInfo ri {};
      ri.board = Board::amcbldc; ri.process = Process::application; ri.firmware.major = 1; ri.firmware.minor = 2; ri.firmware.build = 3; m.reply(out, 3, ri);
      std::snprintf(t, sizeof(t), "BROADCAST load=%d", r); dump(t, out); }
    { bl::Message_BOARD m {}; bool r = m.load(mkbl(0x00, 3, {0x01})); std::printf("BOARD load=%d eepromerase=%d\n", r, m.info.eepromerase); m.reply(out, 3); dump("BOARD reply", out); }
    { bl::Message_ADDRESS m {}; bool r = m.load(mkbl(0x01, 3, {0x10, 0x34, 0x12, 0x00, 0x78, 0x56})); std::printf("ADDRESS load=%d datalen=%lld address=0x%08llX\n", r, I(m.info.datalen), I(m.info.address)); }
    { bl::Message_START m {}; bool r = m.load(mkbl(0x02, 3, {})); m.reply(out, 3, true); std::snprintf(t, sizeof(t), "START load=%d", r); dump(t, out); }
    { bl::Message_DATA m {}; bool r = m.load(mkbl(0x03, 3, {1, 2, 3, 4, 5, 6})); std::printf("DATA load=%d size=%lld d0=%d d5=%d\n", r, I(m.info.size), m.info.data[0], m.info.data[5]); m.reply(out, 3, true); dump("DATA reply", out); }
    { bl::Message_END m {}; bool r = m.load(mkbl(0x04, 3, {})); m.reply(out, 3, false); std::snprintf(t, sizeof(t), "END load=%d", r); dump(t, out); }
    { bl::Message_GET_ADDITIONAL_INFO m {}; bool r = m.load(mkbl(0x0C, 3, {})); bl::Message_GET_ADDITIONAL_INFO::ReplyInfo ri {};
      std::snprintf(ri.info32, sizeof(ri.info32), "%s", "0123456789abcdefghijklmnopqrstu");
      const std::uint8_t n = m.numberofreplies();
      for(std::uint8_t k=0; k<n; k++) { m.reply(out, 3, ri); std::snprintf(t, sizeof(t), "GET_ADDITIONAL_INFO load=%d #%d", r, k); dump(t, out); } }
    { bl::Message_SET_ADDITIONAL_INFO2 m {}; bool valid = false;
      for(std::uint8_t k=0; k<8; k++) { m.load(mkbl(0x0D, 3, {k, static_cast<std::uint8_t>('a'+4*k), static_cast<std::uint8_t>('b'+4*k), static_cast<std::uint8_t>('c'+4*k), static_cast<std::uint8_t>('d'+4*k)})); valid = m.info.valid; }
      std::printf("SET_ADDITIONAL_INFO valid=%d info32=%.32s\n", valid, m.info.info32); }
    // SETCANADDRESS: without mask (the form used today) and with the mask of addresses to discard (fixed in the PR)
    { bl::Message_SETCANADDRESS m {}; bool r = m.load(mkbl(0x32, 3, {0x05})); std::printf("SETCANADDRESS load=%d address=%lld\n", r, I(m.info.address)); }
    { bl::Message_SETCANADDRESS m {}; bool r = m.load(mkbl(0x32, 3, {0xFF, 0x08, 0x80})); std::printf("SETCANADDRESS with mask load=%d address=%lld mask=0x%04llX\n", r, I(m.info.address), I(m.info.randominvalidmask)); }
    { bl::Message_GET_TIMEOFLIFE m {}; bool r = m.load(mkbl(0x0E, 3, {})); bl::Message_GET_TIMEOFLIFE::ReplyInfo ri {}; ri.timeoflife = 0x8877665544332211ULL; m.reply(out, 3, ri);
      std::snprintf(t, sizeof(t), "GET_TIMEOFLIFE load=%d", r); dump(t, out); }
}


int main()
{
    converter();
    motorpolling();
    motorperiodic();
    analogpolling();
    sensorperiodic();
    bootloadermessages();
    return 0;
}
