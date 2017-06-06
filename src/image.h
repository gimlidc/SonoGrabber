#ifndef IMAGE_H
#define IMAGE_H
#include <QMatrix4x4>
#include <QLineF>
#include "frozen.h"

class Image
{
public:
    Image(QMatrix4x4, Frozen);
    Frozen getStatus();
    QMatrix4x4 getPosition();

private:
    QMatrix4x4 position;
    Frozen status;
    QLineF position2D;
};

#endif // IMAGE_H
