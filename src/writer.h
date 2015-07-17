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
    /** File contains indices of images frozen (interesting places) */
    QFile frozenSeqFile;
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
    /** position in the output data file where the number of frames must be entered */
    qint64 indexOfDimZ;

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
    void writeTransform(const char* name, const igtl::Matrix4x4 matrix);
    void closeSequence();
    void writeFooter();
    void writeFrozenIndex();
    int createOutDir();
    int getImageCounter() {return imageCounter; }
};

#endif // WIRTER_H
