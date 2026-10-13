// theCANdispatcher.h
// The thin, non-template part of the CAN protocol handlers.
// - Parser: abstract interface that every thePROThandler<P> implements, so handlers of different protocols
//   (PX3Q, CubeMars, ...) can be held together as Parser*.
// - theCANdispatcher: owns no handler, it only keeps their pointers and offers each frame of a shared CAN bus
//   to them in the order given. The first one that recognises the frame consumes it.
// No heap: the pointers live in a std::array whose size is deduced from the constructor.

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <vector>

#include "embot_hw_can.h"

namespace embot::app::can {

using Frame = embot::hw::can::Frame;

class Parser
{
public:
    // true if the frame belongs to the protocol of this parser (consumed), false if it is not its own
    virtual bool parse(const Frame &frame) = 0;

protected:
    Parser() = default;
    ~Parser() = default;    // not virtual: nobody deletes a handler through a Parser*
};

template<std::size_t N>
class theCANdispatcher
{
public:
    static_assert(N > 0, "a dispatcher needs at least one parser");

    struct Stats
    {
        std::uint32_t frames   {0};     // frames offered to the dispatcher
        std::uint32_t consumed {0};     // frames recognised by a parser
        std::uint32_t unknown  {0};     // frames recognised by nobody
        constexpr Stats() = default;
    };

    // empty and constant-initialised: a static dispatcher costs no code at startup. fill it w/ add() before the first frame
    constexpr theCANdispatcher() = default;

    // the order of the add() matters: the first parser that recognises a frame wins. false if full
    bool add(Parser &parser)
    {
        if(_count >= N) { return false; }
        _parsers[_count++] = &parser;
        return true;
    }

    // or give all the parsers to the ctor (not constant-initialised if the pointers are not constants)
    template<typename... Ps>
    constexpr explicit theCANdispatcher(Ps*... parsers) : _parsers{static_cast<Parser*>(parsers)...}, _count(sizeof...(Ps))
    {
        static_assert(sizeof...(Ps) == N, "wrong number of parsers");
        static_assert((std::is_base_of_v<Parser, Ps> && ...), "every handler must derive from Parser");
    }

    // one frame. true if a parser consumed it
    bool process(const Frame &frame)
    {
        ++_stats.frames;
        for(std::size_t i = 0; i < _count; ++i)
        {
            if(_parsers[i]->parse(frame))
            {
                ++_stats.consumed;
                return true;
            }
        }
        ++_stats.unknown;
        return false;
    }

    // a burst of frames, in place: the consumed ones are removed, the unknown ones stay in `frames` (order preserved).
    // returns the number of consumed frames
    std::size_t process(std::vector<Frame> &frames)
    {
        std::size_t w {0};
        for(std::size_t r = 0; r < frames.size(); ++r)
        {
            if(false == process(frames[r]))
            {
                if(w != r) { frames[w] = frames[r]; }
                ++w;
            }
        }
        const std::size_t consumed = frames.size() - w;
        frames.resize(w);
        return consumed;
    }

    const Stats & stats() const { return _stats; }

private:
    std::array<Parser*, N> _parsers {};
    std::size_t _count {0};
    Stats _stats {};
};

// theCANdispatcher dispatcher {&px3q, &motor};  -> theCANdispatcher<2>
template<typename... Ps>
theCANdispatcher(Ps*...) -> theCANdispatcher<sizeof...(Ps)>;

} // namespace embot::app::can
