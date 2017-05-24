#include "igtlinkclient.h"
#include "mainwindow.h"
#include "imageprocessor.h"
#include <QtDebug>
#include <QDebug>
#include <QByteArray>
#include <QMatrix4x4>
#include <QVector4D>
#include <QPixmap>
#include <QVector>
#include <QRgb>
#include <QImage>
#include <QDateTime>

QReadWriteLock Worker::Lock;
bool Worker::Terminate = true;

IGTLinkClient::IGTLinkClient(SessionParams * connection, qint64 refreshRate, QObject *parent) : QObject(parent)
{
    worker = new Worker(connection);
    in = new QVector4D(connection->getCrop().x(), connection->getCrop().y(), 0, 1);
    worker->moveToThread(&_workerThread);

    qRegisterMetaType<igtl::TransformMessage::Pointer>("igtl::TransformMessage::Pointer");
    qRegisterMetaType<igtl::ImageMessage::Pointer>("igtl::ImageMessage::Pointer");
    connect(&_workerThread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &IGTLinkClient::startWorker, worker, &Worker::start);
//    connect(worker, &Worker::transformReceived, this, &IGTLinkClient::handleTransform);
    lastRefreshTime = QDateTime::currentMSecsSinceEpoch();
    guiRefreshRateMs = refreshRate;
    connect(worker, &Worker::imageReceived, this, &IGTLinkClient::showImage);
    connect(worker, &Worker::stopped, this, &IGTLinkClient::receiveStopSignal);
    connect(worker, &Worker::position, this, &IGTLinkClient::receivePos);    

    firstImage = true;
    for (int i = 0; i < 256; i++) {
        grayScaleColorTable.push_back(QColor(i, i, i).rgb());
    }

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

void IGTLinkClient::showImage(char * imageBuffer, QSize imgSize, QString state)
{
    if (QDateTime::currentMSecsSinceEpoch() < lastRefreshTime + guiRefreshRateMs && state != "FROZEN") {
        if (state == "CROPPED")
            free(imageBuffer);
        return;
    }
    if (imageBuffer == NULL) {
        emit stateChanged(state);
        return;
    }

    if (firstImage) {
        imageBufferCopy = (uchar *)malloc(sizeof(uchar) * imgSize.width() * imgSize.height());
        newImage = QImage(imgSize, QImage::Format_Indexed8);
        newImage.setColorTable(grayScaleColorTable);
        firstImage = false;
    }
    memcpy(imageBufferCopy, imageBuffer, imgSize.width() * imgSize.height());
    if (state == "CROPPED")
        free(imageBuffer);
    newImage = QImage(imageBufferCopy, imgSize.width(), imgSize.height(), imgSize.width(), QImage::Format_Indexed8);
    //newImage.fromData(imageBufferCopy, imgSize.width() * imgSize.height());
    emit imageReceived(newImage);
    lastRefreshTime = QDateTime::currentMSecsSinceEpoch();
}

void IGTLinkClient::receivePos(QMatrix4x4 transform)
{
    emit position(transform);
}
