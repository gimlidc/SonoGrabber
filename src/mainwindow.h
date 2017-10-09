#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QByteArray>
#include <QTimer>
#include <QtSvg/QSvgWidget>
#include <QVector4D>
#include <QReadWriteLock>
//#include "breastselector.h"
#include "sessionparams.h"
#include "sonoimage.h"
#include "startsequence.h"
#include "igtlinkclient.h"
#include "breastgraph.h"
#include "transform.h"
#include "freezemenu.h"
#include "side.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(SessionParams * params, IGTLinkClient * client, QWidget *parent = 0);
    ~MainWindow();

    void setOutputDir(QString dirPath);

//    void setStartImage(const QString &fileName);

public slots:
    void toggleRun(bool t);
    void showImage(QImage newImage);
    void changeState(QString state);
    void updateTime();
    void listeningStopped(int e);
//    void receivePos(QVector4D pos);
    void sequenceTerminator();
    void rcvImgPosition(Image);
    void unfreeze();


signals:
    void startListening();
    void stopListening();
    void position(QMatrix4x4 transform);
    void imgPosition(Image);
    void freezeMenu();
    void hideFreezeMenu();
    void stopRecord();

protected:
    void keyPressEvent(QKeyEvent *);

private:
    Ui::MainWindow *ui;
    QString dirPath;
    SessionParams * params;
    QTimer *timer;
    SonoImage *image;
    int step = 0;
    void newSession();
    void showDiag();
    IGTLinkClient * client;
    bool kbdFreeze=false;
    bool statusSetFrozen=false;
    void kbdSetState(QString state);

    StartSequence *startSequence;
    BreastGraph *bgraph;
    void showBreastGraph();
    QString systemState;
    FreezeMenu *menu;
    int freezeCnt=0;

    QReadWriteLock kbdFreezeLock;

    Transform *transform;
};

#endif // MAINWINDOW_H
