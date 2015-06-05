#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sessionnamegenerator.h"

#include <QDebug>

MainWindow::MainWindow(SessionParams * session, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    params = session;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newSession()
{
    ui->lineEdit->setText(this->dirPath + "/" + SessionNameGenerator::generateSessionDirectory(this->dirPath));
}

void MainWindow::setOutputDir(QString dirPath, QString sessionName)
{
    this->dirPath = dirPath;
    QString subdirName = sessionName;
    if (subdirName.isEmpty())
        subdirName = SessionNameGenerator::generateSessionDirectory(dirPath);
    ui->lineEdit->setText(this->dirPath + "/" + subdirName);
}

void MainWindow::toggleRun(bool buttonPressed)
{
    if (buttonPressed) {
        ui->pushButtonRun->setText("stop");        
        params->setOutputDir(ui->lineEdit->text());
        qDebug() << "Setting output dir: " << params->getOutDir().absolutePath();
        emit startListening();
    }
    else {
        emit stopListening();
        ui->pushButtonRun->setText("run ");
        newSession();
    }
}
