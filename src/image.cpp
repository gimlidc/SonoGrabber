#include "image.h"

Image::Image(QMatrix4x4 pos, Frozen status=UNFROZEN)
{
    position = pos;
    this->status = status;
}

Frozen Image::getStatus()
{
    return status;
}

QMatrix4x4 Image::getPosition()
{
    return position;
}

