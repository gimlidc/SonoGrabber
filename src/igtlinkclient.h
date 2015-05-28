#ifndef IGTLINKCLIENT_H
#define IGTLINKCLIENT_H

#include <QObject>
#include <QThread>
#include <QReadWriteLock>
#include <QFile>
#include <QStringList>
#include <QVector>
#include <QTextStream>

#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlTransformMessage.h"
#include "igtlPositionMessage.h"
#include "igtlImageMessage.h"
#include "igtlClientSocket.h"
#include "igtlStatusMessage.h"

class Worker;

class IGTLinkClient : public QObject
{
    Q_OBJECT
    QThread _workerThread;
    Worker* worker;
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
    void setup(const char *hostname, int port = 18944);
    int open(const char *hostname, int port);
    int open();
    void close();
    void setOutputFile(const char *filename, const char *ilist, const char *tlist, int chS = 1000);
public slots:
    void startReading();
    void handleTransform(const igtl::TransformMessage::Pointer& transMsg);
    void handleImage(const igtl::ImageMessage::Pointer& imgMsg);
    void stopReading();
private slots:
    void receiveStopSignal(int e);
signals:
    void startWorker(igtl::Socket *);
    void stopped(ErrorType);
};

class Worker : public QObject
{
    Q_OBJECT
    static bool Terminate;
    static QReadWriteLock Lock;

    QString _filename;
    QFile rawFile; // File object which handles raw (image) data (.raw)
    QFile headerFile;  // File object which handles the metaheader file (*.mhd)
    QTextStream tstr;
    bool _writeFlag; // Flag to determine if the received stream of messages should be stored in a file
    int imageCounter;
    int fileCounter;
    int chunkSize;
    int size[3];          // image dimension
    QStringList transNameList, imgNameList; // list of transform and image names, which we want to store
    QList<igtl::ImageMessage::Pointer> imgMsgList; // temporarily store image and transform messages to collect all in transNameList and imgNameList with the same time stamp.
    QList<igtl::TransformMessage::Pointer> transMsgList;
    QVector<double> imgTS;
    QVector<double> transTS;

    void writeHeader(const igtl::ImageMessage::Pointer &imgMsg);
    void writeFooter();
    int saveImage(const igtl::ImageMessage::Pointer &imgMsg);
    int saveTransform(const igtl::TransformMessage::Pointer &transMsg);
    void flushData(double ts);
    int ReceiveTransform(igtl::Socket *socket, igtl::MessageHeader::Pointer& header);
    int ReceivePosition(igtl::Socket *socket, igtl::MessageHeader::Pointer& header);
    int ReceiveImage(igtl::Socket *socket, igtl::MessageHeader::Pointer& header);
    void closeFiles();
    void openHeaderFile();
public:
    void setOutputFile(const char *filename, const char *ilist, const char *tlist, int chS);
    static void stop();
    static int isRunning();
public slots:
    void listen(igtl::Socket *socket);
signals:
    void transformReceived(const igtl::TransformMessage::Pointer& transMsg);
    void positionReceived(const igtl::PositionMessage::Pointer& posMsg);
    void imageReceived(const igtl::ImageMessage::Pointer& imgMsg);
    void stopped(int);
};

#endif // IGTLINKCLIENT_H
