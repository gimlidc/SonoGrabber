#ifndef IGTLCONNECTION_H
#define IGTLCONNECTION_H

#include "igtlClientSocket.h"
#include "igtlSocket.h"
#include <QObject>
#include <QString>
#include <QDir>
#include <QRect>

class SessionParams : public QObject
{
    Q_OBJECT
    /** Plus server hostname */
    QString *_hostname;
    /** Plus server port */
    int _port;
    /** Directory where data files will be stored */
    QDir outDir;
    /** Configuration of parser - defines which images and transformations from stream will be parsed */
    QStringList images, transformations;
    /** Number of images stored in one file */
    int chunkSize;
    /** Number of already closed files. */
    int fileCounter;
    /** Maximum frames per second stored in file */
    int fps;
    /** File object which handles raw (image) data (.raw) */
    QFile rawFile;
    /** File object which handles the metaheader file (*.mhd) */
    QFile headerFile;
    /** Cropping of the image */
    QRect crop;
    /** Freeze mark in the image at this position */
    QRect freeze;

public:
    SessionParams(QString hostname, const int port, QObject *parent = 0);
    ~SessionParams();
    void setFreezePosition(QRect freezeMarkPosition);
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
     * @param frameRate maximum frame rate for image storage
     */
    void setOutput(QString dirName, QStringList images, QRect crop, QStringList transformations, int imagesInOneFile, int frameRate);
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
     * @brief getFps frames per second is more than original pictures (some are the same). This
     * will hard reduce count of stored images.
     * @return maximum frame rate for store
     */
    int getFps();
    /**
     * @brief getImageNames
     * @return names of images which will be parsed in the session
     */
    QStringList getImageNames();
    /**
     * @brief getCrop image subsection
     * @return crop area of the image
     */
    QRect getCrop();
    /**
     * @brief getFreeze position of freeze mark in picture
     * @return rectangle where freeze mark appears in coordinates of full picture
     */
    QRect getFreeze();
    /**
     * @brief shouldCrop return if cropping is set
     * @return true if cropping was set and is valid otherwise false
     */
    bool shouldCrop(QSize imgSize);
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
