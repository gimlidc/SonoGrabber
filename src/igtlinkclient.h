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
#include "igtlconnection.h"
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
    explicit IGTLinkClient(IgtlConnection * connection, QObject *parent = 0);
    ~IGTLinkClient();
    /**
     * @brief setOutput configure output of the session
     * @param dirName directory where output will be stored. Directory should be empty or non-existent.
     * @param images set of image labels parsed from the stream (stored into output)
     * @param transformations set of transformation labels parsed from the stream
     * @param imagesInOneFile number of images stored into one file (prevents huge files)
     */
    void setOutput(QString dirName, QStringList images, QStringList transformations, int imagesInOneFile);
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
