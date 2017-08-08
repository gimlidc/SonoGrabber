#include <QHBoxLayout>

#include "centralwindow.h"

CentralWindow::CentralWindow(SessionParams *params, IGTLinkClient * client,
                             QWidget *parent) : QWidget(parent)
{
    this->params = params;
    transform = new Transform(params->getCrop());

    QHBoxLayout *centralLayout = new QHBoxLayout(this);
//    startSequence = new StartSequence(transform, this);
    startSequenceScene = new StartSequenceScene(transform, this);
//    startSequence->setMinimumHeight(300);
//    startSequence->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    centralLayout->addWidget(startSequence);
//    startSequenceScene->setMinimumHeight(600);
    centralLayout->addWidget(startSequenceScene);
//    centralLayout->setAlignment(startSequence, Qt::AlignVCenter);
}
