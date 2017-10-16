#include "mainwindow.h"
#include "ui_mainwindowResize.h"
#include "mainwindowresizable.h"
#include "sonoimage.h"
#include "sessionnamegenerator.h"
#include "igtlinkclient.h"
#include "side.h"
//#include "pathstroke.h"
#include <QDebug>
#include <QPixmap>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QStackedWidget>
#include <QtSvg/QGraphicsSvgItem>
#include <QtSvg/QSvgWidget>
#include <QVector>


MainWindow::MainWindow(SessionParams * session, IGTLinkClient * client, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qRegisterMetaType<Image>();

//    PathStrokeWidget* pathStrokeWidget = new PathStrokeWidget(this);

//    pathStrokeWidget->show();

    transform = new Transform(session->getCrop());
    QVector<qreal> radii = {.333, .666, 1};
    QVector<int> segments = {0, 4, 8};
    bgraph = new BreastGraph(transform, session->getFps(), 0, radii,
                             segments, 0);
    image = new SonoImage;

    params = session;
    timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));
    timer->start();
    ui->lineEdit_3->setStyleSheet("color: red");
    ui->lineEdit_3->setValidator(new QIntValidator(0,1e10,this));

    startSequence = new StartSequence(transform);
    startSequence->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    ui->mainLayout->insertWidget(0, startSequence);
    ui->mainLayout->setStretch(0, 1);
    startSequence->setMinimumSize(500, 300);
    ui->mainLayout->setAlignment(startSequence, Qt::AlignCenter);
//    ui->mainWindow->addWidget(startSequence);
    startSequence->show();
    connect(this, &MainWindow::position, startSequence, &StartSequence::getPos);
    this->client = client;

    ui->state->setText("WAITING");

    menu = new FreezeMenu(0);

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

//    QObject::connect(client, &IGTLinkClient::imgPosition, bgraph, &BreastGraph::rcvImgPosition);
    // pass image position and freeze status to breastgraph
    QObject::connect(client, &IGTLinkClient::imgPosition, this, &MainWindow::rcvImgPosition);
    QObject::connect(this, &MainWindow::imgPosition, bgraph, &BreastGraph::rcvImgPosition);

    // handle freezemenu
    QObject::connect(this, &MainWindow::showFreezeMenu, menu, &FreezeMenu::showMenu);
    QObject::connect(this, &MainWindow::hideFreezeMenu, menu, &FreezeMenu::hideMenu);
    QObject::connect(menu, &FreezeMenu::unfreeze, this, &MainWindow::unfreeze);
    connect(menu, &FreezeMenu::startRecord, client, &IGTLinkClient::startRecord);
    connect(menu, &FreezeMenu::stopRecord, client, &IGTLinkClient::stopRecord);
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
    ui->mainLayout->insertWidget(0, bgraph);
    ui->mainLayout->setStretch(0, 1);
//    ui->mainWindow->addWidget(ui->sonoImage);
//    ui->mainWindow->addWidget(image);
    bgraph->show();
}


void MainWindow::setOutputDir(QString dirPath)
{
    this->dirPath = dirPath;
    ui->lineEdit->setText(this->dirPath);
    ui->lineEdit_3->setText(QString::number(params->getFilenameIndex()));
}

void MainWindow::sequenceTerminator()
{
//    ui->mainWindow->removeWidget(startSequence);
    delete startSequence;
    ui->state->setText("SCANNING");
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
            ui->mainLayout->removeWidget(bgraph);
            bgraph->reset();
            qDebug() << "reset bgraph";
        }
        if (!(this->findChild<QWidget *>("StartSequence")))
        {
            startSequence = new StartSequence(transform);
            startSequence->setMinimumSize(500, 300);
            ui->mainLayout->insertWidget(0, startSequence);
            ui->mainLayout->setAlignment(startSequence, Qt::AlignCenter);
            ui->mainLayout->setStretch(0, 1);

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
    qDebug() << "listening stopped";
    switch ((IGTLinkClient::ErrorType)e) {
    case IGTLinkClient::UserInterrupt:
    case IGTLinkClient::RecordingStopped:
        // ui->state->setText("OK");
        params->incFilenameIndex();
        break;
    case IGTLinkClient::ReceiveError:
        // ui->state->setText("REC ERROR");
        params->incFilenameIndex();
        break;
    case IGTLinkClient::SocketOpenError:
        ui->state->setText("SOCKET ERROR");
    }
    ui->lineEdit_3->setText(QString::number(params->getFilenameIndex()));
    if (e!=IGTLinkClient::RecordingStopped && ui->pushButtonRun->isChecked())
        ui->pushButtonRun->setChecked(false);
}

void MainWindow::showImage(QImage newImage)
{
    bool kbdLocalStat;
    kbdFreezeLock.lockForRead();
    kbdLocalStat = kbdFreeze;
    kbdFreezeLock.unlock();
    if (!kbdLocalStat) {
        ui->sonoImage->setPixmap(QPixmap::fromImage(newImage));
        //    ui->sonoImage->setScaledContents(true);
        //    ui->sonoImage->adjustSize();
        ui->sonoImage->repaint();
    }
}


void MainWindow::changeState(QString state)
{
    if (state.compare("CROPPED")==0)
        ui->state->setText("SCANNING");
    else
        ui->state->setText(state);
    systemState = state;
}

void MainWindow::kbdSetState(QString state) {
    if (state.compare("FROZEN")==0) // equals
        ui->state->setText(state);
    else if (systemState.compare("FROZEN")!=0) // not equal
        ui->state->setText(state);
}

void MainWindow::updateTime()
{
    ui->lineEdit_2->setText(SessionNameGenerator::generateCurrentDateTime());
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F)
    {
        kbdFreezeLock.lockForWrite();
        kbdFreeze = !kbdFreeze;
        kbdFreezeLock.unlock();
        if (kbdFreeze)
            kbdSetState("FROZEN");
        else {
            kbdSetState(systemState);
        }
    }
}

void MainWindow::rcvImgPosition(Image img)
{
    if (kbdFreeze) {
        if (!statusSetFrozen) {
            img.setStatus(Frozen::FROZEN);
            statusSetFrozen = true;
            emit imgPosition(img);
        }
    } else {
        emit imgPosition(img);
        if (img.getStatus()==FROZEN)
            statusSetFrozen = true;
    }
    if (img.getStatus()==FROZEN) {
        freezeCnt++;
        if (freezeCnt>3)
            emit showFreezeMenu();
    } else {
        if (!kbdFreeze && img.getStatus()!=FROZEN && statusSetFrozen) {
            statusSetFrozen = false;
            emit hideFreezeMenu();
            ui->lineEdit_3->setText(QString::number(params->getFilenameIndex()));
        }
    }
}

void MainWindow::unfreeze()
{
    kbdFreeze = false;
}

