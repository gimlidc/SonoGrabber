#include "mainwindow.h"
#include <QApplication>
#include <QtDebug>

#include "igtlinkclient.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    IGTLinkClient client;

    QObject::connect(&w,&MainWindow::startListening,&client,&IGTLinkClient::startReading);
    QObject::connect(&w,&MainWindow::stopListening,&client,&IGTLinkClient::stopReading);

    w.show();

    client.setup("192.168.56.100");
    client.setOutputFile("pokus","Image_Probe","ProbeToTracker,ReferenceToTracker", 10);
    //client.setup("localhost",18944);
    //connect()


    qDebug() << "Running";


    return a.exec();
}
