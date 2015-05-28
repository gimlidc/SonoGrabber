#include "igtlconnection.h"
#include "igtlClientSocket.h"
#include <QDebug>

IgtlConnection::IgtlConnection(QString hostname, const int port)
{
    this->_hostname = hostname;
    this->_port = port;
    this->socket = igtl::ClientSocket::New();
}

IgtlConnection::~IgtlConnection()
{

}

int IgtlConnection::openSocket()
{
    int r = socket->ConnectToServer(_hostname.toLatin1(), _port);

    if (r != 0)
    {
      qWarning() << "Cannot connect to the server.";
    }
    return r;
}

void IgtlConnection::closeSocket()
{
    socket->CloseSocket();
}
