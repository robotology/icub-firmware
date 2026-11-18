// test/host: a board on the PC. the TETHER of amcmj1-svc-demo (theRPCmanager, theSTRmanager, the Dictionary) on two UDP
// sockets of linux, w/out the RTOS: so the host tools (tether.py, tethercodec.py) can be tested against the real code
// of the board.
//
//   ./tether_pcboard [rpcport=6666] [streamport=7777] [hostport=7777] [seconds=0 (forever)]
//
// rpc: every frame is given to theRPCmanager, its reply goes back to the sender. stream: it sends the frame of
// theSTRmanager every period to <IP of who started it>:hostport, and acks the pings. one thread, w/ poll().
// on one PC w/ tether.py str (which listens on 7777) give the board other ports, e.g. 16666 17777 7777, and run
// tether.py w/ --port 16666 (rpc) and --boardport 17777 (str)

#include "embot_svc_tether_ROP.h"
#include "embot_svc_tether_SharedVariables.h"
#include "embot_svc_tether_theRPCmanager.h"
#include "embot_svc_tether_theSTRmanager.h"
#include "embot_svc_theTETHERservice.h"     // the stub of test/host/stub
#include "dictionary.udpdemo.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <cstdio>
#include <cstdlib>

// in svc-demo.cpp, which is not compiled here
namespace udpdemo::echo {
    float readwave() { return 0.0f; }
    PWM readpwm() { return PWM {}; }
    // no echo on the PC: a made-up latency, a triangle whose centre moves on the 128 bins of 2 us, one bin every 10 ms
    void readlatency(types::HistogramSnapshot &snap)
    {
        constexpr int32_t nb {static_cast<int32_t>(types::HistogramSnapshot::numberofbins)};
        const int32_t t {static_cast<int32_t>((embot::core::now() / (10 * embot::core::time1millisec)) % (2 * nb))};
        const int32_t centre {(t < nb) ? t : (2 * nb - 1 - t)};
        embot::instr::Histogram h {};
        h.init(embot::instr::Histogram::Config(0, nb * latencystep, latencystep));
        for(int32_t k = 0; k < nb; k++)
        {
            const int32_t d {(k > centre) ? (k - centre) : (centre - k)};
            const int32_t n {(d < 32) ? (32 - d) : 0};
            for(int32_t i = 0; i < n; i++)
            {
                h.add(static_cast<uint64_t>(k) * latencystep);
            }
        }
        embot::instr::snapshot(h, snap);
    }
}

namespace {

    namespace rop = embot::svc::tether::rop;

    uint64_t nowus()
    {
        using namespace std::chrono;
        static const steady_clock::time_point t0 {steady_clock::now()};
        return static_cast<uint64_t>(duration_cast<microseconds>(steady_clock::now() - t0).count());
    }

    int opensocket(uint16_t port)
    {
        const int s {socket(AF_INET, SOCK_DGRAM, 0)};
        const int one {1};
        setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
        sockaddr_in a {};
        a.sin_family = AF_INET;
        a.sin_port = htons(port);
        a.sin_addr.s_addr = htonl(INADDR_ANY);
        if(0 != bind(s, reinterpret_cast<sockaddr *>(&a), sizeof(a)))
        {
            std::perror("bind");
            std::exit(1);
        }
        return s;
    }

    // the IPaddress of embot: a.b.c.d w/ a in the low byte, as the network order of linux
    embot::net::eth::IPaddress ipof(const sockaddr_in &a)
    {
        return embot::net::eth::IPaddress {static_cast<uint32_t>(a.sin_addr.s_addr)};
    }

}

