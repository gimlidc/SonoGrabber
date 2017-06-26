#include "mainwindow.h"
//#include "breastselector.h"
#include "ui_mainwindow.h"
#include "sonoimage.h"
#include "sessionnamegenerator.h"
#include "igtlinkclient.h"
#include "side.h"
#include <QDebug>
#include <QPixmap>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QStackedWidget>
#include <QtSvg/QGraphicsSvgItem>
#include <QtSvg/QSvgWidget>


MainWindow::MainWindow(SessionParams * session, IGTLinkClient * client, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qRegisterMetaType<Image>();

    transform = new Transform(session->getCrop());
    bgraph = new BreastGraph(transform, session->getFps(), 100, 0);
    image = new SonoImage;

    params = session;
    timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));
    timer->start();
    ui->lineEdit_3->setStyleSheet("color: red");
    ui->lineEdit_3->setValidator(new QIntValidator(0,1e10,this));

    startSequence = new StartSequence(transform);
    startSequence->show();
    ui->mainWindow->addWidget(startSequence);
    connect(this, &MainWindow::position, startSequence, &StartSequence::getPos);
    this->client = client;

    // connect client actions with UI
    QObject::connect(this,&MainWindow::startListening,client,&IGTLinkClient::startReading);
    QObject::connect(this,&MainWindow::stopListening,client,&IGTLinkClient::stopReading);
    QObject::connect(client, &IGTLinkClient::imageReceived, this, &MainWindow::showImage);
    QObject::connect(client, &IGTLinkClient::stateChanged, this, &MainWindow::changeState);
    QObject::connect(client, &IGTLinkClient::stopped, this, &MainWindow::listeningStopped);
    //QObject::connect(client, &IGTLinkClient::position, startSequence, &StartSequence::getPos);
    QObject::connect(client, &IGTLinkClient::imgPosition, startSequence,
                     &StartSequence::rcvImgPosition);
    QObject::connect(startSequence, &StartSequence::terminateStartSequence, this,
                     &MainWindow::sequenceTerminator);

//    QObject::connect(client, &IGTLinkClient::position, bgraph, &BreastGraph::setPosition);
//    QObject::connect(client, &IGTLinkClient::frozen, bgraph, &BreastGraph::receiveFrozen);
    QObject::connect(client, &IGTLinkClient::imgPosition, bgraph, &BreastGraph::rcvImgPosition);
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

void MainWindow::showBreastGraph()
{
    ui->mainWindow->addWidget(bgraph);
//    ui->mainWindow->addWidget(ui->sonoImage);
    ui->mainWindow->addWidget(image);
    bgraph->show();
}


//void MainWindow::receivePos(QVector4D pos)
//{
//    qDebug() << "emit position";
//    emit position(pos);
//}

void MainWindow::setOutputDir(QString dirPath)
{
    this->dirPath = dirPath;
    ui->lineEdit->setText(this->dirPath);
    ui->lineEdit_3->setText(QString::number(params->getFilenameIndex()));
}

void MainWindow::sequenceTerminator()
{
    ui->mainWindow->removeWidget(startSequence);
    delete startSequence;
    showBreastGraph();
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
        if (this->findChild<QWidget *>("BreastGraph")) {
            ui->mainWindow->removeWidget(bgraph);
            bgraph->reset();
            qDebug() << "reset bgraph";
        }
        if (!(this->findChild<QWidget *>("StartSequence")))
        {
            startSequence = new StartSequence(transform);
            ui->mainWindow->addWidget(startSequence);
            startSequence->show();
            QObject::connect(client, &IGTLinkClient::position, startSequence, &StartSequence::getPos);
            QObject::connect(startSequence, &StartSequence::terminateStartSequence, this,
                             &MainWindow::sequenceTerminator);

        }
        else
        {
            startSequence->reset();
        }
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
//    qDebug() << "drawing";
    ui->sonoImage->setPixmap(QPixmap::fromImage(newImage));
    ui->sonoImage->setScaledContents(true);
    ui->sonoImage->adjustSize();
    ui->sonoImage->repaint();
}


void MainWindow::changeState(QString state)
{
    ui->state->setText(state);
}

void MainWindow::updateTime()
{
    ui->lineEdit_2->setText(SessionNameGenerator::generateCurrentDateTime());
}
