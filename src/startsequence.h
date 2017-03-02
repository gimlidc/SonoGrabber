#ifndef STARTSEQUENCE_H
#define STARTSEQUENCE_H

#include <QWidget>
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
    Ui::StartSequence *ui;
    QStackedWidget *imageLR;
    QSvgWidget *image, *imageL, *imageR;
};

#endif // STARTSEQUENCE_H
