#include "igtlinkclient.h"
#include "mainwindow.h"
#include "imageprocessor.h"
#include <QtDebug>
#include <QByteArray>
#include <QMatrix4x4>
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
    if (QDateTime::currentMSecsSinceEpoch() < lastRefreshTime + guiRefreshRateMs) {
        free(imageBuffer);
        return;
    }

    uchar * imageBufferCopy = malloc(sizeof(uchar) * imgSize.width() * imgSize.height());
    memcpy(imageBufferCopy, imageBuffer, imgSize.width() * imgSize.height());
    free(imageBuffer);

    QImage newImage((uchar *) imageBufferCopy, imgSize.width(), imgSize.height(), imgSize.width(), QImage::Format_Indexed8);
    newImage.setColorTable(grayScaleColorTable);
    emit imageReceived(newImage, state);
    lastRefreshTime = QDateTime::currentMSecsSinceEpoch();
}
