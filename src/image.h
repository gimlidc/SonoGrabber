#ifndef IMAGE_H
#define IMAGE_H
#include <QMatrix4x4>
#include <QLineF>
#include "frozen.h"

class Image
{
public:
    Image();
    Image(QMatrix4x4, Frozen);
    Frozen getStatus() const;
    QMatrix4x4 getPosition() const;
    void setLine(QPointF p1, QPointF p2);
    QLineF getLine() const;

private:
    QMatrix4x4 position;
    Frozen status;
    QLineF line2D;
};

Q_DECLARE_METATYPE(Image);

#endif // IMAGE_H
