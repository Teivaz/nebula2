#ifndef N_IPCADDRESS_H
#define N_IPCADDRESS_H
//------------------------------------------------------------------------------
/**
    @class nIpcAddress

    Encapsulates a target address for the nIpc* class family. A target
    address is a string made of 2 parts, host name and port id. A host name
    is either a valid tcp/ip address in string form,
    a name which can be resolved by gethostbyname() (including "localhost"),
    or the special strings "self", "inetself", "any" and "broadcast". The port 
    id string is either a normal name, which will be hashed into a port number, 
    or if the string is valid integer number, this number will be used directly 
    as port number.

    Examples:

    localhost
    192.168.0.90
    flohbox
    broadcast

    Special host names and IP addresses:
    ====================================

    "localhost"     - this will translate to 127.0.0.1
    "any"           - this will translate to INADDR_ANY, which is 0.0.0.0
    "broadcast"     - this will translate to INADDR_BROADCAST, 
                      which is 255.255.255.255
    "self"          - this will translate to the first valid tcp/ip address
                      for this host (there may be more then one tcp/ip 
                      addresses bound to a hostname)
    "inetself"      - This will translate to the first valid tcp/ip address
                      for this host which is not a LAN address (which is not
                      a Class A, B or C network address). If no such exists,
                      the address will fallback to "self".

    (C) 2003 RadonLabs GmbH
*/
#include "kernel/ntypes.h"
#include "kernel/nsocketdefs.h"
#include "util/nstring.h"

//------------------------------------------------------------------------------
class nIpcAddress
{
public:
    /// default constructor
    nIpcAddress();
    /// constructor with address string
    nIpcAddress(const char* hostName, const char* portName);
    /// destructor
    ~nIpcAddress();
    /// set the host name
    void SetHostName(const char* name);
    /// get the host name
    const char* GetHostName() const;
    /// set the port name
    void SetPortName(const char* name);
    /// get the port name
    const char* GetPortName() const;
    /// set the socket address structure
    void SetAddrStruct(const sockaddr_in& addr);
    /// get the socket address structure
    const sockaddr_in& GetAddrStruct();
    /// get the ip address defined by the host name as string
    const char* GetIpAddrString();
    /// get the port number defined by the port name
    short GetPortNum();
    /// update address struct from the address string
    bool ValidateAddrStruct();
    /// update the ip address from the host name
    bool ValidateIpAddr();
    /// update the port number from the port name
    bool ValidatePortNum();

private:
    /// check if an ip address is a valid internet address (vs. LAN address)
    bool IsInternetAddress(const in_addr& addr);

    enum Enums
    {
        MinPortNum = N_SOCKET_MIN_PORTNUM,
        MaxPortNum = N_SOCKET_MAX_PORTNUM,
        PortRange = N_SOCKET_PORTRANGE,
    };

    nString hostName;
    nString portName;
    sockaddr_in addrStruct;
    in_addr ipAddr;
    nString ipAddrString;
    short portNum;

    bool addrStructValid;
    bool ipAddrValid;
    bool portNumValid;
};

//------------------------------------------------------------------------------
#endif
