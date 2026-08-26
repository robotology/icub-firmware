

/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_net_lwip.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "lwip/init.h"
#include "netif/etharp.h"
#include "lwip/timeouts.h"
#include "lwip/udp.h"
#include "lwip/pbuf.h"


#include "embot_hw_lwip.h"


// --------------------------------------------------------------------------------------------------------------------
// - some checks
// --------------------------------------------------------------------------------------------------------------------

static_assert(sizeof(embot::net::eth::IPaddress) == sizeof(ip4_addr_t),
              "IPaddress and ip4_addr_t mjust have same dimension for the reinterpret_cast<>");
static_assert(alignof(embot::net::eth::IPaddress) == alignof(ip4_addr_t),
              "IPaddress e ip4_addr_t must have the same alignment for the reinterpret_cast<>");
#if LWIP_IPV6
    #error "w/ LWIP_IPV6 enabled, ip_addr_t is not an uint32_t anymore: reinterpret_cast<> for embot::net::eth::IPaddress must change"
#endif


static_assert(std::is_same_v<embot::net::lwip::UDPsocket::OnRX::Callback, void (*)(void*, ::udp_pcb*, ::pbuf*, const embot::net::eth::IPaddress*, uint16_t)>,
              "OnRX::Callback ha una firma diversa da quella attesa");

static_assert(std::is_same_v<udp_recv_fn, void (*)(void*, ::udp_pcb*, ::pbuf*, const ip_addr_t*, u16_t)>,
              "udp_recv_fn di lwIP ha una firma diversa da quella attesa: aggiorna OnRX::Callback e il reinterpret_cast in recv()");




// --------------------------------------------------------------------------------------------------------------------
// - extern functions
// --------------------------------------------------------------------------------------------------------------------


namespace embot::net::lwip::sys {
    
                 
    bool init(const embot::net::eth::IPconfig &ipconfig, const embot::core::Callback &onRXframe)
    {
        bool r {true};
                
        lwip_init();        
        embot::hw::lwip::netconfig(ipconfig, onRXframe);
        
        return r;
    }
        
    
    void tick()
    {
        sys_check_timeouts();
        
        embot::hw::lwip::checklink();

//        #if LWIP_DHCP
//        DHCP_Periodic_Handle(embot::net::lwip::sys::network());
//        #endif 
        
    }
    
    void process()
    {
        embot::hw::lwip::checkinput();
    }
    
//    netIF *network()
//    {
//        return embot::hw::lwip::network();
//    }
    
} // namespace embot::net::lwip::sys {


namespace embot::net::lwip::arp {
    
    bool isresolved(const embot::net::eth::IPaddress &addr)
    {
        bool r {true};
        
        struct eth_addr *eth_ret = nullptr;
        const ip4_addr_t *ip_ret = nullptr;

        r = etharp_find_addr(embot::hw::lwip::network(), reinterpret_cast<const ip_addr_t *>(&addr), &eth_ret, &ip_ret) >= 0;        
        
        return r;        
    }    
    
    
    bool resolve(const embot::net::eth::IPaddress &addr, bool forcearpframe)
    {        
        if((false == forcearpframe) && (true == isresolved(addr)))
        {
            return true;
        } 
        
        err_t e = etharp_request(embot::hw::lwip::network(), reinterpret_cast<const ip_addr_t *>(&addr));
        
        return (e == ERR_OK);  

    }        

} //namespace embot::net::lwip::arp {

        
namespace embot::net::lwip::pkt {

    // - packet managed old style: via functions.
    //
    
    OBJ* retrieve(size_t size)
    {
        return pbuf_alloc(PBUF_TRANSPORT, size, PBUF_RAM);
    } 
    
    void release(OBJ *p)
    {
        pbuf_free(reinterpret_cast<struct pbuf*>(p));
    }
    
    void load(OBJ *p, const void *payload, size_t size)
    {
        pbuf_take(reinterpret_cast<struct pbuf*>(p), payload, size);
    }
    
