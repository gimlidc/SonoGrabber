#ifndef STARTSEQUENCE_H
#define STARTSEQUENCE_H

#include <QWidget>
#include <QVector4D>
#include <QSvgWidget>
#include <QStackedWidget>

namespace Ui {
class StartSequence;
}

class StartSequence : public QWidget
{
    Q_OBJECT

public:
    explicit StartSequence(QWidget *parent = 0);
    ~StartSequence();

protected:
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *) override;

private:
    enum class Side{
        LEFT,
        RIGHT
    } side;

    Ui::StartSequence *ui;
    QStackedWidget *imageLR;
    QSvgWidget *image, *imageL, *imageR;
    QSvgWidget* getImageUnderBreast(Side side);
    QSvgWidget* getImageArmpit(Side side);
    void setImage1(Side side);
    void setImage2(Side side);

    Side getSide();

public slots:
    void getPos(QVector4D pos);

};

#endif // STARTSEQUENCE_H
