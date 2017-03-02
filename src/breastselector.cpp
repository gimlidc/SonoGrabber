#include <QtGui>
#include <QtWidgets>
#include <QSvgWidget>
#include <QStackedWidget>
#include <QTimer>
#include "breastselector.h"

#include <iostream>
using namespace std;


BreastSelector::BreastSelector(QWidget *parent) : QWidget(parent)
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);

    QSvgWidget *imageL = new QSvgWidget("/home/schier/qt-test/Image/sel_right.svg");
    QSvgWidget *imageR = new QSvgWidget("/home/schier/qt-test/Image/sel_right.svg");
    imageLR = new QStackedWidget;
    imageLR->addWidget(imageL);
    imageLR->addWidget(imageR);

//    comboBox->addItem("breat 1");
//    comboBox->addItem("breast 2");
}

void BreastSelector::paintEvent(QPaintEvent *)
{
    QTime time = QTime::currentTime();
    int idx = 5;//time.second() % 2;
    cout<<idx;
}

QStackedWidget* BreastSelector::getWidget()
{
    return imageLR;
}
