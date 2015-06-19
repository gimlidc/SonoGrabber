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
#include <QRgb>
#include <QImage>

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
    QVector<QRgb> grayScaleColorTable;
    qint64 lastRefreshTime;
    qint64 guiRefreshRateMs;
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
    explicit IGTLinkClient(SessionParams * connection, qint64 refreshRate = 200, QObject *parent = 0);
    ~IGTLinkClient();    
public slots:
    void startReading();
    void stopReading();
    void showImage(char * imageBuffer, QSize imgSize, QString state);
private slots:
    void receiveStopSignal(int e);
signals:
    void startWorker();
    void stopped(ErrorType);
    void imageReceived(QImage newImage, QString state);
};

#endif // IGTLINKCLIENT_H
