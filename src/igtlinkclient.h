#ifndef IGTLINKCLIENT_H
#define IGTLINKCLIENT_H

#include <QObject>
#include <QThread>
#include <QReadWriteLock>
#include <QFile>
#include <QStringList>
#include <QVector>
#include <QTextStream>
#include <QSettings>

#include "worker.h"
#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlTransformMessage.h"
#include "igtlPositionMessage.h"
#include "igtlImageMessage.h"
#include "igtlClientSocket.h"
#include "igtlStatusMessage.h"

class IGTLinkClient : public QObject
{
    Q_OBJECT
    QThread _workerThread;
    Worker* worker;
    IgtlConnection* connection;
protected:
    igtl::ClientSocket::Pointer _socket;
    QString _hostname;
    int _port;

public:
    enum ErrorType
    {
        UserInterrupt, ReceiveError
    };
    explicit IGTLinkClient(QObject *parent = 0);
    ~IGTLinkClient();
    void setup(QString hostname, int port = 18944);
    void setOutputFile(const char *filename, const char *ilist, const char *tlist, int chS = 1000);
public slots:
    void startReading();
    void handleTransform(const igtl::TransformMessage::Pointer& transMsg);
    void handleImage(const igtl::ImageMessage::Pointer& imgMsg);
    void stopReading();
private slots:
    void receiveStopSignal(int e);
signals:
    void startWorker();
    void stopped(ErrorType);
};

#endif // IGTLINKCLIENT_H
