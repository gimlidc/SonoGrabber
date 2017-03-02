#include "startsequence.h"
#include "ui_startsequence.h"
#include <QSvgWidget>
#include <QStackedWidget>
#include <QTimer>
#include <QTime>
#include <QDebug>

int step=0;

StartSequence::StartSequence(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StartSequence)
{
    ui->setupUi(this);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);

    image = new QSvgWidget();
    image->load((const QString &)"/home/schier/qt-test/Image/place tracker.svg");
    ui->heading->setText("Přilepte sondu");
    ui->heading->adjustSize();
    ui->heading->updateGeometry();

    QSvgWidget *imageL = new QSvgWidget("/home/schier/qt-test/Image/sel_left.svg");
    QSvgWidget *imageR = new QSvgWidget("/home/schier/qt-test/Image/sel_right.svg");
    imageLR = new QStackedWidget();
    imageLR->addWidget(imageL);
    imageLR->addWidget(imageR);

    ui->startBox->addWidget(image);
    image->show();

}

StartSequence::~StartSequence()
{
    delete ui;
}

void StartSequence::mousePressEvent(QMouseEvent *event)
{
    if (event) {
        if (step==0) {
            ui->startBox->replaceWidget(image, imageLR);
            ui->heading->setText("Vyberte prs");
            delete image;
            imageLR->show();
//        } else if (step==1) {
//            ui->startBox->replaceWidget(image, breastSelector->getWidget());
////            delete image;
        }
        step++;
    }
}

void StartSequence::paintEvent(QPaintEvent *)
{
    QTime time = QTime::currentTime();
    int idx = time.second() % 2;
    imageLR->setCurrentIndex(idx);
}


