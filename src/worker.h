#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QDir>
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
#include "sessionparams.h"
#include "writer.h"

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

    SessionParams * session;
    Writer * writer;
    QList<igtl::ImageMessage::Pointer> imgMsgList; // temporarily store image and transform message
    QList<igtl::TransformMessage::Pointer> transMsgList;
    QVector<double> imgTS;
    QVector<double> transTS;
    double lastStoredTS;
    bool frozenImageStored;
    QString currentState;

    int ReceiveTransform(igtl::Socket *socket, igtl::MessageHeader::Pointer& header);
    int ReceivePosition(igtl::Socket *socket, igtl::MessageHeader::Pointer& header);
    int ReceiveImage(igtl::Socket *socket, igtl::MessageHeader::Pointer& header);
    void writeTransformCropped(const igtl::TransformMessage::Pointer &transMsg);
    void flushData(double ts);
    void writeAndNotify(char * imgBuffer, QSize dimensions, bool isFrozen);
    const QString& updateState(bool isFrozen, bool isCropped);
    const QString& setState(const QString& state);
    const QString& getCurrentState();
    void setOutput(); // method can be called when session is fully defined
public:
    Worker(SessionParams * session);
    ~Worker();
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
     * @param state contains info about image - if it is frozen value is set to "Frozen" after call
     */
    void imageReceived(char * imageBuffer, QSize imgSize, QString state);
    /**
     * @brief stopped is emmited when worker is stopped (connection closed).
     */
    void stopped(int);
};

#endif // WORKER_H
