#ifndef BREASTSELECTOR_H
#define BREASTSELECTOR_H

#include <QWidget>
#include <QStackedWidget>
#include <QtGui>

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
    QStackedWidget *imageLR;
    QSvgWidget *image, *imageL, *imageR;
};

#endif // BREASTSELECTOR_H
