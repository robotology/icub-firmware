
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_APP_ETH_H_
#define _EMBOT_APP_ETH_H_

#include "embot_core.h"
#include "embot_core_utils.h"


namespace embot { namespace app { namespace eth {
    
    
    enum class Process : uint8_t { none = 255, eLoader = 0, eUpdater = 1, eApplication = 2 };
    
    enum class Month : uint8_t { 
        Jan =  1, Feb =  2, Mar =  3, Apr =  4, May =  5, Jun =  6,
        Jul =  7, Aug =  8, Sep =  9, Oct = 10, Nov = 11, Dec = 12
    };

    enum class Day : uint8_t { 
        one = 1, two = 2, three = 3, four = 4, five = 5, six = 6,
        seven = 7, eight = 8, nine = 9, ten = 10, eleven = 11, twelve = 12,
        thirteen = 13, fourteen = 14, fifteen = 15, sixteen = 16, seventeen = 17, 
        eigthteen = 18, nineteen = 19, twenty = 20, twentyone = 21, twentytwo = 22, 
        twentythree = 23, twentyfour = 24, twentyfive = 25, twentysix = 26, twentyseven = 27, 
        twentyeight = 28, twentynine = 29, thirthy = 30, thirthyone = 31
    };
    
    struct Date
    {
        uint32_t year   : 12;        /**< the year a.d. upto 2047 */
        uint32_t month  : 4;         /**< the month, where jan is 1, dec is 12. use eEmonth_t */
        uint32_t day    : 5;         /**< the day from 1 to 31 */
        uint32_t hour   : 5;         /**< the hour from 0 to 23 */
        uint32_t minute : 6;         /**< the minute from 0 to 59 */  
        constexpr Date(uint16_t y, Month mo, Day d, uint8_t h, uint8_t m)
            : year(y), month(embot::core::tointegral(mo)), day(embot::core::tointegral(d)), hour(h>23?23:h), minute(m>59?59:m) {}
        //constexpr Date() = default;                
    };
    
    struct Version
    {
        uint8_t major {3};
        uint8_t minor {0};
        constexpr Version(uint8_t ma, uint8_t mi) : major(ma), minor(mi) {}
    };
                
    struct Property 
    {
        Process process {Process::none};
        Version version {0, 0}; 
        Date date {2022, Month::Apr, Day::thirteen, 16, 40};
        constexpr Property() = default;
        //constexpr Property(Process p, const Version &v, const Date &d) : process(p), version(v), date(d) {}
        constexpr bool isvalid() const { return Process::none != process; } 
    };     

    
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
    };
    
    constexpr IPaddress IPlocalhost {127, 0, 0, 1};
    constexpr IPaddress IPdefault {10, 0, 1, 99};
    constexpr IPaddress IPpc104 {10, 0, 1, 104};
    
    using Port = uint16_t;
    
    struct SocketAddress
    {
        IPaddress addr {10, 0, 1, 99};
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
        uint64_t v {0};
        MACaddress() = default;
        constexpr MACaddress(uint64_t i) : v(i) {}
        constexpr MACaddress(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f) 
        {
            v = ((uint64_t)f<<40)|((uint64_t)e<<32)|((uint64_t)d<<24)|((uint64_t)c<<16)|((uint64_t)b<<8)|((uint64_t)a);            
        }
        MACaddress(const char *str) { 
            v = 0;
            if(nullptr == str) { return; }
            uint8_t x[6] = {0};
            if(6 == std::sscanf(str, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &x[0], &x[1], &x[2], &x[3], &x[4], &x[5]))
            {
                v = ((uint64_t)x[5]<<40)|((uint64_t)x[4]<<32)|((uint64_t)x[3]<<24)|((uint64_t)x[2]<<16)|((uint64_t)x[1]<<8)|((uint64_t)x[0]); ;
            }
        }        
        constexpr char * tostring(char *s, size_t size) const { 
            if(nullptr != s) 
            { 
                uint8_t a = (v)&0xff; uint8_t b = (v>>8)&0xff; uint8_t c = (v>>16)&0xff; 
                uint8_t d = (v>>24)&0xff; uint8_t e = (v>>32)&0xff; uint8_t f = (v>>40)&0xff;
                std::snprintf(s, sizeof(s), "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X", a, b, c, d, e, f);           
            }
            return s;
        }
        std::string to_string() const {
            char str[20] = {0};
            uint8_t a = (v)&0xff; uint8_t b = (v>>8)&0xff; uint8_t c = (v>>16)&0xff; 
            uint8_t d = (v>>24)&0xff; uint8_t e = (v>>32)&0xff; uint8_t f = (v>>40)&0xff;
            std::snprintf(str, sizeof(str), "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X", a, b, c, d, e, f);
            return str;
        }
    };
    
    struct IPconfig
    {
        MACaddress macaddr {"10.0.1.99"};
        IPaddress ipaddr {};
        IPmask ipmask {"255.255.255.0"}; 
        constexpr IPconfig() = default;
        constexpr IPconfig(const MACaddress& ma, const IPaddress &ia, const IPmask& im) 
            : macaddr(ma), ipaddr(ia), ipmask(im) {}        
    };    
    
}}} // namespace embot { namespace app { namespace eth {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