int main(int argc, char **argv)
{
    const uint16_t rpcport {static_cast<uint16_t>((argc > 1) ? std::atoi(argv[1]) : 6666)};
    const uint16_t strport {static_cast<uint16_t>((argc > 2) ? std::atoi(argv[2]) : 7777)};
    const uint16_t hostport {static_cast<uint16_t>((argc > 3) ? std::atoi(argv[3]) : 7777)};
    const int seconds {(argc > 4) ? std::atoi(argv[4]) : 0};

    const auto &d {udpdemo::dict::dictionary};
    static embot::svc::tether::SharedVariablesLUT vars {d.variables, d.numberofvariables};
    if(false == vars.initialise())
    {
        std::printf("[error] the SharedVariables: entry %d\n", vars.invalidentry());
        return 1;
    }
    embot::svc::theTETHERservice::getInstance().set(&vars);

    rop::theSTRmanager &str {rop::theSTRmanager::getInstance()};
    const rop::theSTRmanager::Config sc {&vars, 100 * embot::core::time1millisec, embot::core::time1millisec,
                                         10 * embot::core::time1second, hostport, 10 * embot::core::time1second, nullptr, nullptr,
                                         rop::maxsizeofframe};
    rop::theRPCmanager &rpc {rop::theRPCmanager::getInstance()};
    const rop::theRPCmanager::Config rc {&vars, &str, d.operations, d.numberofoperations, nullptr, nullptr, 0x01000000, rop::maxsizeofframe};
    if((false == str.initialise(sc)) || (false == rpc.initialise(rc)))
    {
        std::printf("[error] the managers of TETHER\n");
        return 1;
    }

    const int srpc {opensocket(rpcport)};
    const int sstr {opensocket(strport)};
    std::printf("[info] tether_pcboard: rpc on UDP %u, stream on UDP %u, crc32 0x%08X\n", rpcport, strport, rpc.crc32());
    std::fflush(stdout);

    uint8_t rx[2048];
    uint64_t next {0};
    const uint64_t end {(seconds > 0) ? static_cast<uint64_t>(seconds) * 1000000u : 0};
    for(;;)
    {
        embot::core::testclock() = nowus();
        if((0 != end) && (embot::core::now() > end))
        {
            break;
        }
        pollfd fds[2] {{srpc, POLLIN, 0}, {sstr, POLLIN, 0}};
        poll(fds, 2, 1);
        embot::core::testclock() = nowus();

        if(fds[0].revents & POLLIN)
        {
            sockaddr_in from {};
            socklen_t fl {sizeof(from)};
            const ssize_t n {recvfrom(srpc, rx, sizeof(rx), 0, reinterpret_cast<sockaddr *>(&from), &fl)};
            if(n > 0)
            {
                rpc.parse(rx, static_cast<size_t>(n), ipof(from));
                const void *p {nullptr};
                size_t s {0};
                if(rpc.get(p, s))
                {
                    sendto(srpc, p, s, 0, reinterpret_cast<sockaddr *>(&from), fl);
                }
            }
        }

        if(fds[1].revents & POLLIN)
        {
            sockaddr_in from {};
            socklen_t fl {sizeof(from)};
            const ssize_t n {recvfrom(sstr, rx, sizeof(rx), 0, reinterpret_cast<sockaddr *>(&from), &fl)};
            const void *p {nullptr};
            size_t s {0};
            if((n > 0) && str.onrx(rx, static_cast<size_t>(n), ipof(from), p, s))
            {
                sendto(sstr, p, s, 0, reinterpret_cast<sockaddr *>(&from), fl);
            }
        }

        const uint64_t period {str.period()};
        if((0 != period) && (embot::core::now() >= next))
        {
            next = embot::core::now() + period;
            const void *p {nullptr};
            size_t s {0};
            rop::theSTRmanager::Destination dst {};
            if(str.get(p, s, dst))
            {
                sockaddr_in to {};
                to.sin_family = AF_INET;
                to.sin_port = htons(dst.port);
                to.sin_addr.s_addr = dst.ip.value();
                sendto(sstr, p, s, 0, reinterpret_cast<sockaddr *>(&to), sizeof(to));
            }
        }
    }
    close(srpc);
    close(sstr);
    return 0;
}
