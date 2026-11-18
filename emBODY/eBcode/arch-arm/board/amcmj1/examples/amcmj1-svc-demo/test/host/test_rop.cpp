// test/host: the wire format of TETHER (embot_svc_tether_ROP.h, protocol 6) on a PC.
// - Head: request / reply, options vs referred CMD
// - Writer: padding, room, the max number of ROPs, reserve() + commit(), close()
// - Reader: valid frames, every malformed case of the walk, frames of version 5, and random frames (it never reads
//   out of the payload: run it also w/ -fsanitize=address,undefined)

#include "embot_svc_tether_ROP.h"

#include <cstdio>
#include <cstring>
#include <random>
#include <vector>

namespace {

    using namespace embot::svc::tether::rop;

    int passed {0};
    int failed {0};

    void check(bool ok, const char *what)
    {
        std::printf("  %s  %s\n", ok ? "PASS" : "FAIL", what);
        ok ? passed++ : failed++;
    }

    std::vector<uint8_t> frame(std::initializer_list<std::pair<Head, std::vector<uint8_t>>> rops, size_t capacity = maxsizeofframe)
    {
        std::vector<uint8_t> b(capacity, 0xAA);         // not zero: the writer must write the padding
        Writer w {b.data(), b.size()};
        for(const auto &r : rops)
        {
            w.add(r.first, r.second.empty() ? nullptr : r.second.data());
        }
        b.resize(w.close(1234));
        return b;
    }

    Error parse(const std::vector<uint8_t> &f, size_t maxsize = maxsizeofframe)
    {
        Reader r {f.data(), f.size(), maxsize};
        return r.error();
    }

    void put16(std::vector<uint8_t> &f, size_t at, uint16_t v)
    {
        f[at] = static_cast<uint8_t>(v & 0xff);
        f[at + 1] = static_cast<uint8_t>(v >> 8);
    }

    void head()
    {
        std::printf("Head\n");
        const Head a {Head::request(CMD::set, OPTack, 0x12, 300)};
        check((CMD::set == a.cmd()) && a.ackrequired() && a.wantsreply() && (CMD::none == a.referredcmd()) && (300 == a.size),
              "set+ack: cmd, option, size");
        const Head k {Head::reply(CMD::ack, 0x12, CMD::set, 0)};
        check((CMD::ack == k.cmd()) && (CMD::set == k.referredcmd()) && (false == k.ackrequired()) && (0 == k.options()),
              "ack: the referred CMD in the high nibble, no options");
        const Head n {Head::reply(CMD::nak, 0xA1, CMD::run, 5)};
        check((0x75 == n.opc) && (CMD::run == n.referredcmd()), "nak<run>: opc = 0x75");
        uint8_t wire[4] {};
        Head::request(CMD::say, OPTnone, 0x01, 0x0203).store(wire);
        check((0x01 == wire[0]) && (0x01 == wire[1]) && (0x03 == wire[2]) && (0x02 == wire[3]), "the size is little endian");
        check(Head::request(CMD::ask, OPTnone, 1, 0).wantsreply() && (false == Head::request(CMD::set, OPTnone, 1, 4).wantsreply()),
              "wantsreply: ask yes, set w/out ack no");
    }

