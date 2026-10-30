#include "amcmj1_motdriver.h"

#include <cmath>

// until the ICC w/ amc.mot exists, the gateway plays amc.mot: the control mode is applied at once and a simple model gives
// current, velocity, position and pwm. set it to 0 when the ICC is wired: then the state comes only from onamcmot()
#if !defined(AMCMJ1_MOTDRIVER_EMULATED)
#define AMCMJ1_MOTDRIVER_EMULATED 1
#endif

namespace {

    using namespace amcmj1::mot;

    // constant-initialised: no constructor runs at startup
    struct Data
    {
        MOTORpids pids {};
        MOTORsetup setup {};
        MOTORtarget target {};              // the last one used
        MOTORstate state {};
        MOTORstatus status {};
        ControlMode mode {ControlMode::idle};   // the control mode given to amc.mot
        uint8_t configured {0};
        bool hastarget {false};             // a setpoint arrived after the last change of control mode
        embot::core::Time lasttarget {0};   // for the watchdog
        embot::core::Time lastmodel {0};    // for the emulation
    };

    Data data {};

    bool finite(const MOTORpids &p)
    {
        return std::isfinite(p.curkp) && std::isfinite(p.curki) && std::isfinite(p.curkd) &&
               std::isfinite(p.velkp) && std::isfinite(p.velki) && std::isfinite(p.velkd);
    }

    bool valid(const MOTORsetup &s)
    {
        return std::isfinite(s.nominal) && std::isfinite(s.peak) && std::isfinite(s.overload) &&
               (s.nominal >= 0.0f) && (s.peak >= s.nominal) && (s.overload >= 0.0f) && (s.poles > 0);
    }

    constexpr float clamp(float x, float lo, float hi) { return (x < lo) ? lo : ((x > hi) ? hi : x); }


    // ---- the ICC w/ amc.mot: to do. each function will send the messages of amc.mot (8 bytes each), e.g. the pids as
    //      SET_CURRENT_PID / SET_VELOCITY_PID, the setup as SET_CURRENT_LIMIT + SET_MOTOR_CONFIG, the mode as
    //      SET_CONTROL_MODE, the setpoint as the periodic message of the target

    void toamcmotpids(const MOTORpids &) {}
    void toamcmotsetup(const MOTORsetup &) {}
    void toamcmotmode(ControlMode) {}
    void toamcmottarget(const MOTORtarget &) {}


    // ---- the emulation of amc.mot: first order responses, w/ the time constant tau. not a motor: just something that moves

#if AMCMJ1_MOTDRIVER_EMULATED
    constexpr float tau {0.05f};            // s
    constexpr float degpersecperA {200.0f}; // current mode: the velocity it reaches w/ 1 A
    constexpr float degpersecperpwm {10.0f};// openloop: the velocity it reaches w/ 1 %
    constexpr float degpersecperpwmv {0.1f};// speedvoltage: the pwm it shows for 1 deg/s
    constexpr float apercentpwm {0.02f};    // the current it shows for 1 % of pwm

    void model(embot::core::Time now)
    {
        MOTORstate &s {data.state};
        float dt {static_cast<float>(now - data.lastmodel) / 1.0e6f};
        data.lastmodel = now;
        dt = clamp(dt, 0.0f, 0.02f);
        const float k {clamp(dt / tau, 0.0f, 1.0f)};
        const float sp {data.hastarget ? data.target.value : 0.0f};
        float vref {0.0f};
        switch(data.mode)
        {
            case ControlMode::current:      { s.current = sp; vref = degpersecperA * sp; s.pwm = clamp(sp / apercentpwm, -100.0f, 100.0f); } break;
            case ControlMode::speedvoltage: { vref = sp; s.pwm = clamp(degpersecperpwmv * s.velocity, -100.0f, 100.0f); s.current = apercentpwm * s.pwm; } break;
            case ControlMode::openloop:     { s.pwm = sp; vref = degpersecperpwm * sp; s.current = apercentpwm * s.pwm; } break;
            default:                        { s.pwm = 0.0f; s.current = 0.0f; } break;
        }
        s.velocity += k * (vref - s.velocity);
        s.position += s.velocity * dt;
        s.controlmode = data.mode;
        s.configured = data.configured;
        s.emulated = 1;
        s.counter++;
    }
#endif

} // namespace


