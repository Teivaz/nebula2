//------------------------------------------------------------------------------
//  nipcaddress.cc
//  (C) 2003 RadonLabs GmbH
//------------------------------------------------------------------------------
#include "kernel/nipcaddress.h"
#include "util/nhashtable.h"

//------------------------------------------------------------------------------
/**
*/
nIpcAddress::nIpcAddress() :
    hostName("localhost"),
    addrStructValid(false),
    ipAddrValid(false),
    portNumValid(false),
    portNum(0)
{
    memset(&(this->addrStruct), 0, sizeof(this->addrStruct));
    memset(&(this->ipAddr), 0, sizeof(this->ipAddr));
}

//------------------------------------------------------------------------------
/**
*/
nIpcAddress::nIpcAddress(const char* hostName, const char* portName) :
    addrStructValid(false),
    ipAddrValid(false),
    portNumValid(false),
    portNum(0)
{
    memset(&(this->addrStruct), 0, sizeof(this->addrStruct));
    memset(&(this->ipAddr), 0, sizeof(this->ipAddr));
    this->SetHostName(hostName);
    this->SetPortName(portName);
}

//------------------------------------------------------------------------------
/**
*/
nIpcAddress::nIpcAddress(const char* hostName, short portNum) :
    addrStructValid(false),
    ipAddrValid(false),
    portNumValid(true)
{
    memset(&(this->addrStruct), 0, sizeof(this->addrStruct));
    memset(&(this->ipAddr), 0, sizeof(this->ipAddr));
    this->SetHostName(hostName);
    this->SetPortNum(portNum);
}

//------------------------------------------------------------------------------
/**
*/
nIpcAddress::~nIpcAddress()
{
    // empty
}

//------------------------------------------------------------------------------
/**
    Set the host name, this includes the special host names "any",
    "localhost", "broadcast", "self" and "inetself". The corresponding
    ip address can be queried using the GetIpAddress() method.
*/
void
nIpcAddress::SetHostName(const char* name)
{
    n_assert(name);
    this->hostName = name;
    this->addrStructValid = false;
    this->ipAddrValid = false;
}

//------------------------------------------------------------------------------
/**
    Get the host name.
*/
const char*
nIpcAddress::GetHostName() const
{
    return this->hostName.Get();
}

//------------------------------------------------------------------------------
/**
    Set the port name. The corresponding port number can be queried
    using the method GetPortNum().
*/
void
nIpcAddress::SetPortName(const char* name)
{
    n_assert(name);
    this->portName = name;
    this->addrStructValid = false;
    this->portNumValid = false;
}

//------------------------------------------------------------------------------
/**
    Get the port name.
*/
const char*
nIpcAddress::GetPortName() const
{
    return this->portName.Get();
}

//------------------------------------------------------------------------------
/**
    Return true if an ip address is an "internet" address (not a 
    Class A, B or C network address). If a machine is both connected
    to a LAN and the Internet, this method can be used to find the ip
    address that is visible from outside the LAN.
*/
bool
nIpcAddress::IsInternetAddress(const in_addr& addr)
{
    // generate address string from addr
    char* addrString = inet_ntoa(addr);

    // tokenize string into its members
    nArray<nString> tokens;
    nString str = addrString;
    str.Tokenize(".", tokens);
    n_assert(tokens.Size() == 4);
    int b1 = atoi(tokens[0].Get());
    int b2 = atoi(tokens[1].Get());
    int b3 = atoi(tokens[2].Get());
    // int b4 = atoi(tokens[3].Get());
    if ((b1 == 10) && (b2 >= 0) && (b2 <= 254))
    {
        // Class A net
        return false;
    }
    else if ((b1 == 172) && (b2 >= 16) && (b2 <= 31))
    {
        // Class B net
        return false;
    }
    else if ((b1 == 192) && (b2 == 168) && (b3 >= 0) && (b3 <= 254))
    {
        // Class C net
        return false;
    }
    else if (b1 < 224)
    {
        // unknown other local net type
        return false;
    }
    // an internet address
    return true;
}

