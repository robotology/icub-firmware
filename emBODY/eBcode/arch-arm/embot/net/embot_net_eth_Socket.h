
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// - include guard ----------------------------------------------------------------------------------------------------

#ifndef EMBOT_NET_ETH_SOCKET_H_
#define EMBOT_NET_ETH_SOCKET_H_


#include "embot_core.h"
#include "embot_net_eth.h"
#include "embot_net_eth_Packet.h"



namespace embot::net::eth {
    
    class theIPservice; // forward declatarion
    
#if 0
 Class `embot::net::eth::Socket`
 It is a UDP socket which always transmits and receives.
 It is meant to be used by ONE owner thread, which calls open(), close(), connect(), transmit(), 
 receive(), input() and setaction(). The IP service thread (which must have a priority higher than 
 the one of any owner thread) only calls the private addRXfifo() and remTXfifo().
#endif
    
    class Socket
    {
    public:

        struct Pipe
        {
            uint8_t  maxpackets {1};
            uint16_t maxsizeofpacket {1500};
        };

        struct Config
        {
            Pipe inputpipe {};
            Pipe outputpipe {};
            Config() = default;
        };
        
        struct Action
        {
            embot::core::Callback onrx {};  
            embot::core::Callback ontx {};            
        };
        
        struct Properties
        {
            embot::net::eth::Port localport {0};
            embot::net::eth::IPaddress bindingaddress {embot::net::eth::IPany};
            Action action {};
        };


        Socket(const Config &config);
        ~Socket();   

        // blocking: the call returns when ::theIPservice has attached the socket
        bool open(const Properties& props);
        // blocking: the call returns when ::theIPservice has detached the socket
        bool close();
        // blocking: it asks ::theIPservice to resolve the MAC address of the remote host (ARP)
        bool connect(const embot::net::eth::IPaddress &remoteaddress, embot::core::relTime tout = embot::core::time1second);
        
        // it copies the packet into the output pipe and alerts ::theIPservice, which sends it as soon as possible.
        // returns false if the socket is not open, if the pipe is full or if the packet is bigger than 
        // Config::outputpipe.maxsizeofpacket. It never blocks (but for the very short lock of the pipe).
        bool transmit(const embot::net::eth::Packet &packet); //, embot::core::relTime tout = embot::core::time1second);
        
        // number of packets in the input pipe
        size_t input() const;
        
        // it moves a packet from the input pipe into @packet. 
        // tout = 0 makes the call non-blocking. returns false if no packet is available within tout 
        // or if @packet.capacity() is smaller than the received packet (in such a case the packet is dropped).
        bool receive(embot::net::eth::Packet &packet, embot::core::relTime tout = 0);

        const Config & config() const;
        const Properties & properties() const;

        // it changes the actions of the socket (open or not). It is safe w.r.t. the IP service thread.
        void setaction(const Action &action);
        
    private:
        // for use only by embot::net::eth::theIPservice 
        friend class embot::net::eth::theIPservice;
    
        // it adds to the input pipe a packet received by the IP network, releases the token which counts 
        // the received packets and calls Properties::Action::onrx().
        // returns false if the pipe is full or if the packet is too big: the packet is dropped.
        bool addRXfifo(const embot::net::eth::Packet &packet); 

        // it removes a packet from the output pipe so that it can be sent to the IP network 
        // and calls Properties::Action::ontx(). returns false if the pipe is empty.
        bool remTXfifo(embot::net::eth::Packet &packet); 

    public:
        // moveable, non copiable
    
        // move-only: a Socket owns its pipes, so copying makes no sense.
        // moving is meaningful only for a closed socket: theIPservice keeps a pointer to an attached Socket.
        Socket(const Socket&) = delete;
        Socket& operator=(const Socket&) = delete;
        Socket(Socket&&) noexcept;
        Socket& operator=(Socket&&) noexcept;    
    
    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    };
    
} // namespace embot::net::eth {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
