#ifndef IMAGE_H
#define IMAGE_H
#include <QMatrix4x4>
#include <QLineF>
#include "frozen.h"

class Image
{
public:
    Image();
//    Image(QMatrix4x4 pos, Frozen status);
    Image(QMatrix4x4 pos, double ts, Frozen status);
    Frozen getStatus() const;
    QMatrix4x4 getPosition() const;
    void setLine(QPointF p1, QPointF p2);
    void setLine(QLineF line);
    QLineF getLine() const;
    double getTimeStamp() const;

private:
    QMatrix4x4 position;
    Frozen status;
    QLineF line2D;
    double (ts);
};

Q_DECLARE_METATYPE(Image);

#endif // IMAGE_H
