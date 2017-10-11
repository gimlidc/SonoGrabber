#include "writer.h"
#include "sessionnamegenerator.h"
#include <QDebug>
#include <QDir>

Writer::Writer(QDir targetDir, int chunkSize)
{
    dir = targetDir;
    headerFile.setFileName(dir.absolutePath() + "/" + "header.mhd");
    imageCounter = 0;
    fileCounter = 0;
    rawFile.setFileName(dir.absolutePath() + "/" + SessionNameGenerator::generateRawFileName(fileCounter));
    frozenSeqFile.setFileName(dir.absolutePath() + "/" + "frozen.txt");
    frozenSeqPosFile.setFileName(dir.absolutePath() + "/" + "frozenIdxPos.txt");
    this->chunkSize = chunkSize;
}

Writer::~Writer()
{

}

int Writer::createOutDir()
{
    if (dir.exists()) {
        qWarning() << "Directory exists data can be overwritten: " << dir.absolutePath();
    } else {
        qDebug() << "Creating target directory: " << dir.absolutePath();
        if (!QDir().mkpath(dir.absolutePath())) {
            return 1;
        }
    }
    return 0;
}

void Writer::openHeaderFile()
{
    qWarning() << "opening header file";
    if (!headerFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Cannot open file" << headerFile.fileName();
        return;
    } else {
        qDebug() << "Header file opened: " << headerFile.fileName();
    }
    tstr.setDevice(&headerFile);
    tstr.setRealNumberPrecision(16);
}

void Writer::closeFiles()
{
    writeFooter();
    rawFile.close();
    frozenSeqFile.close();
    frozenSeqPosFile.close();
    headerFile.close();
}

void Writer::startSequence(double timestamp)
{
    tstr << SessionNameGenerator::generateTransformPrefix(imageCounter) << "Timestamp = " << timestamp << "\n";
}

void Writer::writeTransform(const igtl::TransformMessage::Pointer &transMsg)
{
    tstr << SessionNameGenerator::generateTransformPrefix(imageCounter) << transMsg->GetDeviceName()
         << "Transform = ";

    igtl::Matrix4x4 matrix;

    transMsg->GetMatrix(matrix);
    tstr << matrix[0][0] << ' ' << matrix[0][1] << ' ' << matrix[0][2] << ' ' << matrix[0][3] << ' '
         << matrix[1][0] << ' ' << matrix[1][1] << ' ' << matrix[1][2] << ' ' << matrix[1][3] << ' '
         << matrix[2][0] << ' ' << matrix[2][1] << ' ' << matrix[2][2] << ' ' << matrix[2][3] << ' '
         << matrix[3][0] << ' ' << matrix[3][1] << ' ' << matrix[3][2] << ' ' << matrix[3][3] << '\n';

    tstr << SessionNameGenerator::generateTransformPrefix(imageCounter)
         << transMsg->GetDeviceName() << "TransformStatus = OK\n";
}

void Writer::writeTransform(const char *name, const igtl::Matrix4x4 matrix)
{
    tstr << SessionNameGenerator::generateTransformPrefix(imageCounter) << name
         << "Transform = ";

    tstr << matrix[0][0] << ' ' << matrix[0][1] << ' ' << matrix[0][2] << ' ' << matrix[0][3] << ' '
         << matrix[1][0] << ' ' << matrix[1][1] << ' ' << matrix[1][2] << ' ' << matrix[1][3] << ' '
         << matrix[2][0] << ' ' << matrix[2][1] << ' ' << matrix[2][2] << ' ' << matrix[2][3] << ' '
         << matrix[3][0] << ' ' << matrix[3][1] << ' ' << matrix[3][2] << ' ' << matrix[3][3] << '\n';

    tstr << SessionNameGenerator::generateTransformPrefix(imageCounter)
         << name << "TransformStatus = OK\n";
}

