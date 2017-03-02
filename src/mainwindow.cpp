#include "mainwindow.h"
//#include "breastselector.h"
#include "ui_mainwindow.h"
#include "sessionnamegenerator.h"
#include "igtlinkclient.h"
#include "startsequence.h"
#include <QDebug>
#include <QPixmap>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QStackedWidget>
#include <QtSvg/QGraphicsSvgItem>
#include <QtSvg/QSvgWidget>

MainWindow::MainWindow(SessionParams * session, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    params = session;
    timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));
    timer->start();
    ui->lineEdit_3->setStyleSheet("color: red");
    ui->lineEdit_3->setValidator(new QIntValidator(0,1e10,this));
    StartSequence *startSequence = new StartSequence();
    ui->mainWindow->addWidget(startSequence);
    startSequence->show();

//    image = new QSvgWidget();
//    image->load((const QString &)"/home/schier/qt-test/Image/place tracker.svg");
//    ui->startBox->addWidget(image);
//    image->show();
//    breastSelector = new BreastSelector();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newSession()
{
    QString dirTimePart;
    dirPath = ui->lineEdit->text();
    params->setFilenameIndex(ui->lineEdit_3->text().toInt());
    params->setOutputDir(SessionNameGenerator::generateSessionDirectory(dirPath,params->getFilenameIndex(),dirTimePart));
    ui->lineEdit->setText(dirPath);
    ui->lineEdit_2->setText(dirTimePart);
    ui->lineEdit_3->setText(QString::number(params->getFilenameIndex()));

}

void MainWindow::setOutputDir(QString dirPath)
{
    this->dirPath = dirPath;
    ui->lineEdit->setText(this->dirPath);
    ui->lineEdit_3->setText(QString::number(params->getFilenameIndex()));
}


void MainWindow::toggleRun(bool buttonPressed)
{
    if (buttonPressed) {
        ui->pushButtonRun->setText("Stop");
        timer->stop();
        ui->lineEdit->setEnabled(false);
        ui->lineEdit_2->setEnabled(false);
        ui->lineEdit_3->setEnabled(false);
        newSession();
        qDebug() << "Setting output dir: " << params->getOutDir().absolutePath();
        emit startListening();
    }
    else {
        emit stopListening();
        timer->start();
        ui->pushButtonRun->setText("Start");
        ui->lineEdit->setEnabled(true);
        ui->lineEdit_2->setEnabled(true);
        ui->lineEdit_3->setEnabled(true);
        ui->lineEdit_3->setText(QString::number(params->getFilenameIndex()));
        ui->sonoImage->setPixmap(QPixmap());
    }
}


void MainWindow::listeningStopped(int e)
{
    switch ((IGTLinkClient::ErrorType)e) {
    case IGTLinkClient::UserInterrupt:
        ui->state->setText("OK");
        params->incFilenameIndex();
        break;
    case IGTLinkClient::ReceiveError:
        ui->state->setText("REC ERROR");
        params->incFilenameIndex();
        break;
    case IGTLinkClient::SocketOpenError:
        ui->state->setText("SOCKET ERROR");
    }
    ui->lineEdit_3->setText(QString::number(params->getFilenameIndex()));
    if (ui->pushButtonRun->isChecked())
        ui->pushButtonRun->setChecked(false);
}

void MainWindow::showImage(QImage newImage)
{
    qDebug() << "drawing";
    ui->sonoImage->setPixmap(QPixmap::fromImage(newImage));
    ui->sonoImage->adjustSize();
    ui->sonoImage->repaint();
}

//void MainWindow::startImage()
//{
//    QSvgWidget *image = new QSvgWidget("/home/schier/qt-test/Image/place tracker.svg");
//    ui->startBox->addWidget(image);
//    image->show();

//}


//void MainWindow::selectBreast()
//{
//    QSvgWidget *imageL = new QSvgWidget("/home/schier/qt-test/Image/sel_right.svg");
//    QSvgWidget *imageR = new QSvgWidget("/home/schier/qt-test/Image/sel_right.svg");
//    QStackedWidget *imageLR = new QStackedWidget;
//    imageLR->addWidget(imageL);
//    imageLR->addWidget(imageR);
//    ui->labelStartTitle->setText("Vyberte prs");
//    ui->startBox->replaceWidget(image, imageLR);
//    delete image;
////    ui->startBox->addWidget(imageLR);
//}

void MainWindow::changeState(QString state)
{
    ui->state->setText(state);
}

void MainWindow::updateTime()
{
    ui->lineEdit_2->setText(SessionNameGenerator::generateCurrentDateTime());
}
