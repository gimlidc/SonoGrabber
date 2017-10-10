#include <QtWidgets>
#include "freezemenu.h"

FreezeMenu::FreezeMenu(QWidget *parent) : QWidget(parent)
{
    menu = new QGroupBox(tr("&Freeze"));

    buttons = new QVBoxLayout;
    menuBox = new QDialogButtonBox(Qt::Vertical);
    stopRecordBox = new QDialogButtonBox(Qt::Vertical);
    this->setStyleSheet("QPushButton {padding: 30px; margin: 20px; font-size: 40pt}");

    QPushButton *saveButton = new QPushButton(tr("&Ulož snímek"));
    QPushButton *renderButton = new QPushButton(tr("&Záznam pro 3D"));
    QPushButton *regionButton = new QPushButton(tr("&Vyznač oblast"));

    connect(renderButton, SIGNAL(clicked(bool)), this, SLOT(startRecordSlot()));

    QFont font = saveButton->font();
//    font.setPointSize(25);
//    saveButton->setFont(font);
//    renderButton->setFont(font);
//    regionButton->setFont(font);
//    buttons->addWidget(saveButton);
//    buttons->addWidget(renderButton);
//    buttons->addWidget(regionButton);

    menuBox->addButton(saveButton, QDialogButtonBox::ActionRole);
    menuBox->addButton(renderButton, QDialogButtonBox::ActionRole);
    menuBox->addButton(regionButton, QDialogButtonBox::ActionRole);


//    stopLayout = new QVBoxLayout;
    QPushButton *stopRecord = new QPushButton(tr("&Ukonči záznam"));
    stopRecordBox->addButton(stopRecord, QDialogButtonBox::ActionRole);
//    stopRecord->setFont(font);
//    stopLayout->addWidget(stopRecord);

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
    } else {
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

