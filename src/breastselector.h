#ifndef BREASTSELECTOR_H
#define BREASTSELECTOR_H

#include <QSvgWidget>
#include <QStackedWidget>
#include <QtGui>
#include "ui_startsequence.h"

class BreastSelector : public QWidget
{
    Q_OBJECT
public:
    explicit BreastSelector(QWidget *parent = 0);
    QStackedWidget* getWidget();


protected:
//    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event) override;


private:
    Ui::StartSequence *ui;
    QStackedWidget *imageLR;
    QSvgWidget *image, *imageL, *imageR;
};

#endif // BREASTSELECTOR_H
