#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QDebug>
#include "breastgraph.h"
#include "side.h"


BreastGraph::BreastGraph(Side side, QWidget *parent) : QWidget(parent)
{
      this->setObjectName("BreastGraph");

//    QPainter painter(this);
//    painter.setWindow(QRect(-1, -1.2, 1, 1));
//    painter.setViewport(QRect(-1, -1.2, 1, 1));
//    painter.drawEllipse(QPoint(0,0), .9, .9);
}

BreastGraph::~BreastGraph()
{

}

void BreastGraph::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect v = painter.viewport();
    int dh = width();
    int dv = height();
    int side = qMin(dh, dv)/1.2;
    painter.setWindow(-1, -1.2, 2, 2.2);
    painter.setViewport(QRect((dh-side)/2, dv-side, side, side));
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::black, 0, Qt::SolidLine));
    qreal deltaR = 1.0/4;
    for (int i=1; i<(4+1); i++) {
        painter.drawEllipse(QPointF(0.0,0.0), (qreal)i*deltaR, (qreal)i*deltaR);
    }
    painter.save();
    for (int cnt=0; cnt<12; cnt++) {
        painter.drawLine(QPointF(-1.0,0), QPointF(-deltaR,0));
        painter.rotate(360.0/12.0);
    }
    painter.restore();
}
