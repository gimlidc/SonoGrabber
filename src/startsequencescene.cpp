#include <QTimer>
#include <QSvgWidget>
#include <QVBoxLayout>
#include <QSizePolicy>
#include "startsequencescene.h"

//int step=0;
//QTimer* timer;
//QSvgWidget *img;

StartSequenceScene::StartSequenceScene(Transform *pos, QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    initImage(parent);
    layout->addWidget(imageL);
}

void StartSequenceScene::initImage(QWidget *parent)
{    
    imageL = new QSvgWidget(":/images/sel_left_wtracker.svg");
//    QVBoxLayout *layout = new QVBoxLayout(parent);
//    imageL->setBaseSize(size.width(), size.height());
 //   view->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    imageL->show();
}

void StartSequenceScene::showEvent(QShowEvent *event)
{
//    view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}
