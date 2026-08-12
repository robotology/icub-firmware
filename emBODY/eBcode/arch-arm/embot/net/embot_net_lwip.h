

/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------


#ifndef EMBOT_NET_LWIP_H_
#define EMBOT_NET_LWIP_H_


#if 0

    this is a light C++ layer for lwip that interfaces w/ the embot environment and simplifies the usage of lwip
    for the case of udp usage.
    
    note: this is for NO_SYS bening defined in the lwipopts.h file so all calls must be done in a single thread,
    taht we call thrLWIP
    
    examples of optimal usage for achieving a simple reply to a ping is the following:
    - call init() in the thrLWIP.startup() w/ a proper callback on eth received frames that wakes up 
      the thrLWIP.onevent()
    - be sure that tick() is called rather often, for instance by sending an evtTick periodically every 50 ms to
      wake up the thrLWIP.onevent()
    - execute process() inside thrLWIP.onevent() when the onRXframe callback sends an event.   
      one could call process() in polling mode, but that would give delay in reply
      
      
    example of optimal usage w/ a server UDP socket
    - same as above, but somehow inside the thrLWIP, in its .startup() or in its .onevent():
    - create a UDP object, bind it to a soccket address, for instance any IP and port 666
    - add to it a callback taht will be executed inside process(), so inside thrLWIP.onevent(), when
      the lwip stack process the received frame. 
    - the above callback can reply immediately to the incoming packet by creating a reply packet, filling it,
      transmitting it w/ using send() on the same UDP object and then by deleing the reply packet
      OR it can store the received frame in some RX FIFO for later processing 
    - in any case the rxpkt must be deleted at end of the callback     
    
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



namespace embot::net::lwip {
    
    // - packet as an object.
    //    
       
    struct Packet
    {
        explicit Packet(size_t size);                   // it gets memory from lwip internals w/ udp_new()
        explicit Packet(struct ::pbuf *raw);            // it wraps a pbuf. use it in the UDPsocket::OnRX::callback()
        bool isvalid() const;                           // false se udp_new e' fallito nel costruttore
        bool load(const void *payload, size_t size);    // ritorna false se troppo grande o non valido
        size_t size() const;                            // capacita' allocata (tot_len del pbuf)
        size_t copyto(void *destination) const;  

        
        ~Packet();

        Packet(const Packet&) = delete;
        Packet& operator=(const Packet&) = delete;
        
        Packet(Packet&& other) noexcept;
        Packet& operator=(Packet&& other) noexcept;
        
        struct ::pbuf *rawpbuf() const { return p; }
    private:
        struct ::pbuf *p {nullptr};
    };   
 

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

