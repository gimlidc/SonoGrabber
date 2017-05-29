#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <QRect>
#include <QMatrix4x4>
#include <QVector3D>

class Transform
{
public:
    Transform(QRect crop);
    QVector3D getOrig(QMatrix4x4 *transform);
    QVector3D getX(QMatrix4x4 *transform);
    QVector3D getY(QMatrix4x4 *transform);
    QVector3D getLowest(QMatrix4x4 *transform);

private:
    QRect crop;

};

#endif // TRANSFORM_H
