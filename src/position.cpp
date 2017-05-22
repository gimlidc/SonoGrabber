#include "position.h"
#include <QRect>

QRect *crop;

Position::Position(QRect *crop)
{
    this->crop = crop;
}

QVector3D Position::getOrig(QMatrix4x4 *transform)
{
    QVector4D in = QVector4D(crop->x(), crop->y(), 0, 1);
    return QVector3D(*transform * in);
}

QVector3D Position::getX(QMatrix4x4 *transform)
{

}