    size_t size(OBJ *p)
    {
        return reinterpret_cast<struct pbuf*>(p)->tot_len;
    }
    
    size_t copyto(OBJ *p, void *destination)
    {
        size_t r {0};
        if(nullptr != destination)
        {
            r = pbuf_copy_partial(reinterpret_cast<struct pbuf*>(p), destination, reinterpret_cast<struct pbuf*>(p)->tot_len, 0);
        }                
        return r;
    }        
    
} // namespace embot::net::lwip::pkt {


namespace embot::net::lwip::udp {
    
    OBJ *retrieve()
    {
        return udp_new();
    }
    
    void release(OBJ *u)
    {
        udp_remove(reinterpret_cast<struct udp_pcb*>(u));
    }

    bool recv(OBJ *u, const OnUDPrx &onrx)
    {
        using LWIPonUDPreception = void (*) (void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, uint16_t port);
        static_assert(std::is_same<LWIPonUDPreception, udp_recv_fn>::value, "signature of udp_recv_fn has changed");        
        udp_recv(reinterpret_cast<struct udp_pcb*>(u), reinterpret_cast<LWIPonUDPreception>(onrx.callback), onrx.arg); 
        return true;        
    }

    bool bind(OBJ *u, const embot::net::eth::SocketAddress &socketaddress)
    {
        err_t e = udp_bind(reinterpret_cast<struct udp_pcb*>(u), reinterpret_cast<const ip_addr_t*>(&socketaddress.addr), socketaddress.port);
        return ERR_OK == e;
    }
    
    embot::net::eth::Port localport(OBJ *u)
    {
        return u->local_port;
    }

    bool send(OBJ *u, pkt::OBJ *p, const embot::net::eth::SocketAddress &socketaddress)
    {
        err_t e = udp_sendto(reinterpret_cast<struct udp_pcb*>(u), reinterpret_cast<struct pbuf*>(p), reinterpret_cast<const ip_addr_t *>(&socketaddress.addr), socketaddress.port);
        return ERR_OK == e;          
    }

    bool connect(OBJ *u, const embot::net::eth::SocketAddress &socketaddress)
    {
        err_t e = udp_connect(reinterpret_cast<struct udp_pcb*>(u), reinterpret_cast<const ip_addr_t *>(&socketaddress.addr), socketaddress.port);
        return ERR_OK == e;         
    } 

    bool send(OBJ *u, pkt::OBJ *p)
    {
        err_t e = udp_send(reinterpret_cast<struct udp_pcb*>(u), reinterpret_cast<struct pbuf*>(p));
        return ERR_OK == e;          
    }

    void disconnect(OBJ *u)
    {
        udp_disconnect(reinterpret_cast<struct udp_pcb*>(u));
    }  
    
} // namespace embot::net::lwip::udp {



// --- Packet


embot::net::lwip::Packet embot::net::lwip::Packet::allocate(size_t size)
{
    return Packet(pbuf_alloc(PBUF_TRANSPORT, static_cast<u16_t>(size), PBUF_RAM));
}

embot::net::lwip::Packet embot::net::lwip::Packet::adopt(struct ::pbuf *raw)
{
    return Packet(raw);
}

embot::net::lwip::Packet::~Packet()
{
    if (nullptr != p)
    {
        pbuf_free(p);
    }
}

embot::net::lwip::Packet::Packet(Packet&& other) noexcept : p(other.p)
{
    other.p = nullptr;   // ownership transferred: source object left empty
}

embot::net::lwip::Packet& embot::net::lwip::Packet::operator=(Packet&& other) noexcept
{
    if (this != &other)
    {
        if (nullptr != p)
        {
            pbuf_free(p);   // free any pbuf already owned by *this
        }
        p = other.p;
        other.p = nullptr;
    }
    return *this;
}

bool embot::net::lwip::Packet::isvalid() const
{
    return (nullptr != p);
}

