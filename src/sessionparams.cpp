#include "sessionparams.h"
#include "igtlClientSocket.h"
#include "sessionnamegenerator.h"
#include <QDebug>

SessionParams::SessionParams(QString hostname, const int port, QObject *parent) : QObject(parent)
{
    _hostname = new QString(hostname);
    _port = port;
    socket = igtl::ClientSocket::New();
    chunkSize = -1;
    outDir = QDir().tempPath();
}

SessionParams::~SessionParams()
{
    delete _hostname;
}

void SessionParams::setOutputDir(QString targetDirectory)
{
    outDir = QDir(targetDirectory);
}

void SessionParams::setOutput(QString targetDirectory, QStringList imgList, QRect cropping, QStringList transList, int imagesInOneFile)
{
    qDebug() << "Output directory:" << targetDirectory << "Grabbed images:" << imgList << "Grabbed transformations:" << transList << "Chunk size:" << imagesInOneFile;
    setOutputDir(targetDirectory);
    chunkSize = imagesInOneFile;
    images = QStringList(imgList);
    crop = QRect(cropping);
    transformations = QStringList(transList);
}

int SessionParams::getChunkSize()
{
    return chunkSize;
}

QString SessionParams::getHostname()
{
    return *_hostname;
}

QDir SessionParams::getOutDir()
{
    return outDir;
}

QStringList SessionParams::getImageNames()
{
    return images;
}

QRect SessionParams::getCrop()
{
    return crop;
}

bool SessionParams::shouldCrop(QSize imgSize)
{
    if (crop.width() == 0 || crop.height() == 0) {
        // invalid cropping was set
        return false;
    }
    if (crop.x() + crop.width() > imgSize.width()) {
        // Crop rectangle is bigger than image
        return false;
    }
    if (crop.y() + crop.height() > imgSize.height()) {
        // Crop rectangle is bigger than image
        return false;
    }
    return true;
}

QStringList SessionParams::getTransNames()
{
    return transformations;
}

int SessionParams::getPort()
{
    return _port;
}

int SessionParams::openSocket()
{
    qDebug() << "Trying to connect an IGT server:" << getHostname() << ":" << getPort();
    int r = socket->ConnectToServer(getHostname().toStdString().c_str(), getPort());

    if (r != 0)
      qWarning() << "Cannot connect to the server.";

    return r;
}

void SessionParams::closeSocket()
{
    socket->CloseSocket();
}
