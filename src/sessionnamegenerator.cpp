#include "sessionnamegenerator.h"
#include <QDir>
#include <QDebug>
#include <QDateTime>

QString SessionNameGenerator::generate(QString dirPath)
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
