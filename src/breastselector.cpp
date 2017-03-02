#include <QtGui>
#include <QtWidgets>
#include <QSvgWidget>
#include <QStackedWidget>
#include <QTimer>
#include "ui_startsequence.h"
#include "breastselector.h"

#include <iostream>
using namespace std;


BreastSelector::BreastSelector(QWidget *parent) : QWidget(parent)
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);

    image = new QSvgWidget();
    image->load((const QString &)"/home/schier/qt-test/Image/place tracker.svg");
    ui->startBox->addWidget(image);
    image->show();

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

//void BreastSelector::setStartImage(const QString &fileName)
//{
//    image->load(fileName);
//}


QStackedWidget* BreastSelector::getWidget()
{
    return imageLR;
}
