#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QByteArray>
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

    void setOutputDir(QString dirPath, QString sessionName = "");

public slots:
    void toggleRun(bool t);
    void showImage(QImage newImage, QString state);
signals:
    void startListening();
    void stopListening();
private:
    Ui::MainWindow *ui;
    QString dirPath;
    SessionParams * params;
    void newSession();
};

#endif // MAINWINDOW_H