bool embot::net::lwip::Packet::load(const void *payload, size_t size)
{
    if (nullptr == p)
    {
        return false;
    }
    return (ERR_OK == pbuf_take(p, payload, static_cast<u16_t>(size)));
}

size_t embot::net::lwip::Packet::size() const
{
    return (nullptr != p) ? p->tot_len : 0;
}

size_t embot::net::lwip::Packet::copyto(void *destination, size_t destcapacity) const
{
    if ((nullptr == p) || (nullptr == destination))
    {
        return 0;
    }
    if (p->tot_len > destcapacity)
    {
        return 0;   // does not fit: explicit rejection, no partial copy
    }
    return pbuf_copy_partial(p, destination, p->tot_len, 0);
}

//embot::net::lwip::Packet::Packet(size_t size)
//{
//    p = pbuf_alloc(PBUF_TRANSPORT, static_cast<u16_t>(size), PBUF_RAM);
//    // p puo' essere nullptr se l'allocazione fallisce: verificabile con isvalid()
//}

//embot::net::lwip::Packet::Packet(struct ::pbuf *raw) : p(raw)
//{
//    // nessuna allocazione: adottiamo un pbuf gia' esistente.
//    // il distruttore lo liberera' con pbuf_free, come fa gia' per quelli allocati da noi.
//}

//embot::net::lwip::Packet::~Packet()
//{
//    if (nullptr != p)
//    {
//        pbuf_free(p);
//    }
//}

//embot::net::lwip::Packet::Packet(Packet&& other) noexcept : p(other.p)
//{
//    other.p = nullptr; // svuota l'oggetto sorgente: ownership trasferita
//}

//embot::net::lwip::Packet& embot::net::lwip::Packet::operator=(embot::net::lwip::Packet&& other) noexcept
//{
//    if (this != &other)
//    {
//        if (nullptr != p)
//        {
//            pbuf_free(p); // libera l'eventuale pbuf gia' posseduto da *this
//        }
//        p = other.p;
//        other.p = nullptr;
//    }
//    return *this;
//}

//bool embot::net::lwip::Packet::isvalid() const
//{
//    return (nullptr != p);
//}

//bool embot::net::lwip::Packet::load(const void *payload, size_t size)
//{
//    if (nullptr == p)
//    {
//        return false;
//    }
//    return (ERR_OK == pbuf_take(p, payload, static_cast<u16_t>(size)));
//}

//size_t embot::net::lwip::Packet::size() const
//{
//    return (nullptr != p) ? p->tot_len : 0;
//}

//size_t embot::net::lwip::Packet::copyto(void *destination, size_t destcapacity) const
//{
//    if ((nullptr == p) || (nullptr == destination))
//    {
//        return 0;
//    }
//    if (p->tot_len > destcapacity)
//    {
//        return 0;   // destination is too small
//    }    
//    return pbuf_copy_partial(p, destination, p->tot_len, 0);
//}

embot::net::lwip::Packet embot::net::lwip::convert(const embot::net::eth::Packet &src)
{
    embot::net::lwip::Packet dst = embot::net::lwip::Packet::allocate(src.size());

    if (!dst.isvalid())
    {
        return dst;
    }

    if (!dst.load(src.data(), src.size()))
    {
        return embot::net::lwip::Packet();   // defensive: should not happen at equal size
    }

    return dst;   
}

bool embot::net::lwip::convert(const embot::net::lwip::Packet &src, const embot::net::eth::IPaddress &addr, embot::net::eth::Port port, embot::net::eth::Packet &dst)
{
    if (!src.isvalid())
    {
        return false;
    }

    size_t n = src.copyto(dst.data(), dst.capacity());
    if ((0 == n) && (src.size() > 0))
    {
        return false;   // destination is too small
    }
    dst.setsize(n);
    dst.setaddress(embot::net::eth::SocketAddress(addr, port));

    return true;
}


