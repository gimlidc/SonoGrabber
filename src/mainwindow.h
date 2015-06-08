#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
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
    void showImage(int width, int height, const uchar * buffer);
signals:
    void startListening();
    void stopListening();
private:
    Ui::MainWindow *ui;
    QString dirPath;
    SessionParams * params;
    QImage newImage;
    QVector<QRgb> grayScaleColorTable;
    void newSession();
};

#endif // MAINWINDOW_H
