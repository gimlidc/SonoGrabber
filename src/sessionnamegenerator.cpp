#include "sessionnamegenerator.h"
#include <QDir>
#include <QDebug>
#include <QDateTime>

QString SessionNameGenerator::DateTimeFormat1 = "yyyy-MM-dd_hh-mm-ss";
QString SessionNameGenerator::DateTimeFormat2 = "yyyy-MM-dd_hh-mm-ss.zzz";

QString SessionNameGenerator::generateSessionDirectory(QString dirPath, int index, QString& strDateTime)
{
    QDir dir(dirPath);
    QDateTime dateTime = QDateTime::currentDateTime();

    strDateTime = dateTime.toString(DateTimeFormat1);

    if (!dir.exists()) {
        qWarning() << "Specified directory does not exist!";
        return strDateTime.append(QString("_%1").arg(index));
    }

    QDir target(dirPath.append("/").append(strDateTime).append(QString("_%1").arg(index)));

    if (target.exists())
        return dateTime.toString(DateTimeFormat2).append(QString("_%1").arg(index));

    return target.path();
}

QString SessionNameGenerator::generateCurrentDateTime()
{
    return QDateTime::currentDateTime().toString(DateTimeFormat1);
}

QString SessionNameGenerator::generateRawFileName(int fileNo)
{
    return QString("%1.raw").arg(fileNo,3,10,QChar('0'));
}

QString SessionNameGenerator::generateTransformPrefix(int imageNo)
{
    return QString("Seq_Frame%1_").arg(imageNo,4,10,QChar('0'));
}
