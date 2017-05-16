#include "sonoimage.h"

SonoImage::SonoImage(QWidget *parent) : QWidget(parent)
{
    image = new QLabel(parent);
    image->setObjectName(QStringLiteral("image"));
    image->setGeometry(QRect(514, 134, 241, 291));
    image->setScaledContents(true);
}

SonoImage::~SonoImage()
{
    delete this;
}
