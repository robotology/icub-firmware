

/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------


#ifndef EMBOT_NET_LWIP_H_
#define EMBOT_NET_LWIP_H_


#if 0


    
#endif


#include "embot_core.h"
#include "embot_net_eth.h"

// forward declarations. i use them only through their pointers
struct netif;
struct pbuf;
struct udp_pcb;


namespace embot::net::lwip::sys {
    
    // - system section
    //
    
    // specify also a callback executed by the ETH IRQ handler on RX of a frame
    bool init(const embot::net::eth::IPconfig &ipconfig, const embot::core::Callback &onRXframe);
    
    
    // to be called at about every 50 ms 
    void tick();
    
    // to be activated by onRXframe.execute()
    void process();
       
    
} // namespace embot::net::lwip:;sys {


namespace embot::net::lwip::arp {
    
    bool isresolved(const embot::net::eth::IPaddress &addr);    
    bool resolve(const embot::net::eth::IPaddress &addr, bool forcearpframe);
    
} // namespace embot::net::lwip::arp {


namespace embot::net::lwip::pkt {
    
    // - packet managed old style: via functions.
    //  

    using OBJ = struct pbuf;    
        
    OBJ* retrieve(size_t size); 
    void release(OBJ *p);
    void load(OBJ *p, const void *payload, size_t size);   
    size_t size(OBJ *p);
    size_t copyto(OBJ *p, void *destination);       

} // namespace embot::net::lwip::pkt {


namespace embot::net::lwip::udp {
    
    // - udp socket managed old style: via functions.
    // 

    using OBJ = struct udp_pcb;    
    
    struct OnUDPrx
    {
        using Callback = void (*) (void *arg, udp::OBJ *udp, pkt::OBJ *rxpkt, const embot::net::eth::IPaddress *addr, const embot::net::eth::Port port);
        Callback callback {nullptr};
        void * arg {nullptr};
        
        constexpr OnUDPrx() = default;
        constexpr OnUDPrx(Callback _c, void *_a) : callback(_c), arg(_a) {}
        void load(Callback _c, void *_a) { callback = _c; arg = _a; }
    };  
    
    OBJ *retrieve();
    void release(OBJ *u);
    bool recv(OBJ *u, const OnUDPrx &onrx);
    bool bind(OBJ *u, const embot::net::eth::SocketAddress &socketaddress);
    embot::net::eth::Port localport(OBJ *u);    
    bool send(OBJ *u, pkt::OBJ *p, const embot::net::eth::SocketAddress &socketaddress);    
    bool connect(OBJ *u, const embot::net::eth::SocketAddress &socketaddress);
    bool send(OBJ *u, pkt::OBJ *p);
    void disconnect(OBJ *u);
    
    
    
} // namespace embot::net::lwip::udp {


#include "embot_net_eth_Packet.h"

namespace embot::net::lwip {
    
    // - packet as an object.
    //    
       
    struct Packet
    {
        // empty packet, not valid (isvalid()==false): useful as a placeholder
        Packet() = default;

        // allocates a packet of 'size' bytes
        static Packet allocate(size_t size);

        // adopts an already existing pbuf (e.g. the one passed by OnRX):
        // the destructor will free it with pbuf_free, same as for one we allocated ourselves
        static Packet adopt(struct ::pbuf *raw);

        bool isvalid() const;
        bool load(const void *payload, size_t size);
        size_t size() const;

        // copies the packet's content into 'destination', which the caller declares
        // to be 'destcapacity' bytes long. if the packet (size()) does not fit into
        // destcapacity, copies nothing and returns 0: no silent truncation, and no
        // possible overflow even if the caller did not check the size beforehand.
        size_t copyto(void *destination, size_t destcapacity) const;

        ~Packet();
        Packet(const Packet&) = delete;
        Packet& operator=(const Packet&) = delete;
        Packet(Packet&& other) noexcept;
        Packet& operator=(Packet&& other) noexcept;

        struct ::pbuf *rawpbuf() const { return p; }

    private:
        // used only internally by allocate()/adopt()
        explicit Packet(struct ::pbuf *raw) : p(raw) {}

        struct ::pbuf *p {nullptr};
    };

    
    // - conversion vs embot::net::eth::Packet
    //   

    // it can be used by UDPsocket::send() to obtain a embot::net::lwip::Packet from a embot::net::eth::Packet
    embot::net::lwip::Packet convert(const embot::net::eth::Packet &src);  

    // it can be used inside a UDPsocket::OnRX::callback() to load into a embot::net::eth::Packet the embot::net::lwip::Packet rxPacket {rxpkt}
    bool convert(const embot::net::lwip::Packet &src, const embot::net::eth::IPaddress &addr, embot::net::eth::Port port, embot::net::eth::Packet &dst);    
    // or directly
    bool convert(::pbuf *rxpkt, const embot::net::eth::IPaddress *addr, const embot::net::eth::Port port, embot::net::eth::Packet &dst);

    // - udp socket as an object.
    //  
    
    struct UDPsocket
    {
        struct OnRX
        {
            using Callback = void (*) (void *arg, ::udp_pcb *udp, ::pbuf *rxpkt, const embot::net::eth::IPaddress *addr, const embot::net::eth::Port port);
            Callback callback {nullptr};
            void * arg {nullptr};
            
            constexpr OnRX() = default;
            constexpr OnRX(Callback _c, void *_a) : callback(_c), arg(_a) {}
            void load(Callback _c, void *_a) { callback = _c; arg = _a; }
            void clear() { callback = nullptr; arg = nullptr; }
            bool isvalid() const { if(nullptr != callback) { return true; } else { return false; } } 
        };  
        
        UDPsocket();
        bool isvalid() const;          // false is pbuf_alloc has failed in ctor

        bool recv(const OnRX &onrx);
        bool bind(const embot::net::eth::SocketAddress &socketaddress); 
        embot::net::eth::Port localport() const;        
        bool send(const Packet &packet, const embot::net::eth::SocketAddress &socketaddress);
        bool connect(const embot::net::eth::SocketAddress &socketaddress);
        bool send(const Packet &packet);
        void disconnect();
        
        ~UDPsocket();

        UDPsocket(const UDPsocket&) = delete;
        UDPsocket& operator=(const UDPsocket&) = delete;
        
        UDPsocket(UDPsocket&& other) noexcept;
        UDPsocket& operator=(UDPsocket&& other) noexcept;

        static UDPsocket wrap(struct ::udp_pcb *raw);
        
    private:
        struct ::udp_pcb *u {nullptr};
        bool owner {true};
        UDPsocket(struct ::udp_pcb *raw, bool own);
    }; 
    
         
    
} // namespace embot::net::lwip {

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

