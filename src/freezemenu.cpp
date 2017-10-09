#include <QtWidgets>
#include "freezemenu.h"

FreezeMenu::FreezeMenu(QWidget *parent) : QWidget(parent)
{
    menu = new QGroupBox(tr("&Freeze"));

    buttons = new QVBoxLayout;
    QPushButton *saveButton = new QPushButton(tr("&Ulož snímek"));
    QPushButton *renderButton = new QPushButton(tr("&Záznam pro 3D"));
    QPushButton *regionButton = new QPushButton(tr("&Vyznač oblast"));

    connect(renderButton, SIGNAL(clicked(bool)), this, SLOT(handleRecord()));

    QFont font = saveButton->font();
    font.setPointSize(25);
    saveButton->setFont(font);
    renderButton->setFont(font);
    regionButton->setFont(font);
    buttons->addWidget(saveButton);
    buttons->addWidget(renderButton);
    buttons->addWidget(regionButton);

    setLayout(buttons);

    stopLayout = new QVBoxLayout;
    QPushButton *stopRecord = new QPushButton(tr("&Ukonči záznam"));
    stopRecord->setFont(font);
    stopLayout->addWidget(stopRecord);

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
    this->layout()->deleteLater();
    if (!record) {
        setLayout(buttons);
    } else {
        setLayout(stopLayout);
    }
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

void FreezeMenu::handleRecord()
{
    emit startRecord();
    record = true;
    hide();
}

