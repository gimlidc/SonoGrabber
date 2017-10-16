#include <QtWidgets>
#include "freezemenu.h"

FreezeMenu::FreezeMenu(QWidget *parent) : QWidget(parent)
{
    menu = new QGroupBox(tr("&Freeze"));

    buttons = new QVBoxLayout;
    menuBox = new QDialogButtonBox(Qt::Vertical);
    stopRecordBox = new QDialogButtonBox(Qt::Vertical);
    this->setStyleSheet("QPushButton {padding: 30px; margin: 20px; font-size: 40pt}");

    timer = new QTimer(this);

    QPushButton *saveButton = new QPushButton(tr("&Save image"));
    QPushButton *renderButton = new QPushButton(tr("&Record 3D"));
    QPushButton *regionButton = new QPushButton(tr("&Mark region"));

    connect(renderButton, SIGNAL(clicked(bool)), this, SLOT(startRecordSlot()));

    QFont font = saveButton->font();

    menuBox->addButton(saveButton, QDialogButtonBox::ActionRole);
    menuBox->addButton(renderButton, QDialogButtonBox::ActionRole);
    menuBox->addButton(regionButton, QDialogButtonBox::ActionRole);


    QPushButton *stopRecord = new QPushButton(tr("&Info: data saved"));
    stopRecordBox->addButton(stopRecord, QDialogButtonBox::ActionRole);

    buttons->addWidget(menuBox);
    buttons->addWidget(stopRecordBox);
    stopRecordBox->setVisible(false);
    setLayout(buttons);
    adjustSize();

    connect(stopRecord, SIGNAL(clicked(bool)), this, SLOT(stopRecordSlot()));

//    resize(480, 320);
    this->setGeometry(QStyle::alignedRect(
                          Qt::LeftToRight,
                    Qt::AlignCenter,
                    this->size(),
                    qApp->desktop()->availableGeometry()));
//    this->setWindowFlags(Qt::WindowStaysOnTopHint);
}

void FreezeMenu::showMenu()
{
    if (record) {
        menuBox->setVisible(false);
        stopRecordBox->setVisible(true);
        connect(timer, SIGNAL(timeout()), this, SLOT(hideMenu()));
        timer->start(5000);
        record = false;
        emit stopRecord();
    } else {
        disconnect(timer, SIGNAL(timeout()), this, SLOT(hideMenu()));
        menuBox->setVisible(true);
        stopRecordBox->setVisible(false);
    }
    adjustSize();
    raise();
    show();
}

void FreezeMenu::hideMenu()
{
    hide();
}

void FreezeMenu::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F) {
        hide();
        emit unfreeze();
    }
}

void FreezeMenu::startRecordSlot()
{
    emit startRecord();
    record = true;
    hide();
}

void FreezeMenu::stopRecordSlot()
{
    emit stopRecord();
    record = false;
    hide();
}

