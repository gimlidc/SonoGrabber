#include "igtlinkclient.h"
#include <QtDebug>
#include <QByteArray>
#include <QMatrix4x4>
#include <QPixmap>


QReadWriteLock Worker::Lock;
bool Worker::Terminate = true;

IGTLinkClient::IGTLinkClient(SessionParams * connection, QObject *parent) : QObject(parent)
{
    worker = new Worker(connection);
    worker->moveToThread(&_workerThread);
    qRegisterMetaType<igtl::TransformMessage::Pointer>("igtl::TransformMessage::Pointer");
    qRegisterMetaType<igtl::ImageMessage::Pointer>("igtl::ImageMessage::Pointer");
    connect(&_workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &IGTLinkClient::startWorker, worker, &Worker::start);
//    connect(worker, &Worker::transformReceived, this, &IGTLinkClient::handleTransform);
    connect(worker, &Worker::imageReceived, this, &IGTLinkClient::handleImage);
    connect(worker, &Worker::stopped, this, &IGTLinkClient::receiveStopSignal);

    _workerThread.start();

}

IGTLinkClient::~IGTLinkClient()
{
    stopReading();
    _workerThread.quit();
    _workerThread.wait();
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

void IGTLinkClient::handleTransform(const igtl::TransformMessage::Pointer &transMsg)
{
    //qDebug() << "Transform received.";
    //qDebug() << transMsg->GetDeviceName();
}

void IGTLinkClient::handleImage(const igtl::ImageMessage::Pointer &imgMsg)
{
    qDebug() << "Image received. Whole size with header: " << imgMsg->GetBodySizeToRead();
    int resolution[3];
    imgMsg->GetDimensions(resolution);
    qDebug() << "Image dimensions:" << resolution[0] << "x" << resolution[1] << "x" << resolution[2];
    int size = imgMsg->GetImageSize();

    uchar * buffer = (uchar *)imgMsg->GetPackBodyPointer() + imgMsg->GetPackBodySize() - size;

    emit notifyNewImage(resolution[0], resolution[1], buffer);
}
