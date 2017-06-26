#include "image.h"

Image::Image()
{
    position = QMatrix4x4();
    status = UNFROZEN;
}

Image::Image(QMatrix4x4 pos, Frozen status=UNFROZEN)
{
    position = pos;
    this->status = status;
}

Frozen Image::getStatus() const
{
    return status;
}

QMatrix4x4 Image::getPosition() const
{
    return position;
}

void Image::setLine(QPointF p1, QPointF p2)
{
    line2D = QLineF(p1, p2);
}

QLineF Image::getLine() const
{
    return line2D;
}
