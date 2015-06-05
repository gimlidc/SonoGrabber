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

void SessionParams::setOutput(QString targetDirectory, QStringList imgList, QStringList transList, int imagesInOneFile)
{
    qDebug() << "Output directory:" << targetDirectory << "Grabbed images:" << imgList << "Grabbed transformations:" << transList << "Chunk size:" << imagesInOneFile;
    setOutputDir(targetDirectory);
    chunkSize = imagesInOneFile;
    images = QStringList(imgList);
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

QStringList SessionParams::getTransNames()
{
    return transformations;
}

int SessionParams::getPort()
{
    return _port;
}

QString SessionParams::getHeaderFileName()
{
    return outDir.absolutePath() + "/header.mhd";
}

QString SessionParams::getRawFileName(int fileNo)
{
    return outDir.absolutePath() + "/" + SessionNameGenerator::generateRawFileName(fileNo);
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