void Writer::writeImage(char* buffer, QSize imgSize)
{
    if (!imageCounter) {
        size[0] = imgSize.width();
        size[1] = imgSize.height();
    } else {
        if (size[0] != imgSize.width() || size[1] != imgSize.height()) {
            qWarning() << "Image size has changed! Data may be corrupted.";
        }
    }
    if (!rawFile.isOpen()) {
        if (!rawFile.open(QIODevice::WriteOnly)) {
            qWarning() << "Image file could not be opened:" << rawFile.fileName();
        }
    }
    rawFile.write((const char *)buffer, imgSize.width() * imgSize.height());
    rawFile.flush();
    qDebug() << "write image";
}

void Writer::closeSequence()
{
    tstr.flush();
    imageCounter++;
    if (chunkSize && (imageCounter % chunkSize == 0)) {
        rawFile.close();
        fileCounter++;
        rawFile.setFileName(dir.absolutePath() + "/" + SessionNameGenerator::generateRawFileName(fileCounter));
    }
}

void Writer::writeHeader(const igtl::ImageMessage::Pointer &imgMsg)
{
    if (imageCounter) { // header already written
        return;
    }
    // Retrive the image data
    float spacing[3]; // spacing (mm/pixel)
    int svsize[3]; // sub-volume size
    int svoffset[3]; // sub-volume offset
    int scalarType; // scalar type

    scalarType = imgMsg->GetScalarType();
    imgMsg->GetDimensions(size);
    imgMsg->GetSpacing(spacing);
    imgMsg->GetSubVolume(svsize, svoffset);

    tstr << "ObjectType = Image\n"
       << "NDims = 3\n"
       << "DimSize = "; // << size[0] << ' ' << size[1] << ' ';
    tstr.flush();
    indexOfDimZ = tstr.pos();
    tstr << "                         \n"
       << "AnatomicalOrientation = RAI\n"
       << "BinaryData = True\n"
       << "BinaryDataByteOrderMSB = False\n"
       << "CenterOfRotation = 0 0 0\n"
       << "CompressedData = False\n"
       << "ElementNumberOfChannels = 1\n"
       << "Offset = 0 0 0\n"
       << "TransformMatrix = 1 0 0 0 1 0 0 0 1\n"
       << "UltrasoundImageOrientation = MF\n"
       << "UltrasoundImageType = BRIGHTNESS\n"
       << "ElementType = MET_UCHAR\n"
       << "ElementSpacing = 1 1 1\n"; //<< "ElementSpacing = " << spacing[0] << ' ' << spacing[1] << ' ' << spacing[2] << '\n'
}

void Writer::writeFooter()
{
    //tstr << "DimSize = " << size[0] << ' ' << size[1] << ' ' << imageCounter << '\n'
    if (imageCounter == 0) { // header not written
        return;
    }
    tstr << "ElementDataFile = "; // LIST" << '\n';
    for (int i = 0; i <= fileCounter; ++i) {
        tstr << SessionNameGenerator::generateRawFileName(i) << ' ';
    }
    tstr.seek(indexOfDimZ);
    tstr  << size[0] << ' ' << size[1] << ' ' << imageCounter;
    tstr.flush();
}

void Writer::writeFrozenIndex()
{
    if (!frozenSeqFile.isOpen()) {
        if (!frozenSeqFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning() << "Cannot open file" << frozenSeqFile.fileName();
            return;
        }
    }

    frozenSeqFile.write(QString::number(imageCounter).append("\n").toLatin1());
    frozenSeqFile.flush();
}

void Writer::writeFrozenIndexAndPos(QVector4D pos)
{
    if (!frozenSeqPosFile.isOpen()) {
        if (!frozenSeqPosFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning() << "Cannot open file" << frozenSeqPosFile.fileName();
            return;
        }
    }

    QTextStream out(&frozenSeqPosFile);
    out << imageCounter << " " << pos.x() << " " << pos.y() << " " << pos.z() << "\n";
//    frozenSeqFile.write(QString::number(imageCounter).append(" ").toLatin1());
    out.flush();
//    frozenSeqFile.flush();

}
