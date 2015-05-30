#ifndef IGTLCONNECTION_H
#define IGTLCONNECTION_H

#include "igtlClientSocket.h"
#include "igtlSocket.h"
#include <QObject>
#include <QString>

class IgtlConnection : public QObject
{
    Q_OBJECT
    QString *_hostname;
    int _port;

public:
    IgtlConnection(QString hostname, const int port, QObject *parent = 0);
    ~IgtlConnection();
    /**
     * @brief getHostname
     * @return hostname of Plus server
     */
    QString getHostname();
    /**
     * @brief getPort
     * @return port where Plus server is listening
     */
    int getPort();
    /**
     * @brief openSocket creates new connection to Plus server
     * @return 0 if connection was successfully opened error code otherwise
     */
    int openSocket();
    /**
     * @brief closeSocket disconnect from the Plus server
     */
    void closeSocket();
    /** opened socket handler */
    igtl::ClientSocket::Pointer socket;
};

#endif // IGTLCONNECTION_H
