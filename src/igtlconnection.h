#ifndef IGTLCONNECTION_H
#define IGTLCONNECTION_H

#include "igtlClientSocket.h"
#include "igtlSocket.h"

class IgtlConnection
{

    const char *_hostname;
    const int _port;
    // params for socket opening
    igtl::ClientSocket _socket;

public:
    IgtlConnection();
    ~IgtlConnection();
    igtl::Socket openSocket();

    // ready connection
    igtl::Socket * socket;
};

#endif // IGTLCONNECTION_H
