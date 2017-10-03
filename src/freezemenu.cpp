#include <QtWidgets>
#include "freezemenu.h"

FreezeMenu::FreezeMenu(QWidget *parent) : QWidget(parent)
{
    menu = new QGroupBox(tr("&Freeze"));

    QPushButton *saveButton = new QPushButton(tr("&Ulož snímek"));
    QPushButton *renderButton = new QPushButton(tr("&Zobraz 3D"));
    QPushButton *regionButton = new QPushButton(tr("&Vyznač oblast"));

    QFont font = saveButton->font();
    font.setPointSize(25);
    saveButton->setFont(font);
    renderButton->setFont(font);
    regionButton->setFont(font);

    QVBoxLayout *buttons = new QVBoxLayout;
    buttons->addWidget(saveButton);
    buttons->addWidget(renderButton);
    buttons->addWidget(regionButton);

    setLayout(buttons);
//    resize(480, 320);
    this->setGeometry(QStyle::alignedRect(
                          Qt::LeftToRight,
                    Qt::AlignCenter,
                    this->size(),
                    qApp->desktop()->availableGeometry()));
//    this->setWindowFlags(Qt::WindowStaysOnTopHint);
}

