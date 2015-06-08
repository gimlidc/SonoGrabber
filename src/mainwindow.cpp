#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sessionnamegenerator.h"

#include <QDebug>
#include <QVector>
#include <QRgb>

MainWindow::MainWindow(SessionParams * session, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    params = session;

    for (int i = 0; i < 256; i++) {
        grayScaleColorTable.push_back(QColor(i, i, i).rgb());
    }
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

void MainWindow::showImage(int width, int height, const uchar* buffer)
{
    if (newImage.width() != width || newImage.height() != height) {
        newImage = QImage(buffer, width, height, width, QImage::Format_Indexed8);
        newImage.setColorTable(MainWindow::grayScaleColorTable);
    } else {
        for(int y = 0; y < height; y++)
            for(int x = 0; x < width; x++) {
                newImage.setPixel(x, y, buffer[x + y * width]);
            }
    }
    ui->sonoImage->setPixmap(QPixmap::fromImage(newImage));
    ui->sonoImage->adjustSize();
    ui->sonoImage->repaint();
}
