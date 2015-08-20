#include "mainwindow.h"
#include <QApplication>
#include <QtDebug>
#include <QSettings>

#include "igtlinkclient.h"
#include <QLineEdit>
#include <QCommandLineParser>
#include <QDir>
#include <QRect>

int main(int argc, char *argv[])
{   
    // Create application
    QApplication app(argc, argv);
    QApplication::setApplicationName("Sono Grabber");
    QApplication::setApplicationVersion("0.1");

    // Create command-line parser
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.setApplicationDescription("application purpose is to connect to Plus server and process incoming data. Data are of two types: images and trasformations. All incoming data are stored and through GUI can be split into serveral session files.");
    // PATH to config file definition
    QCommandLineOption configFilePath(QStringList() << "c" << "path/to/config/file.ini", QCoreApplication::translate("main", "Configuration file is in ini format."), QCoreApplication::translate("main", "file absolute path"));
    parser.addOption(configFilePath);
    // Load parameters from command-line
    parser.process(QApplication::arguments());

    // Load configuration file (if not specified, default values are used)
    QFile configFile(parser.value(configFilePath));
    if (configFile.exists())
        qDebug() << "loading configuration from file" << configFile.fileName();
    else
        qDebug() << "configuration file not found, using default values";
    QSettings settings(parser.value(configFilePath), QSettings::IniFormat);

    // Load connection parameters
    SessionParams session(settings.value("plusServer/host","localhost").toString(),
                 settings.value("plusServer/port", "18944").toInt(), 0);

    session.setFreezePosition(settings.value("image/freezeArea", QRect(0,0,0,0)).value<QRect>());

    // session directory
    QString dirName = settings.value("output/dir", QDir::tempPath()).toString();

    // configure grabber output
    session.setOutput(dirName,
                      settings.value("image/name", "Image_Probe").toStringList(),
                      settings.value("image/crop", QRect(0, 0, 0, 0)).value<QRect>(),
                      settings.value("transformations/names", "ProbeToTracker,ReferenceToTracker").toStringList(),
                      settings.value("output/imageCount", "1000").toInt(),
                      settings.value("output/fps", 100).toInt(),
                      settings.value("output/filenameIndex", "1").toInt() );

    // Create client for server connection (in separate thread)
    IGTLinkClient client(&session, settings.value("gui/refreshRateMs", 200).value<qint64>());

    // Create gui
    MainWindow w(&session);
    w.setOutputDir(dirName);
    w.setWindowTitle(app.applicationName().append(" ").append(app.applicationVersion()));

    // connect client actions with UI
    QObject::connect(&w,&MainWindow::startListening,&client,&IGTLinkClient::startReading);
    QObject::connect(&w,&MainWindow::stopListening,&client,&IGTLinkClient::stopReading);
    QObject::connect(&client, &IGTLinkClient::imageReceived, &w, &MainWindow::showImage);
    QObject::connect(&client, &IGTLinkClient::stateChanged, &w, &MainWindow::changeState);
    QObject::connect(&client, &IGTLinkClient::stopped, &w, &MainWindow::listeningStopped);

    w.show();
    return app.exec();
}
