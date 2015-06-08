#ifndef IGTLINKCLIENT_H
#define IGTLINKCLIENT_H

#include <QObject>
#include <QThread>
#include <QReadWriteLock>
#include <QFile>
#include <QStringList>
#include <QVector>
#include <QByteArray>
#include <QTextStream>
#include <QSettings>

#include "worker.h"
#include "sessionparams.h"
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
protected:
    igtl::ClientSocket::Pointer _socket;

public:
    /**
     * @brief ErrorType describes error when data parsing is broken.
     */
    enum ErrorType
    {
        UserInterrupt, ReceiveError
    };
    explicit IGTLinkClient(SessionParams * connection, QObject *parent = 0);
    ~IGTLinkClient();
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
    void notifyNewImage(int width, int height, const uchar * intensities);
};

#endif // IGTLINKCLIENT_H
