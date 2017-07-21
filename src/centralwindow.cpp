#include <QHBoxLayout>

#include "centralwindow.h"

CentralWindow::CentralWindow(SessionParams *params, IGTLinkClient * client,
                             QWidget *parent) : QWidget(parent)
{
    this->params = params;
    transform = new Transform(params->getCrop());

    QHBoxLayout *centralLayout = new QHBoxLayout(this);
    startSequence = new StartSequence(transform, this);
    startSequence->setMinimumHeight(300);
    startSequence->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    centralLayout->addWidget(startSequence);
//    centralLayout->setAlignment(startSequence, Qt::AlignVCenter);
}
