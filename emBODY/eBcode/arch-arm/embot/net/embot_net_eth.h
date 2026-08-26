
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef EMBOT_NET_ETH_H_
#define EMBOT_NET_ETH_H_


#include "embot_core.h"

#if __cpp_lib_byteswap >= 202110L
#include <bit>
#endif

namespace embot::net::eth {
            
    struct IPaddress
    {
        uint32_t v {0};
        IPaddress() = default;
        constexpr IPaddress(uint32_t i) : v(i) {}
        constexpr IPaddress(uint8_t a, uint8_t b, uint8_t c, uint8_t d) : v((d<<24)|(c<<16)|(b<<8)|(a)) {}
        IPaddress(const char *str) { 
            v = 0;
            if(nullptr == str) { return; }
            uint8_t a{0}; uint8_t b{0}; uint8_t c{0}; uint8_t d{0}; 
            if(4 == std::sscanf(str, "%hhi.%hhi.%hhi.%hhi", &a, &b, &c, &d)) 
            {
                v = ((d<<24)|(c<<16)|(b<<8)|(a)); 
            }    
        }
        constexpr char * tostring(char *s, size_t size) const { 
            if(nullptr != s) { std::snprintf(s, size, "%d.%d.%d.%d", (v)&0xff, (v>>8)&0xff, (v>>16)&0xff, v>>24); }
            return s;
        }
        std::string to_string() const {
            return std::string(std::to_string((v)&0xff) + "." + std::to_string((v>>8)&0xff) + "." + std::to_string((v>>16)&0xff) + "." + std::to_string(v>>24));
        }
        constexpr uint32_t value() const { return v; }
        constexpr uint32_t networkorder() const 
        {
        #if __cpp_lib_byteswap >= 202110L
            return std::byteswap(v);
        #else
            return ((v & 0x000000FFU) << 24) |
                   ((v & 0x0000FF00U) <<  8) |
                   ((v & 0x00FF0000U) >>  8) |
                   ((v & 0xFF000000U) >> 24);
        #endif
        }

    };
    
    constexpr IPaddress IPany {0, 0, 0, 0};
    constexpr IPaddress IPbroadcast {255, 255, 255, 255};
    constexpr IPaddress IPloopback {127, 0, 0, 1};
    constexpr IPaddress IPlocalhost {IPloopback};
    constexpr IPaddress IPdefault {10, 0, 1, 99};
    constexpr IPaddress IPpc104 {10, 0, 1, 104};
    
    using Port = uint16_t;
    
    struct SocketAddress
    {
        IPaddress addr {0, 0, 0, 0};
        Port port {0};
        constexpr SocketAddress(const IPaddress &a, const Port &p) : addr(a), port(p) {}
        constexpr SocketAddress() = default;
        std::string to_string() const {
            return addr.to_string() + ":" + std::to_string(port);
        }
        constexpr bool isvalid() const { return 0 != port; }
    };
 
    struct SocketSize
    {
        uint8_t inputmaxdatagrams {1};
        uint16_t inputmaxdatagramsize {1500};
        uint8_t outputmaxdatagrams {1};
        uint16_t outputmaxdatagramsize {1500};
        constexpr SocketSize(uint8_t i, uint16_t is, uint8_t o, uint16_t os) 
            : inputmaxdatagrams(1), inputmaxdatagramsize(is), 
              outputmaxdatagrams(o), outputmaxdatagramsize(os) {}
        constexpr SocketSize() = default;
    };
    
    struct SocketDescriptor
    {
        SocketSize size {1, 1500, 1, 1500};
        Port localport {3333};
        constexpr SocketDescriptor(const SocketSize &s, Port p) : size(s), localport(p) {}
        constexpr SocketDescriptor() = default;
    };    
    
    using IPmask = IPaddress;
 
    struct MACaddress
    {
        uint8_t v[6] {0x00, 0x80, 0xE1, 0x01, 0x02, 0x03};
        MACaddress() = default;
        constexpr MACaddress(uint8_t oui0, uint8_t oui1, uint8_t oui2, uint8_t nic0, uint8_t nic1, uint8_t nic2) 
        {
            v[0] = oui0; v[1] = oui1; v[2] = oui2; v[3] = nic0; v[4] = nic1; v[5] = nic2;
        }
        MACaddress(uint64_t x) {
            std::memmove(v, &x, sizeof(v));
        }            
        MACaddress(const char *str) { 
            if(nullptr == str) { return; }

            if(6 != std::sscanf(str, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &v[0], &v[1], &v[2], &v[3], &v[4], &v[5]))
            {
                std::memset(v, 0, sizeof(v));
            }
        }        
        char * tostring(char *s, size_t size) const { 
            if(nullptr != s) 
            { 
                std::snprintf(s, size, "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X", v[0], v[1], v[2], v[3], v[4], v[5]);           
            }
            return s;
        }
        std::string to_string() const {
            char str[20] = {0};
            tostring(str, sizeof(str));
            return str;
        }
        uint64_t toU64() const
        {
            uint64_t u = *reinterpret_cast<const uint64_t*>(v);
            return u;
        }            
    };
    
    struct IPconfig
    {
        MACaddress macaddr {1, 2, 3, 4, 5, 6};
        IPaddress ipaddr {10, 0, 1, 99};
        IPmask ipmask {255, 255, 255, 0};         
        IPaddress gateway {10, 0, 1, 104};
        constexpr IPconfig() = default;
        constexpr IPconfig(const MACaddress& ma, const IPaddress &ia, const IPmask& im) 
            : macaddr(ma), ipaddr(ia), ipmask(im), gateway({10, 0, 1, 104}) {} 
        constexpr IPconfig(const MACaddress& ma, const IPaddress &ia, const IPmask& im, const IPaddress &gt) 
            : macaddr(ma), ipaddr(ia), ipmask(im), gateway(gt) {}                
    };   
    
    
    
} // namespace embot::net::eth {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
