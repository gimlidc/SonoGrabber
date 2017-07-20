#include "mainwindowresizable.h"
#include "sonoimage.h"
#include "sessionnamegenerator.h"
#include "igtlinkclient.h"
#include "side.h"
#include "pathstroke.h"
#include <QDebug>
#include <QTimer>
#include <QPixmap>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QStackedWidget>
#include <QtSvg/QGraphicsSvgItem>
#include <QtSvg/QSvgWidget>
#include <QVector>


SessionDataLine::SessionDataLine(QDir dirPath, int fileIdx, QWidget* parent) : QWidget(parent)
{
    timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));
    timer->start();

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);
    QLabel *labelSave = new QLabel("Save session");
    labelSave->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mainLayout->addWidget(labelSave);

    dirPathEdit = new QLineEdit(dirPath.absolutePath());
    dirPathEdit->setMinimumWidth(250);
    dirPathEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mainLayout->addWidget(dirPathEdit);

    QString dirTimePart;
    dirTimeEdit = new QLabel(SessionNameGenerator::generateCurrentDateTime());
    dirTimeEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    mainLayout->addWidget(dirTimeEdit);

    mainLayout->addSpacing(10);

    QLabel *labelIdx = new QLabel("Session index");
    mainLayout->addWidget(labelIdx);
    fileIdxEdit = new QLineEdit(QString::number(fileIdx));
    fileIdxEdit->setStyleSheet("border: solid; color: red");
    mainLayout->addWidget(fileIdxEdit);
}

StatusLine::StatusLine(QWidget* parent) : QWidget(parent)
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setMargin(0);
    QLabel *statusLabel = new QLabel("Status");
    mainLayout->addWidget(statusLabel);
    statusText = new QLabel("OK");
    mainLayout->addWidget(statusText);
}

void StatusLine::changeState(QString state)
{
    statusText->setText(state);
}

void SessionDataLine::updateTime()
{
    dirTimeEdit->setText(SessionNameGenerator::generateCurrentDateTime());
}

MainWindowResizable::MainWindowResizable(SessionParams * session, IGTLinkClient * client,
                                         QWidget *parent) : QWidget(parent)
{
    qRegisterMetaType<Image>();
    transform = new Transform(session->getCrop());
    QVector<qreal> radii = {.333, .666, 1};
    QVector<int> segments = {0, 4, 8};
    image = new SonoImage;

    params = session;
    timer = new QTimer(this);
    timer->setInterval(1000);
//    connect(timer, SIGNAL(timeout()), this, SLOT(updateTime()));
    timer->start();
//    ui->lineEdit_3->setStyleSheet("color: red");
//    ui->lineEdit_3->setValidator(new QIntValidator(0,1e10,this));

    bgraph = new BreastGraph(transform, session->getFps(), 0, radii,
                             segments, 0);
    startSequence = new StartSequence(transform);

    // Layouting
    QVBoxLayout *basicLayout = new QVBoxLayout(this);
    sessionData = new SessionDataLine(session->getOutDir(), params->getFilenameIndex(), this);
    sessionData->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    basicLayout->addWidget(sessionData);
    statusLine = new StatusLine(this);
    basicLayout->addWidget(statusLine);
}


