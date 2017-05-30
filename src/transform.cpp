#include "transform.h"
#include <QRect>


Transform::Transform(QRect crop)
{
    this->crop = crop;
}

QVector3D Transform::getOrig(QMatrix4x4 *transform)
{
    QVector4D in = QVector4D(crop.x(), crop.y(), 0, 1);
    return QVector3D(*transform * in);
}

QVector3D Transform::getX(QMatrix4x4 *transform)
{
    QVector4D in = QVector4D(crop.x()+crop.width(), crop.y(), 0, 1);
    return QVector3D(*transform * in);
}

QVector3D Transform::getC(QMatrix4x4 *transform)
{
    QVector4D in = QVector4D(crop.x()+crop.width()/2, crop.y(), 0, 1);
    return QVector3D(*transform * in);
}

QVector3D Transform::getY(QMatrix4x4 *transform)
{
    QVector4D in = QVector4D(crop.x(), crop.y()+crop.height(), 0, 1);
    return QVector3D(*transform * in);
}

QVector3D Transform::getLowest(QMatrix4x4 *transform)
{
    QVector3D p0 = getOrig(transform);
    QVector3D px = getX(transform);
    return p0.y() < px.y() ?
                p0 :
                px;
}


