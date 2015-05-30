#include "igtlconnection.h"
#include "igtlClientSocket.h"
#include <QDebug>

IgtlConnection::IgtlConnection(QString hostname, const int port, QObject *parent) : QObject(parent)
{
    _hostname = new QString(hostname);
    _port = port;
    socket = igtl::ClientSocket::New();
}

IgtlConnection::~IgtlConnection()
{
    //delete _hostname;
}

QString IgtlConnection::getHostname() {
    return *_hostname;
}

int IgtlConnection::getPort() {
    return _port;
}

int IgtlConnection::openSocket()
{
    qWarning()  << "connect" << getHostname() << ":" << getPort();
    int r = socket->ConnectToServer(getHostname().toStdString().c_str(), getPort());

    if (r != 0)
      qWarning() << "Cannot connect to the server.";

    return r;
}

void IgtlConnection::closeSocket()
{
    socket->CloseSocket();
}
