#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setOutputDir(QString dirPath, QString sessionName = "");
public slots:
    void toggleRun(bool t);
signals:
    void startListening();
    void stopListening();
private:
    Ui::MainWindow *ui;
    QString dirPath;
    void newSession();
};

#endif // MAINWINDOW_H
