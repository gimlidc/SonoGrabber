#ifndef STARTSEQUENCE_H
#define STARTSEQUENCE_H

#include <QWidget>
#include <QVector4D>
#include <QSvgWidget>
#include <QStackedWidget>
#include "side.h"
#include "transform.h"
#include "image.h"

namespace Ui {
class StartSequence;
}

class StartSequence : public QWidget
{
    Q_OBJECT

public:
    explicit StartSequence(Transform *pos, QWidget *parent = 0);
    void reset();
    ~StartSequence();

protected:
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *) override;

private:
    Side side;

    Ui::StartSequence *ui;
    QStackedWidget *imageLR;
    QSvgWidget *image, *imageL, *imageR;
    QSvgWidget* getImageUnderBreast(Side side);
    QSvgWidget* getImageArmpit(Side side);
    void setImage1(Side side);
    void setImage2(Side side);

    void initImage(bool reset);

    Side getSide();
    int w;
    Transform *position;

signals:
    void terminateStartSequence();
    void sideSig(Side side);

public slots:
    void getPos(QMatrix4x4 transform);
    void rcvImgPosition(Image probePos);

};

#endif // STARTSEQUENCE_H
