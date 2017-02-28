#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QByteArray>
#include <QTimer>
#include <QtSvg/QSvgWidget>
#include "sessionparams.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(SessionParams * params, QWidget *parent = 0);
    ~MainWindow();

    void setOutputDir(QString dirPath);

    void setStartImage(const QString &fileName);

public slots:
    void toggleRun(bool t);
    void showImage(QImage newImage);
    void changeState(QString state);
    void updateTime();
    void listeningStopped(int e);
    void startImage();

//protected:
//    void mousePressEvent(QMouseEvent *event) override;

signals:
    void startListening();
    void stopListening();
private:
    Ui::MainWindow *ui;
    QString dirPath;
    SessionParams * params;
    QTimer *timer;
    QSvgWidget *image;
    void newSession();
    void showDiag();
};

#endif // MAINWINDOW_H
