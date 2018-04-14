#include "base_inet_addr.h"
#include <stdio.h>

BASE_NAMESPACE_BEGIN_DECL

Inet_Addr::Inet_Addr( bool state )
{
    net_state = state;
    memset(&addr4_, 0, sizeof(struct sockaddr_in));
    memset(&addr6_, 0, sizeof(struct sockaddr_in6));

    // IPV4;
    addr4_.sin_family = AF_INET;
    addr4_.sin_addr.s_addr = htonl(INADDR_ANY);
    addr4_.sin_port = htons(0);

    // IPV6;
    addr6_.sin6_family = AF_INET6;
    struct in6_addr anyaddr = IN6ADDR_ANY_INIT;
    addr6_.sin6_addr = anyaddr;
    addr6_.sin6_port = htons(0);
}

Inet_Addr::Inet_Addr(const BASEOBJECT::Inet_Addr &addr)
{
    net_state   = addr.net_state;
    addr4_      = addr.addr4_;
    addr6_      = addr.addr6_;
}

Inet_Addr::Inet_Addr(const string& hostname, uint16_t port )
{
    memset( &addr4_ , 0 , sizeof(struct sockaddr_in  ) );
    memset( &addr6_ , 0 , sizeof(struct sockaddr_in6 ) );

    net_state = false;
    struct addrinfo* addrinfo_result;
    int error_result = getaddrinfo( hostname.c_str() , NULL , NULL , &addrinfo_result );
    if ( 0 != error_result )
    {
        freeaddrinfo( addrinfo_result );
        return;
    }
    const struct sockaddr* sa = addrinfo_result->ai_addr;
    switch ( addrinfo_result->ai_addr->sa_family )
    {
    case AF_INET6:
        net_state = true;
        break;
    default:
        break;
    }

    // IPV6 ÍøÂç×´Ì¬;
    if ( net_state )
    {
        addr6_.sin6_port    = htons( port );
        addr6_.sin6_family  = AF_INET6;
        ::inet_ntop( addr6_.sin6_family , &(((struct sockaddr_in6 *)sa)->sin6_addr) , (char*)hostname.c_str() , INET6_ADDRSTRLEN );
        ::inet_pton( addr6_.sin6_family , hostname.c_str() , &addr6_.sin6_addr );
    }
    else
    {
        addr4_.sin_family       = AF_INET;
        addr4_.sin_port         = htons( port );
        addr4_.sin_addr.s_addr  = inet_addr( hostname.c_str() );
        if ( addr4_.sin_addr.s_addr == -1 )
        {
            hostent* host = gethostbyname( hostname.c_str() );
            if ( NULL == host ) return;
            addr4_.sin_addr.s_addr = (*reinterpret_cast<uint32_t*>(host->h_addr_list[0]));
        }
    }
    freeaddrinfo( addrinfo_result );
}

BASEOBJECT::Inet_Addr::Inet_Addr(uint32_t ip, uint16_t port)
{
    net_state = false;
    memset(&addr4_, 0, sizeof(struct sockaddr_in));
    addr4_.sin_addr.s_addr  = htonl(ip);
    addr4_.sin_port         = htons(port);
    addr4_.sin_family       = AF_INET;
}

BASEOBJECT::Inet_Addr::Inet_Addr( in6_addr hostname , uint16_t port )
{
    #ifdef __IOS__
    net_state = true;
    memset(&addr6_, 0, sizeof(struct sockaddr_in6));
    uint16_t add_size = sizeof( addr6_.sin6_addr.__u6_addr.__u6_addr32 ) / sizeof( __uint32_t );
    for ( uint16_t i = 0; i < add_size; ++i )
    {
        addr6_.sin6_addr.__u6_addr.__u6_addr32[i] = htonl(hostname.__u6_addr.__u6_addr32[i]);
    }
    addr6_.sin6_port   = htons( port );
    addr6_.sin6_family = AF_INET6;
    #endif
}

Inet_Addr::~Inet_Addr()
{
}

bool Inet_Addr::is_null() const
{
    return net_state ? ((ntohs(addr4_.sin_port) == 0)) : ((ntohs(addr6_.sin6_port) == 0));
}


bool BASEOBJECT::Inet_Addr::operator!=( const Inet_Addr& addr ) const
{
    return !((*this) == addr);
}

