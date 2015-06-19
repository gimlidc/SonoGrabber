#include "sessionnamegenerator.h"
#include <QDir>
#include <QDebug>
#include <QDateTime>

QString SessionNameGenerator::generateSessionDirectory(QString dirPath)
{
    QDir dir(dirPath);
    QDateTime dateTime = QDateTime::currentDateTime();

    if (!dir.exists()) {
        qWarning() << "Specified directory does not exist!";
        return dateTime.toString("yyyy-MM-dd_hh-mm-ss");
    }

    QDir target(dirPath.append("/").append(dateTime.toString("yyyy-MM-dd_hh-mm-ss")));

    if (target.exists())
        return dateTime.toString("yyyy-MM-dd_hh-mm-ss.zzz");

    return target.dirName();
}

QString SessionNameGenerator::generateRawFileName(int fileNo)
{
    return QString("%1.raw").arg(fileNo,3,10,QChar('0'));
}

QString SessionNameGenerator::generateTransformPrefix(int imageNo)
{
    return QString("Seq_Frame%1_").arg(imageNo,4,10,QChar('0'));
}
