#include <QHBoxLayout>

#include "centralwindow.h"
#include "sonoimageresizable.h"

CentralWindow::CentralWindow(SessionParams *params, IGTLinkClient * client,
                             QWidget *parent) : QWidget(parent)
{
    this->params = params;
    transform = new Transform(params->getCrop());
    image = new SonoImageResizable;

    QHBoxLayout *centralLayout = new QHBoxLayout(this);
//    startSequence = new StartSequence(transform, this);
    startSequenceScene = new StartSequenceScene(transform, this);    
//    startSequence->setMinimumHeight(300);
//    startSequence->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    centralLayout->addWidget(startSequence);
//    startSequenceScene->setMinimumHeight(600);
    centralLayout->addWidget(startSequenceScene);
    centralLayout->addWidget(image);
//    centralLayout->setAlignment(startSequence, Qt::AlignVCenter);
}