namespace amcmj1::motdriver {

    const char *why(Result r)
    {
        switch(r)
        {
            case Result::ok:            return "ok";
            case Result::notidle:       return "macromotor not idle";
            case Result::notconfigured: return "macromotor: pids and setup first";
            case Result::badvalue:      return "macromotor: value not valid";
            case Result::fault:         return "macromotor: external fault";
            default:                    return "macromotor: failed";
        }
    }

    Result setpids(const MOTORpids &pids)
    {
        if(ControlMode::idle != data.mode)
        {
            data.status.refused++;
            return Result::notidle;
        }
        if(false == finite(pids))
        {
            data.status.refused++;
            return Result::badvalue;
        }
        data.pids = pids;
        data.configured |= configuredPIDS;
        data.state.configured = data.configured;
        toamcmotpids(pids);
        return Result::ok;
    }

    Result setsetup(const MOTORsetup &setup)
    {
        if(ControlMode::idle != data.mode)
        {
            data.status.refused++;
            return Result::notidle;
        }
        if(false == valid(setup))
        {
            data.status.refused++;
            return Result::badvalue;
        }
        data.setup = setup;
        data.configured |= configuredSETUP;
        data.state.configured = data.configured;
        toamcmotsetup(setup);
        return Result::ok;
    }

    Result setmode(ControlMode mode, embot::core::Time now)
    {
        if(false == askable(mode))
        {
            data.status.refused++;
            return Result::badvalue;
        }
        if(ControlMode::idle != mode)
        {
            if(configuredALL != (data.configured & configuredALL))
            {
                data.status.refused++;
                return Result::notconfigured;
            }
            if(0 != (data.state.faults & faults::externalfault))
            {
                data.status.refused++;
                return Result::fault;
            }
        }
        data.mode = mode;
        data.hastarget = false;             // the setpoints of the previous mode are not valid any more
        data.lasttarget = now;              // the watchdog starts now
        data.status.modes++;
        toamcmotmode(mode);
#if AMCMJ1_MOTDRIVER_EMULATED
        data.state.controlmode = mode;
#endif
        return Result::ok;
    }

    Target target(const MOTORtarget &t, embot::core::Time now)
    {
        if(ControlMode::idle == data.mode)
        {
            return Target::idle;
        }
        if((t.mode != data.mode) || (false == std::isfinite(t.value)))
        {
            data.status.rejected++;
            return Target::wrongmode;
        }
        MOTORtarget use {t};
        if(ControlMode::openloop == use.mode)
        {
            use.value = clamp(use.value, -100.0f, 100.0f);
        }
        else if((ControlMode::current == use.mode) && (data.setup.peak > 0.0f))
        {
            use.value = clamp(use.value, -data.setup.peak, data.setup.peak);
        }
        data.target = use;
        data.hastarget = true;
        data.lasttarget = now;
        data.status.targets++;
        toamcmottarget(use);
#if AMCMJ1_MOTDRIVER_EMULATED
        model(now);
#endif
        return Target::used;
    }

    void tick(embot::core::Time now)
    {
        if((ControlMode::idle != data.mode) && ((now - data.lasttarget) > mot::watchdog))
        {
            data.status.watchdogs++;
            setmode(ControlMode::idle, now);
        }
#if AMCMJ1_MOTDRIVER_EMULATED
        if((now - data.lastmodel) > (5 * embot::core::time1millisec))
        {
            model(now);                     // no setpoints: the model goes on at the rate of the tick
        }
#endif
    }

    void onamcmot(const MOTORstate &state)
    {
#if !AMCMJ1_MOTDRIVER_EMULATED
        data.state = state;
        data.state.configured = data.configured;
        data.state.emulated = 0;
        if(ControlMode::hwfault == state.controlmode)
        {
            data.mode = ControlMode::idle;  // amc.mot left the mode: a new one must be asked
        }
#else
        (void)state;
#endif
    }

    MOTORstate state() { return data.state; }
    MOTORpids pids() { return data.pids; }
    MOTORsetup setup() { return data.setup; }
    MOTORtarget lasttarget() { return data.target; }
    MOTORstatus status() { return data.status; }

} // namespace amcmj1::motdriver
