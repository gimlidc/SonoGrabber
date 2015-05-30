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

/**
 * @brief Worker is responsible for socket reading
 *
 * Worker should run in separate thread. Worker is started through method start()
 * and stopped through method stop(). After start is called worker connects to Plus server
 * and load data. Data are parsed and stored to disk.
 */
class Worker : public QObject
{
    Q_OBJECT
    static bool Terminate;
    static QReadWriteLock Lock;

    IgtlConnection * connection;
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
    Worker(IgtlConnection * connection);
    ~Worker();
    /**
     * @brief setOutput configure what and where will be stored
     * @param dirName specify target directory for data storing. Directory should be empty or non-existent.
     * @param images specify labels of image data which will be parsed from incoming stream
     * @param transformations labels of transformation data (for parsing)
     * @param imagesInOneFile define how many images is stored into one output file
     */
    void setOutput(QString dirName, QStringList images, QStringList transformations, int imagesInOneFile);
    /**
     * @brief stop disconnect from server and flush data to HDD
     */
    static void stop();
    /**
     * @brief isRunning
     * @return true if worker is running, false otherwise (stopped/error)
     */
    static int isRunning();
public slots:
    /**
     * @brief start creates new connection (Plus server) and start to process incoming data
     */
    void start();
signals:
    /**
     * @brief transformReceived When transformation is correctly parsed this signal is emmited.
     * @param transMsg parsed message
     */
    void transformReceived(const igtl::TransformMessage::Pointer& transMsg);
    /**
     * @brief positionReceived When position is received signal is emmited
     * @param posMsg parsed position
     */
    void positionReceived(const igtl::PositionMessage::Pointer& posMsg);
    /**
     * @brief imageReceived when image is received signal is emmited
     * @param imgMsg parsed image
     */
    void imageReceived(const igtl::ImageMessage::Pointer& imgMsg);
    /**
     * @brief stopped is emmited when worker is stopped (connection closed).
     */
    void stopped(int);
};

#endif // WORKER_H