    void writer()
    {
        std::printf("Writer\n");
        const std::vector<uint8_t> f {frame({{Head::request(CMD::sig, OPTnone, 1, 1), {0x11}},
                                             {Head::request(CMD::sig, OPTnone, 2, 6), {1, 2, 3, 4, 5, 6}},
                                             {Head::reply(CMD::ack, 3, CMD::ping, 0), {}}})};
        check(f.size() == 16 + 8 + 12 + 4, "sizes: 4 + pad4(size) each");
        check((0 == f[16 + 5]) && (0 == f[16 + 6]) && (0 == f[16 + 7]), "the padding is zero");
        check((0 == f[4]) && (3 == f[5]), "header: layout 0, 3 ROPs");
        check(Error::none == parse(f), "what the writer writes, the reader accepts");

        std::vector<uint8_t> b(maxsizeofframe);
        Writer w {b.data(), b.size()};
        const std::vector<uint8_t> big(maxdatasize, 0x5A);
        check(nullptr != w.add(Head::request(CMD::say, OPTnone, 1, maxdatasize), big.data()), "one ROP of 512 bytes");
        check(nullptr != w.add(Head::request(CMD::say, OPTnone, 2, maxdatasize), big.data()), "two ROPs of 512 bytes");
        check(false == w.fits(maxdatasize), "a third does not fit 1472 bytes");
        check(w.fits(1472 - 16 - 2 * 516 - 4), "but the rest of the frame does");
        check(false == w.fits(maxdatasize + 1), "never more than maxdatasize");
        const size_t s {w.close(0)};
        b.resize(s);
        check((16 + 2 * 516 == s) && (Error::none == parse(b)), "1048 bytes, valid");

        std::vector<uint8_t> c(maxsizeofframe);
        Writer x {c.data(), c.size()};
        for(int i = 0; i < 40; i++)
        {
            x.add(Head::request(CMD::ping, OPTnone, 0, 0), nullptr);
        }
        check(FrameHeader::maxROPs == x.numberofROPs(), "at most 32 ROPs, also if bytes remain");

        std::vector<uint8_t> d(200, 0xEE);
        Writer y {d.data(), 100};
        check(nullptr == y.add(Head::request(CMD::say, OPTnone, 1, 81), nullptr), "a writer of 100 bytes refuses 16 + 4 + 84");
        check(nullptr != y.add(Head::request(CMD::say, OPTnone, 1, 80), nullptr), "and accepts 16 + 4 + 80");
        check(0xEE == d[100], "never past its capacity");

        std::vector<uint8_t> e(maxsizeofframe, 0xAA);
        Writer z {e.data(), e.size()};
        uint8_t *p {z.reserve(36)};
        std::memcpy(p, "abc", 3);
        z.commit(Head::reply(CMD::nak, 7, CMD::ask, 3));
        e.resize(z.close(0));
        check((16 + 8 == e.size()) && (0 == e[16 + 4 + 3]), "reserve(36) + commit of 3 bytes: a ROP of 8 bytes, padding zero");
        check(Error::none == parse(e), "valid");

        Writer n {nullptr, 1000};
        check((false == n.fits(0)) && (nullptr == n.add(Head::request(CMD::ping, OPTnone, 0, 0), nullptr)) && (0 == n.close(0)),
              "a writer w/out buffer does nothing");
    }

