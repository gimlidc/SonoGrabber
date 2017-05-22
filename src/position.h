#ifndef POSITION_H
#define POSITION_H

#include <QRect>
#include <QMatrix4x4>
#include <QVector3D>

class Position
{
public:
    Position(QRect crop);
    QVector3D getOrig(QMatrix4x4 *transform);
    QVector3D getX(QMatrix4x4 *transform);
};

#endif // POSITION_H
