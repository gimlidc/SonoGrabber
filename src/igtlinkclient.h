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
#include <QVector4D>

#include "worker.h"
#include "sessionparams.h"
#include "image.h"
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
    bool firstImage;
    uchar * imageBufferCopy;
    QImage newImage;
protected:
    igtl::ClientSocket::Pointer _socket;

public:
    /**
     * @brief ErrorType describes error when data parsing is broken.
     */
    enum ErrorType
    {
        UserInterrupt, RecordingStopped, ReceiveError, SocketOpenError
    };
    explicit IGTLinkClient(SessionParams * connection, qint64 refreshRate = 200, QObject *parent = 0);
    ~IGTLinkClient();

private:
    QVector4D *in;




public slots:
    void startReading();
    void stopReading();
    void showImage(char * imageBuffer, QSize imgSize, QString state);
    void rcvImgPosition(Image);
    void receivePos(QMatrix4x4 pos);
    void receiveFrozen(int imgNumber);
    void startRecord();
    void stopRecord();
private slots:    
    void receiveStopSignal(int e);
signals:
    void startWorker();
    void stopped(ErrorType);
    void imageReceived(QImage newImage);
    void stateChanged(QString state);
    void imgPosition(Image);
    void position(QMatrix4x4 pos);
    void frozen(int imgNumber);
    void startRecordSig();
    void stopRecordSig();
    void testWorker();
};

#endif // IGTLINKCLIENT_H
