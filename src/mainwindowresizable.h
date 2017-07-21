#ifndef MAINWINDOWRESIZABLE_H
#define MAINWINDOWRESIZABLE_H

#include <QImage>
#include <QTimer>
#include <QPushButton>
#include <QtSvg/QSvgWidget>
#include <QVector4D>
#include <QLineEdit>

#include <QWidget>

#include "sessionparams.h"
#include "sonoimage.h"
#include "startsequence.h"
#include "igtlinkclient.h"
#include "breastgraph.h"
#include "transform.h"
#include "side.h"
#include "centralwindow.h"


class SessionDataLine : public QWidget
{
    Q_OBJECT

public:
    explicit SessionDataLine(QDir dirPath, int fileIdx, QWidget* parent);

public slots:
    void updateTime();

private:
    QLineEdit *dirPathEdit;
    QLineEdit *fileIdxEdit;
    QLabel *dirTimeEdit;

    QTimer *timer;
};

class StatusLine : public QWidget
{
    Q_OBJECT

public:
    explicit StatusLine(QWidget* parent);

public slots:
    void changeState(QString state);

private:
    QLabel *statusText;

};

class MainWindowResizable : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindowResizable(SessionParams * params, IGTLinkClient * client,
                                 QWidget *parent = nullptr);
//    ~MainWindowResizable();

    void setOutputDir(QString dirPath);

signals:
    void startListening();
    void stopListening();
    void position(QMatrix4x4 transform);

public slots:
//    void toggleRun(bool t);
//    void showImage(QImage newImage);
//    void changeState(QString state);
//    void updateTime();
//    void listeningStopped(int e);
//    void sequenceTerminator();

private:
    QString dirPath;
    SessionParams * params;
    QTimer *timer;
    SonoImage *image;
    int step = 0;
    void newSession();
    void showDiag();
    IGTLinkClient * client;

    StartSequence *startSequence;
    BreastGraph *bgraph;
    void showBreastGraph();

    Transform *transform;

    SessionDataLine *sessionData;
    StatusLine *statusLine;
    CentralWindow *centralWindow;
    QPushButton *runButton;
};

#endif // MAINWINDOWRESIZABLE_H