    void reader()
    {
        std::printf("Reader\n");
        const std::vector<uint8_t> good {frame({{Head::request(CMD::ask, OPTnone, 1, 0), {}},
                                                {Head::request(CMD::set, OPTack, 2, 5), {1, 2, 3, 4, 5}},
                                                {Head::request(CMD::run, OPTack, 0xA1, 2), {9, 8}}})};
        Reader r {good.data(), good.size(), maxsizeofframe};
        View v {};
        int n {0};
        bool ok {Error::none == r.error()};
        while(r.next(v))
        {
            ok = ok && (((0 == n) && (CMD::ask == v.cmd()) && (0 == v.size())) ||
                        ((1 == n) && (CMD::set == v.cmd()) && v.ackrequired() && (5 == v.size()) && (3 == v.data[2])) ||
                        ((2 == n) && (0xA1 == v.id()) && (0x0809 == v.value<uint16_t>())));
            n++;
        }
        check(ok && (3 == n), "a valid frame: three views w/ head and data in place");
        check(1234 == r.header().timeinmicroseconds, "the time of the header");

        check(Error::none == parse(frame({})), "a frame w/out ROPs");
        check(Error::nullpayload == Reader(nullptr, 100, maxsizeofframe).error(), "null payload");

        std::vector<uint8_t> f {good};
        f.resize(15);
        check(Error::toosmall == parse(f), "less than a header");

        check(Error::toobig == parse(good, good.size() - 1), "bigger than the max of the receiver");

        f = good; f[0] ^= 1;
        check(Error::wrongsignature == parse(f), "wrong signature");

        f = good; f[4] = 8;
        check(Error::wronglayout == parse(f), "layout 8: a frame of version 5 is rejected");

        f = frame({});
        f[5] = 33;
        check(Error::toomanyROPs == parse(f), "33 ROPs");

        f = good; f[5] = 4;
        check(Error::wrongsize == parse(f), "numOfROPs too big: no room for a head");

        f = good; f[5] = 2;
        check(Error::wrongsize == parse(f), "numOfROPs too small: bytes left at the end");

        f = good; f.push_back(0);
        check(Error::wrongsize == parse(f), "one byte more");

        f = good; f.pop_back();
        check(Error::wrongsize == parse(f), "one byte less: the padding of the last ROP is missing");

        f = good; put16(f, 16 + 4 + 2, 30);
        check(Error::wrongsize == parse(f), "a size that goes past the frame");

        f = good; put16(f, 16 + 4 + 2, 1);
        check(Error::wrongsize == parse(f), "a size that leaves bytes at the end");

        std::vector<uint8_t> b(maxsizeofframe, 0);
        Writer w {b.data(), b.size()};
        w.add(Head::request(CMD::set, OPTnone, 1, 513 - 1), nullptr);
        b.resize(w.close(0));
        put16(b, 16 + 2, 513);
        b.resize(b.size() + 4, 0);
        check(Error::wrongsize == parse(b), "513 bytes of data: more than maxdatasize");

        f = good;
        f[16 + 4 + 4 + 5] = 0x77;
        check(Error::none == parse(f), "the padding is ignored: not zero is still valid");
    }

    void randomframes()
    {
        std::printf("Reader on random frames\n");
        std::mt19937 rng {12345};
        int accepted {0};
        bool sound {true};
        for(int i = 0; i < 200000; i++)
        {
            // a valid header w/ random content, or random bytes, or a valid frame w/ a byte changed
            std::vector<uint8_t> f;
            const int kind {static_cast<int>(rng() % 3)};
            if(2 == kind)
            {
                std::vector<uint8_t> b(maxsizeofframe);
                Writer w {b.data(), b.size()};
                const int nr {static_cast<int>(rng() % 8)};
                for(int k = 0; k < nr; k++)
                {
                    w.add(Head::request(CMD::sig, OPTnone, static_cast<ID>(rng()), static_cast<uint16_t>(rng() % 300)), nullptr);
                }
                b.resize(w.close(rng()));
                b[rng() % b.size()] ^= static_cast<uint8_t>(1u << (rng() % 8));
                f = b;
            }
            else
            {
                f.resize(rng() % (maxsizeofframe + 50));
                for(auto &x : f) { x = static_cast<uint8_t>(rng()); }
                if((1 == kind) && (f.size() >= 16))
                {
                    const FrameHeader h {static_cast<uint8_t>(rng() % 34), 0};
                    std::memcpy(f.data(), &h, 16);
                }
            }
            Reader r {f.data(), f.size(), maxsizeofframe};
            if(Error::none == r.error())
            {
                accepted++;
                // re-walk: every ROP inside the payload, the sum fills it exactly
                View v {};
                size_t end {sizeofframeheader};
                int count {0};
                while(r.next(v))
                {
                    const size_t start {static_cast<size_t>(v.data - f.data()) - headsize};
                    sound = sound && (start == end) && (v.size() <= maxdatasize) && ((start + sizeofROP(v.size())) <= f.size());
                    end = start + sizeofROP(v.size());
                    count++;
                }
                sound = sound && (end == f.size()) && (count == r.header().numOfROPs);
            }
        }
        std::printf("  (%d of 200000 accepted)\n", accepted);
        check(sound && (accepted > 1000), "every accepted frame is sound, no access out of the payload");
    }

}

int main()
{
    head();
    writer();
    reader();
    randomframes();
    std::printf("%d/%d ROP checks passed\n", passed, passed + failed);
    return (0 == failed) ? 0 : 1;
}
