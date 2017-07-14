#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <QRect>
#include <QMatrix4x4>
#include <QVector3D>

class Transform
{
public:
    Transform(QRect crop);
    QVector3D getOrig(const QMatrix4x4 &transform);
    QVector3D getX(const QMatrix4x4 &transform);
    QVector3D getY(const QMatrix4x4 &transform);
    QVector3D getC(const QMatrix4x4 &transform);
    QVector3D getLowest(const QMatrix4x4 &transform);
    qreal getHeight(const QMatrix4x4 &transform);

private:
    QRect crop;

};

#endif // TRANSFORM_H
