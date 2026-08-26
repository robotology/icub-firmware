

/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_net_eth_packet.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - some checks
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - object
// --------------------------------------------------------------------------------------------------------------------

namespace embot::net::eth {
     
    Packet::Packet(size_t capacity) 
        : payload_(capacity ? new uint8_t[capacity] : nullptr), size_(0), capacity_(capacity), owns_(capacity != 0)
    {
    }

    Packet::~Packet()
    {
        release();
    }

    Packet::Packet(Packet&& other) noexcept
    {
        movefrom(other);
    }

    Packet& Packet::operator=(Packet&& other) noexcept
    {
        if(this != &other)
        {
            release();
            movefrom(other);
        }
        return *this;
    }

    bool Packet::load(size_t n, const void* src)
    {
        if(n > capacity_)
        {
            return false;
        }
        std::memcpy(payload_, src, n);
        size_ = n;
        return true;
    }

    void Packet::adopt(void* m, size_t s)
    {
        adopt(m, s, s);
    }

    void Packet::adopt(void* m, size_t capacity, size_t initialSize)
    {
        release();
        payload_  = static_cast<uint8_t*>(m);
        capacity_ = capacity;
        size_     = (initialSize <= capacity) ? initialSize : capacity;
        owns_     = false;
    }

    bool Packet::setsize(size_t n)
    {
        if(n > capacity_)
        {
            return false;
        }
        size_ = n;
        return true;
    }
    
    void Packet::setaddress(const SocketAddress &a)
    {
        address_ = a;
    }

    void Packet::release()
    {
        if(owns_ && payload_)
        {
            delete[] payload_;
        }
        payload_  = nullptr;
        size_     = 0;
        capacity_ = 0;
        owns_     = false;
    }

    void Packet::movefrom(Packet& other) noexcept
    {
        payload_  = other.payload_;
        size_     = other.size_;
        capacity_ = other.capacity_;
        owns_     = other.owns_;

        other.payload_  = nullptr;
        other.size_     = 0;
        other.capacity_ = 0;
        other.owns_     = false;
    }


} // namespace embot::net::eth {
    
    
// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

