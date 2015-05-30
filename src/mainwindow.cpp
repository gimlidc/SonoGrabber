#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sessionnamegenerator.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newSession()
{
    ui->lineEdit->setText(this->dirPath + "/" + SessionNameGenerator::generate(this->dirPath));
}

void MainWindow::setOutputDir(QString dirPath, QString sessionName)
{
    this->dirPath = dirPath;
    QString subdirName = sessionName;
    if (subdirName.isEmpty())
        subdirName = SessionNameGenerator::generate(dirPath);
    ui->lineEdit->setText(this->dirPath + "/" + subdirName);
}

void MainWindow::toggleRun(bool buttonPressed)
{
    if (buttonPressed) {
        ui->pushButtonRun->setText("stop");
        emit startListening();
    }
    else {
        emit stopListening();
        ui->pushButtonRun->setText("run ");
        newSession();
    }
}
