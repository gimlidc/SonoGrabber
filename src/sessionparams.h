#ifndef IGTLCONNECTION_H
#define IGTLCONNECTION_H

#include "igtlClientSocket.h"
#include "igtlSocket.h"
#include <QObject>
#include <QString>
#include <QDir>

class SessionParams : public QObject
{
    Q_OBJECT
    QString *_hostname;
    int _port;
    QDir outDir;
    QStringList images, transformations;
    int chunkSize;

public:
    SessionParams(QString hostname, const int port, QObject *parent = 0);
    ~SessionParams();
    /**
     * @brief setOutputDir configure target directory for session saving
     * @param dirName full path of target directory
     */
    void setOutputDir(QString dirName);
    /**
     * @brief setOutput configure what and where will be stored
     * @param dirName specify target directory for data storing. Directory should be empty or non-existent.
     * @param images specify labels of image data which will be parsed from incoming stream
     * @param transformations labels of transformation data (for parsing)
     * @param imagesInOneFile define how many images is stored into one output file
     */
    void setOutput(QString dirName, QStringList images, QStringList transformations, int imagesInOneFile);
    /**
     * @brief getChunkSize defines size of file size in number of images stored in
     * @return number of images stored in one file for this session. Return -1 if not set.
     */
    int getChunkSize();
    /**
     * @brief getHostname
     * @return hostname of Plus server
     */
    QString getHostname();
    /**
     * @brief getOutDir output directory of the session
     * @return output directory for this session or tmp dir if session is not correctly configured.
     */
    QDir getOutDir();
    /**
     * @brief getImageNames
     * @return names of images which will be parsed in the session
     */
    QStringList getImageNames();
    /**
     * @brief getTransNames
     * @return names of transformations which will be parsed in the session
     */
    QStringList getTransNames();
    /**
     * @brief getPort
     * @return port where Plus server is listening
     */
    int getPort();
    /**
     * @brief openSocket creates new connection to Plus server
     * @return 0 if connection was successfully opened error code otherwise
     */
    int openSocket();
    /**
     * @brief closeSocket disconnect from the Plus server
     */
    void closeSocket();
    /** opened socket handler */
    igtl::ClientSocket::Pointer socket;
};

#endif // IGTLCONNECTION_H
