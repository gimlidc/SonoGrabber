#ifndef SONOIMAGE_H
#define SONOIMAGE_H

#include <QWidget>
#include <QtWidgets/QLabel>

class SonoImage : public QWidget
{
    Q_OBJECT
public:
    explicit SonoImage(QWidget *parent = 0);
    ~SonoImage();

private:
    QLabel *image;

signals:

public slots:
};

#endif // SONOIMAGE_H
