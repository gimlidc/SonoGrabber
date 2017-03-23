#include "startsequence.h"
#include "ui_startsequence.h"
#include <QSvgWidget>
#include <QStackedWidget>
#include <QLayoutItem>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QCursor>
#include <QPoint>

int step=0;
QTimer* timer;
QSvgWidget *img;

StartSequence::StartSequence(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StartSequence)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);

    initImage(false);

//    timer = new QTimer(this);
//    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
//    timer->start(1000);

////    image = new QSvgWidget();
////    image->load((const QString &)"/home/schier/qt-test/Image/place tracker.svg");
//    ui->heading->setText("Přilepte sondu\nOznačte kalibrační bod");
//    ui->heading->adjustSize();
//    ui->heading->updateGeometry();

//    QSvgWidget *imageL = new QSvgWidget("/home/schier/qt-test/Image/sel_left_wtracker.svg");
//    QSvgWidget *imageR = new QSvgWidget("/home/schier/qt-test/Image/sel_right_wtracker.svg");
//    imageLR = new QStackedWidget();
//    imageLR->addWidget(imageL);
//    imageLR->addWidget(imageR);

//    ui->startBox->addWidget(imageLR);
//    imageLR->show();

}

void StartSequence::initImage(bool reset)
{

//    image = new QSvgWidget();
//    image->load((const QString &)"/home/schier/qt-test/Image/place tracker.svg");
    if (step>0 || !reset) {
        ui->heading->setText("Přilepte sondu\nOznačte kalibrační bod");
        ui->heading->adjustSize();
        ui->heading->updateGeometry();

        QSvgWidget *imageL = new QSvgWidget("/home/schier/qt-test/Image/sel_left_wtracker.svg");
        QSvgWidget *imageR = new QSvgWidget("/home/schier/qt-test/Image/sel_right_wtracker.svg");
        imageLR = new QStackedWidget();
        imageLR->addWidget(imageL);
        imageLR->addWidget(imageR);
        if (reset) {
            ui->startBox->replaceWidget(img, imageLR);
            delete img;
        }
        else
            ui->startBox->addWidget(imageLR);
    }


    imageLR->show();
    step = 0;
}

StartSequence::~StartSequence()
{
    delete ui;
}

QSvgWidget* StartSequence::getImageUnderBreast(Side side)
{
    QString path;
    if (side==Side::LEFT)
        path = "/home/schier/qt-test/Image/selected_left.svg";
    else
        path = "/home/schier/qt-test/Image/selected_right.svg";
    QSvgWidget *img = new QSvgWidget(path);
    return img;
}

void StartSequence::reset()
{
    initImage(true);
}

QSvgWidget* StartSequence::getImageArmpit(Side side)
{
    QString path;
    if (side==Side::LEFT)
        path = "/home/schier/qt-test/Image/armpit_left.svg";
    else
        path = "/home/schier/qt-test/Image/armpit_right.svg";
    return new QSvgWidget(path);
}

StartSequence::Side StartSequence::getSide()
{
    return Side::LEFT;
}

void StartSequence::setImage1(Side side)
{
    ui->heading->setText("Označte kalibrační bod\npod prsem");
    img = getImageUnderBreast(side);
    int cnt = ui->startBox->count();
    ui->startBox->replaceWidget(imageLR, img);
    delete imageLR;
}

void StartSequence::setImage2(Side side)
{
    QSvgWidget *img2;

    ui->heading->setText("Označte kalibrační bod\nv podpaží");
    img2 = img;
    img = getImageArmpit(side);
    ui->startBox->replaceWidget(img2, img);
    delete img2;
}

void StartSequence::mousePressEvent(QMouseEvent *event)
{
    if (event) {
        int w = imageLR->width();
        switch (step) {
        case 0:
            if (this->imageLR->mapFromGlobal(QCursor::pos()).x() >= w/2) {
                side = Side::LEFT;
            }
            else
                side = Side::RIGHT;
            setImage1(side);
            break;
        case 1:
            setImage2(side);
            break;
        case 2:
            emit terminateStartSequence();
        default:
            break;
        }
        step++;
    }
}

void StartSequence::getPos(QVector4D pos)
{
    switch (step) {
    case 0:
        //disconnect(timer, SIGNAL(timeout()), this, SLOT(update()));
        if (pos.z()<0)
            side = Side::LEFT;
        else
            side = Side::RIGHT;
        setImage1(side);
        break;
    case 1:
        setImage2(side);
        break;
    case 2: emit terminateStartSequence();
    default:
        break;
    }
    step++;
}

void StartSequence::paintEvent(QPaintEvent *)
{
    if (step==0) {
        QTime time = QTime::currentTime();
        int idx = time.second() % 2;
        imageLR->setCurrentIndex(idx);
    }
}


