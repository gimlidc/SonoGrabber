#include "igtlinkclient.h"
#include <QtDebug>
#include <QByteArray>
#include <QMatrix4x4>


QReadWriteLock Worker::Lock;
bool Worker::Terminate = true;

IGTLinkClient::IGTLinkClient(QObject *parent) : QObject(parent)
{
    worker = new Worker(connection);
    worker->moveToThread(&_workerThread);
    qRegisterMetaType<igtl::TransformMessage::Pointer>("igtl::TransformMessage::Pointer");
    qRegisterMetaType<igtl::ImageMessage::Pointer>("igtl::ImageMessage::Pointer");
    connect(&_workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &IGTLinkClient::startWorker, worker, &Worker::start);
//    connect(worker, &Worker::transformReceived, this, &IGTLinkClient::handleTransform);
//    connect(worker, &Worker::imageReceived, this, &IGTLinkClient::handleImage);
    connect(worker, &Worker::stopped, this, &IGTLinkClient::receiveStopSignal);

    setup("localhost");

    _workerThread.start();

}

IGTLinkClient::~IGTLinkClient()
{

    stopReading();
    _workerThread.quit();
    _workerThread.wait();
}

void IGTLinkClient::setup(QString hostname, int port)
{
    connection = new IgtlConnection(hostname, port);
}

void IGTLinkClient::startReading()
{
    emit startWorker();
}

void IGTLinkClient::stopReading()
{
    Worker::stop();
}

void IGTLinkClient::receiveStopSignal(int e)
{
    emit stopped((ErrorType) e);
}

void IGTLinkClient::setOutputFile(const char *filename, const char *ilist, const char *tlist, int chS)
{
    worker->setOutputFile(filename, ilist, tlist, chS);
}

void IGTLinkClient::handleTransform(const igtl::TransformMessage::Pointer &transMsg)
{
    //qDebug() << "Transform received.";
    //qDebug() << transMsg->GetDeviceName();
}

void IGTLinkClient::handleImage(const igtl::ImageMessage::Pointer &imgMsg)
{
    //qDebug() << "Image received.";
    //qDebug() << imgMsg->GetDeviceName();
}
