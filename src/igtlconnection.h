#ifndef IGTLCONNECTION_H
#define IGTLCONNECTION_H

#include "igtlClientSocket.h"
#include "igtlSocket.h"
#include <QString>

class IgtlConnection
{

    QString _hostname;
    int _port;

public:
    IgtlConnection(QString hostname, const int port);
    ~IgtlConnection();
    int openSocket();
    void closeSocket();
    // opened socket
    igtl::ClientSocket * socket;
};

#endif // IGTLCONNECTION_H
