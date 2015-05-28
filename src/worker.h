#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QVector>
#include <QReadWriteLock>
#include "igtlImageMessage.h"
#include "igtlTransformMessage.h"
#include "igtlSocket.h"
#include "igtlPositionMessage.h"
#include "igtlconnection.h"

class Worker : public QObject
{
    Q_OBJECT
    static bool Terminate;
    static QReadWriteLock Lock;

    IgtlConnection* connection;
    QString _filename;
    QFile rawFile; // File object which handles raw (image) data (.raw)
    QFile headerFile;  // File object which handles the metaheader file (*.mhd)
    QTextStream tstr;
    bool _writeFlag; // Flag to determine if the received stream of messages should be stored in a
    int imageCounter;
    int fileCounter;
    int chunkSize;
    int size[3];          // image dimension
    QStringList transNameList, imgNameList; // list of transform and image names, which we want to
    QList<igtl::ImageMessage::Pointer> imgMsgList; // temporarily store image and transform message
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
    Worker(IgtlConnection* connection);
    ~Worker();
    void setOutputFile(const char *filename, const char *ilist, const char *tlist, int chS);
    static void stop();
    static int isRunning();
public slots:
    void start();
signals:
    void transformReceived(const igtl::TransformMessage::Pointer& transMsg);
    void positionReceived(const igtl::PositionMessage::Pointer& posMsg);
    void imageReceived(const igtl::ImageMessage::Pointer& imgMsg);
    void stopped(int);
};

#endif // WORKER_H
