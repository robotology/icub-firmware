/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_INSTR_SCOPEEVIEWER_H_
#define __EMBOT_INSTR_SCOPEEVIEWER_H_

// a Scope in the Event Viewer of uVision: between on() and off() the activity Config::name is shown as running.
// it needs the library eventviewer (libs/midware/eventviewer: its api in the include path, its .c in the project).
//
// it comes from embot::app::scope::SignalEViewer (embot/app/embot_app_scope.h).

#include "embot_instr.h"

namespace embot::instr {

    class ScopeEViewer : public Scope
    {
    public:

        struct Config
        {
            // every ScopeEViewer must have its own pair {name, label}.
            // name is what the Event Viewer shows: a function w/ external linkage, e.g. void tick_of_the_motor() {}
            // label picks one of the user-defined IDs of the eventviewer
            enum class LABEL : uint8_t { one = 0, two = 1, three = 2, four = 3, five = 4, six = 5, seven = 6, eight = 7, numberof = 8, none = 31 };
            using NAME = void (*)();

            NAME name {nullptr};
            LABEL label {LABEL::none};

            constexpr Config() = default;
            constexpr explicit Config(NAME na, LABEL la) : name(na), label(la) {}

            constexpr bool isvalid() const { return (nullptr != name) && (LABEL::none != label) && (LABEL::numberof != label); }
        };

        // it loads the name in the eventviewer. w/ a Config not valid, on() and off() do nothing
        explicit ScopeEViewer(const Config &cfg);
        ~ScopeEViewer() override = default;

        void on() override;
        void off() override;

    private:
        uint8_t evid {0};       // the evEntityId_t of the eventviewer
        uint8_t prev {0};       // the activity running before on()
        bool valid {false};
    };

} // namespace embot::instr


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
