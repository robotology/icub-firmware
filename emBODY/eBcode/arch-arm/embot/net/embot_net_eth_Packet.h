
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef EMBOT_NET_ETH_PACKET_H_
#define EMBOT_NET_ETH_PACKET_H_


#include "embot_core.h"
#include "embot_net_eth.h"


namespace embot::net::eth {
            
    class Packet
    {
    public:
        
        // empty: does not own memory and capacity = size = 0
        Packet() = default;

        // owns memory: allocates capacity bytes, size = 0
        explicit Packet(size_t capacity);
    
        // just sugar syntax: Packet pkt = Packet::allocate(1500);
        static Packet allocate(size_t capacity) { return Packet(capacity); }

        ~Packet();

        // move-only
        Packet(Packet&& other) noexcept;
        Packet& operator=(Packet&& other) noexcept;
        Packet(const Packet&) = delete;
        Packet& operator=(const Packet&) = delete;

        // copies n bytes from src inside the buffer (its own or external, as long as n <= capacity).
        // returns false if n > capacity().
        bool load(size_t n, const void* src);

        // adopts a valid external buffer: size = capacity = s.
        // if Packet already own an internal buffer the internal is freed before the external is linked.
        void adopt(void* m, size_t s);

        // adopts a valid external buffer  w/ capacity bytes, but w/ an intialsize
        // of useful bytes (utile per chi crea un buffer vuoto
        // che verra' riempito in seguito, es. da un allocatore esterno).
        void adopt(void* m, size_t capacity, size_t initialsize);

        // tells that n bytes are valid 
        // returns false if n > capacity().
        bool setsize(size_t n);
        
        void setaddress(const SocketAddress &a);

        uint8_t*       data()           { return payload_; }
        const uint8_t* data()     const { return payload_; }
        size_t         size()     const { return size_; }
        size_t         capacity() const { return capacity_; }
        bool           owns()     const { return owns_; }
        
        const SocketAddress& address() const { return address_; }
        SocketAddress& address() { return address_; }

    private:
        void release();
        void movefrom(Packet& other) noexcept;

        uint8_t* payload_  = nullptr;
        size_t   size_     = 0;
        size_t   capacity_ = 0;
        bool     owns_     = false;
    
        SocketAddress address_ {};
    };   
    
} // namespace embot::net::eth {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