//------------------------------------------------------------------------------
/**
    This updates the ip address field by converting the host name to 
    an ip address.
*/
bool
nIpcAddress::ValidateIpAddr()
{
    n_assert(!this->ipAddrValid);

    // first check for special case hostnames
    if (this->hostName == "any")
    {
        // the "ANY" address
        this->ipAddr.s_addr = htonl(INADDR_ANY);
    }
    else if (this->hostName == "broadcast")
    {
        // the "BROADCAST" address
        this->ipAddr.s_addr = htonl(INADDR_BROADCAST);
    }
    else if ((this->hostName == "self") || (this->hostName == "inetself"))
    {
        // the machine's ip address
        char localHostName[512];
        int err = gethostname(localHostName, sizeof(localHostName));
        if (0 != err)
        {
            // error getting host name
            return false;
        }

        // resolve host name
        struct hostent* he = gethostbyname(localHostName);
        if (0 == he)
        {
            // could not resolve own host name(!)
            return false;
        }        
        
        // initialize with the default address 
        this->ipAddr = *((struct in_addr *)he->h_addr);
        if (this->hostName == "inetself")
        {
            // if internet address requested, scan list of ip addresses
            // for a non-Class A,B or C network address
            int i;
            for (i = 0; (0 != he->h_addr_list[i]); i++)
            {
                if (this->IsInternetAddress(*((struct in_addr *)he->h_addr_list[i])))
                {
                    this->ipAddr = *((struct in_addr *)he->h_addr_list[i]);
                    break;
                }
            }
        }
    }
    else
    {
        struct hostent* he = gethostbyname(this->hostName.Get());
        if (0 == he)
        {
            // could not resolve host name!
            return false;
        }
        this->ipAddr = *((struct in_addr *)he->h_addr);
    }

    // also convert the ip address to a string
    char buf[64];
    sprintf(buf, "%s", inet_ntoa(this->ipAddr));
    this->ipAddrString = buf;

    // all ok
    this->ipAddrValid = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Validate the port number by converting the port name to a port number
    inside the allowed port range.
*/
bool
nIpcAddress::ValidatePortNum()
{
    n_assert(!this->portNumValid);
    this->portNum = ((short)hash(this->portName.Get(), PortRange)) + MinPortNum;
    this->portNumValid = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Validate the complete ip address struct.
*/
bool
nIpcAddress::ValidateAddrStruct()
{
    n_assert(!this->addrStructValid);

    // validate ip address if necessary
    if (!this->ipAddrValid)
    {
        if (!this->ValidateIpAddr())
        {
            return false;
        }
    }

    // validate port num if necessary
    if (!this->portNumValid)
    {
        if (!this->ValidatePortNum())
        {
            return false;
        }
    }
    
    // fill the address struct
    this->addrStruct.sin_family = AF_INET;
    this->addrStruct.sin_port   = htons(this->portNum);
    this->addrStruct.sin_addr   = this->ipAddr;

    this->addrStructValid = true;
    return true;
}

//------------------------------------------------------------------------------
/**
    Set the ip address structure. This will also initialize the host name,
    the port number and the port name.
*/
void
nIpcAddress::SetAddrStruct(const sockaddr_in& addr)
{
    this->addrStruct = addr;
    this->ipAddr  = this->addrStruct.sin_addr;
    this->portNum = ntohs(this->addrStruct.sin_port);
    this->addrStructValid = true;
    this->ipAddrValid = true;
    this->portNumValid = true;

    // initialize the host name, port number, and port name
    this->hostName = inet_ntoa(this->addrStruct.sin_addr);
    char buf[64];
    sprintf(buf, "%d", this->portNum);
    this->portName = buf;
}

//------------------------------------------------------------------------------
/**
    Get the ip address structure.
*/
const sockaddr_in&
nIpcAddress::GetAddrStruct()
{
    if (!this->addrStructValid)
    {
        this->ValidateAddrStruct();
    }
    return this->addrStruct;
}

//------------------------------------------------------------------------------
/**
    Get the ip address as string.
*/
const char*
nIpcAddress::GetIpAddrString()
{
    if (!this->ipAddrValid)
    {
        this->ValidateIpAddr();
    }
    return this->ipAddrString.Get();
}



//------------------------------------------------------------------------------
/**
    Sets the port num in host byte order.
*/
void 
nIpcAddress::SetPortNum(short portnum)
{
    this->portNum = portnum;
    this->portNumValid = true;
}
//------------------------------------------------------------------------------
/**
    Get the port num in host byte order.
*/
short
nIpcAddress::GetPortNum()
{
    if (!this->portNumValid)
    {
        this->ValidatePortNum();
    }
    return this->portNum;
}