bool BASEOBJECT::Inet_Addr::operator <(const Inet_Addr& addr) const
{
    // IPV4;
    if ( !net_state )
    {
        if(ntohl(addr4_.sin_addr.s_addr) < ntohl(addr.addr4_.sin_addr.s_addr))
            return true;
        else if(ntohl(addr4_.sin_addr.s_addr) > ntohl(addr.addr4_.sin_addr.s_addr))
            return false;
        else
        {
            return (ntohs(addr4_.sin_port) < ntohs(addr.addr4_.sin_port) ? true : false);
        }
    }
    // IPV6;
    #ifdef __IOS__
    uint16_t add_size = sizeof( addr6_.sin6_addr.__u6_addr.__u6_addr32 ) / sizeof( uint32_t );
    for ( uint16_t i = add_size - 1; i >= 0; --i )
    {
        if ( ntohl( addr6_.sin6_addr.__u6_addr.__u6_addr32[i] ) < ntohl( addr.addr6_.sin6_addr.__u6_addr.__u6_addr32[i] ) )
            return true;
        else if ( ntohl( addr6_.sin6_addr.__u6_addr.__u6_addr32[i] ) > ntohl( addr.addr6_.sin6_addr.__u6_addr.__u6_addr32[i] ) )
            return false;
    }
    return (ntohs(addr6_.sin6_port) < ntohs(addr.addr6_.sin6_port) ? true : false);
    #else
    return false;
    #endif
}

bool BASEOBJECT::Inet_Addr::operator >( const Inet_Addr& addr ) const
{
    // IPV4;
    if ( !net_state )
    {
        if(ntohl(addr4_.sin_addr.s_addr) > ntohl(addr.addr4_.sin_addr.s_addr))
            return true;
        else if(ntohl(addr4_.sin_addr.s_addr) < ntohl(addr.addr4_.sin_addr.s_addr))
            return false;
        else
        {
            return (ntohs(addr4_.sin_port) > ntohs(addr.addr4_.sin_port) ? true : false);
        }
    }

    // IPV6;
    #ifdef __IOS__
    uint16_t add_size = sizeof( addr6_.sin6_addr.__u6_addr.__u6_addr32 ) / sizeof( uint32_t );
    for ( uint16_t i = add_size - 1; i >= 0; --i )
    {
        if ( ntohl( addr6_.sin6_addr.__u6_addr.__u6_addr32[i] ) > ntohl( addr.addr6_.sin6_addr.__u6_addr.__u6_addr32[i] ) )
            return true;
        else if ( ntohl( addr6_.sin6_addr.__u6_addr.__u6_addr32[i] ) < ntohl( addr.addr6_.sin6_addr.__u6_addr.__u6_addr32[i] ) )
            return false;
    }
    return (ntohs(addr6_.sin6_port) > ntohs(addr.addr6_.sin6_port) ? true : false);
    #else
    return false;
    #endif
}

bool BASEOBJECT::Inet_Addr::operator==( const Inet_Addr& addr ) const
{
    // IPV4;
    if ( !net_state )
    {
        return (ntohl(addr4_.sin_addr.s_addr) == ntohl(addr.addr4_.sin_addr.s_addr) && ntohs(addr4_.sin_port) == ntohs(addr.addr4_.sin_port));
    }
    // IPV6;
    #ifdef __IOS__
    uint16_t add_size = sizeof( addr6_.sin6_addr.__u6_addr.__u6_addr32 ) / sizeof( __uint32_t );
    for ( uint16_t i = 0; i < add_size; ++i )
    {
        if ( ntohl( addr6_.sin6_addr.__u6_addr.__u6_addr32[i] ) != ntohl( addr.addr6_.sin6_addr.__u6_addr.__u6_addr32[i] ) )
        {
            return false;
        }
    }
    return (ntohs(addr6_.sin6_port) == ntohs(addr.addr6_.sin6_port));
    #else
    return false;
    #endif
}

const sockaddr* BASEOBJECT::Inet_Addr::get_addr( void ) const
{
    return ( net_state ? ( struct sockaddr* )( &addr6_ ): (struct sockaddr* )( &addr4_ ) );
}

sockaddr* BASEOBJECT::Inet_Addr::get_addr( void )
{
    return ( net_state ? ( struct sockaddr*)( &addr6_ ) : ( struct sockaddr* )( &addr4_ ) );
}

int32_t BASEOBJECT::Inet_Addr::get_addr_len(void) const
{
    return ( net_state ? sizeof( struct sockaddr_in6 ) : sizeof( struct sockaddr_in ) );
}

Inet_Addr& Inet_Addr::operator = (const Inet_Addr& addr)
{
    this->net_state = addr.net_state;
    this->addr4_    = addr.addr4_;
    this->addr6_    = addr.addr6_;
    return *this;
}

Inet_Addr& Inet_Addr::operator =(const sockaddr_in &addr)
{
    this->addr4_ = addr;
    return *this;
}

Inet_Addr& Inet_Addr::operator =(const sockaddr_in6 &addr6)
{
    this->addr6_ = addr6;
    return *this;
}

BASE_NAMESPACE_END_DECL