bool embot::net::lwip::convert(::pbuf *rxpkt, const embot::net::eth::IPaddress *addr, const embot::net::eth::Port port, embot::net::eth::Packet &dst)
{
    embot::net::lwip::Packet src = embot::net::lwip::Packet::adopt(rxpkt);   // caveat: must always adopt the pbuf, so that it is freed on exit regardless of outcome
    if (nullptr == addr)
    {
        return false;
    }
    return convert(src, *addr, port, dst);
}


// udp

embot::net::lwip::UDPsocket::UDPsocket()
{
    u = udp_new();
}

embot::net::lwip::UDPsocket::UDPsocket(struct ::udp_pcb *raw, bool own) : u(raw), owner(own) {}
    
embot::net::lwip::UDPsocket embot::net::lwip::UDPsocket::wrap(struct ::udp_pcb *raw)
{
    return UDPsocket(raw, false); // non-owning: il distruttore NON chiamera' udp_remove
}

embot::net::lwip::UDPsocket::~UDPsocket()
{
    if ((nullptr != u) && owner)
    {
        udp_remove(u);
    }
}

embot::net::lwip::UDPsocket::UDPsocket(embot::net::lwip::UDPsocket&& other) noexcept : u(other.u), owner(other.owner)
{
    other.u = nullptr;
}

embot::net::lwip::UDPsocket& embot::net::lwip::UDPsocket::operator=(embot::net::lwip::UDPsocket&& other) noexcept
{
    if (this != &other)
    {
        if ((nullptr != u) && owner) { udp_remove(u); }
        u = other.u;
        owner = other.owner;
        other.u = nullptr;
    }
    return *this;
}

bool embot::net::lwip::UDPsocket::isvalid() const
{
    return (nullptr != u);
}

bool embot::net::lwip::UDPsocket::bind(const embot::net::eth::SocketAddress &socketaddress)
{
    if ((nullptr == u) || (!socketaddress.isvalid())) { return false; }

    ip4_addr_t ip {};
    ip.addr = socketaddress.addr.value(); // layout compatibile, vedi static_assert sopra

    err_t e = udp_bind(u, &ip, socketaddress.port);
    return (ERR_OK == e);
}

embot::net::eth::Port embot::net::lwip::UDPsocket::localport() const
{
    if (nullptr == u) { return 0; }   
    return u->local_port;       
}    

bool embot::net::lwip::UDPsocket::connect(const embot::net::eth::SocketAddress &socketaddress)
{
    if ((nullptr == u) || (!socketaddress.isvalid())) { return false; }

    ip4_addr_t ip {};
    ip.addr = socketaddress.addr.value();

    err_t e = udp_connect(u, &ip, socketaddress.port);
    return (ERR_OK == e);
}

void embot::net::lwip::UDPsocket::disconnect()
{
    if (nullptr != u)
    {
        udp_disconnect(u);
    }
}

bool embot::net::lwip::UDPsocket::recv(const embot::net::lwip::UDPsocket::OnRX &onrx)
{
    if (nullptr == u) { return false; }

    // onrx.callback puo' essere nullptr: e' un modo legittimo per "spegnere" la ricezione,
    // coerente con la firma di udp_recv_fn di lwIP che accetta anche NULL
    udp_recv(u, reinterpret_cast<udp_recv_fn>(onrx.callback), onrx.arg);
    
    return true;
}

bool embot::net::lwip::UDPsocket::send(const embot::net::lwip::Packet &packet, const embot::net::eth::SocketAddress &socketaddress)
{
    if ((nullptr == u) || (!packet.isvalid()) || (!socketaddress.isvalid())) { return false; }

    ip4_addr_t ip {};
    ip.addr = socketaddress.addr.value();

    err_t e = udp_sendto(u, packet.rawpbuf(), &ip, socketaddress.port);
    return (ERR_OK == e);
}

bool embot::net::lwip::UDPsocket::send(const Packet &packet)
{
    if ((nullptr == u) || (!packet.isvalid())) { return false; }

    err_t e = udp_send(u, packet.rawpbuf());
    return (ERR_OK == e);
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

