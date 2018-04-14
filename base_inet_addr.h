// 定义IP地址封装类，实现IPV4地址的一些相关操作;

#ifndef __BASE_INET_ADDR_H
#define __BASE_INET_ADDR_H

#include "base_namespace.h"
#include "base_os.h"
#include "base_bin_stream.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std;

BASE_NAMESPACE_BEGIN_DECL

class Inet_Addr
{
public:
    Inet_Addr( bool state = false );
    Inet_Addr( const Inet_Addr& addr );
    Inet_Addr( const string& hostname , uint16_t port );
    Inet_Addr( in6_addr hostname , uint16_t port );
    Inet_Addr(uint32_t ip, uint16_t port);

    virtual ~Inet_Addr();

    bool            is_null() const;
    bool            is_net_ipv6( void ) const { return net_state; }

    int32_t         get_addr_len( void ) const;
    const sockaddr* get_addr( void )     const;
    sockaddr*       get_addr( void );


    Inet_Addr&      operator=(const Inet_Addr& addr);
    Inet_Addr&      operator=(const sockaddr_in& addr);
    Inet_Addr&      operator=(const sockaddr_in6& addr6);

    bool            operator==( const Inet_Addr& addr ) const;
    bool            operator!=( const Inet_Addr& addr ) const;
    bool            operator >( const Inet_Addr& addr ) const;
    bool            operator <( const Inet_Addr& addr ) const;

private:
    sockaddr_in     addr4_;         // IPV4网络地址;
    sockaddr_in6    addr6_;         // IPV6网络地址;
    bool            net_state;      // 是否IPV6网络;

};
BASE_NAMESPACE_END_DECL

#endif
