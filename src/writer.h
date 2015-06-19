#ifndef WIRTER_H
#define WIRTER_H

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QSize>
#include <igtlTransformMessage.h>
#include <igtlImageMessage.h>

class Writer
{
    /** Output directory, should be empty or non-existent */
    QDir dir;
    /** File object which handles raw (image) data (.raw) */
    QFile rawFile;
    /** File object which handles the metaheader file (*.mhd) */
    QFile headerFile;
    /** number of files used for images */
    int fileCounter;
    /** number of currently stored images (in last file) */
    int imageCounter;
    /** Number of images in one file */
    int chunkSize;
    /** image size */
    int size[3];
    /** Header file stream */
    QTextStream tstr;

public:
    Writer(QDir targetDir, int chunkSize);
    ~Writer();

    void writeImage(char* buffer, QSize imgSize);
    void flushData(double ts);
    void closeFiles();
    void openHeaderFile();
    void writeHeader(const igtl::ImageMessage::Pointer &imgMsg);
    void startSequence(double timestamp);
    void writeTransform(const igtl::TransformMessage::Pointer &transMsg);
    void closeSequence();
    void writeFooter();
    int createOutDir();
};

#endif // WIRTER_H
