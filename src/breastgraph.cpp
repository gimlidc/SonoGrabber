#include <QtCore/qmath.h>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QVector>
#include <QDebug>
#include <math.h>
#include "breastgraph.h"
#include "side.h"

qreal r = 100;
qreal top = .2;

BreastGraph::BreastGraph(Side side, QWidget *parent) : QWidget(parent)
{
    this->setObjectName("BreastGraph");
    // lobe
    qreal n = 2*r;
    // touch points
    qreal p = (-n-qSqrt(n*n - r*r))/(2*(r*r));
    qreal xMez = round(qSqrt((-(2*p*n+1)-qSqrt(4*(r*r)*(p*p)+4*n*p+1))/(2*(p*p)))*10)/10.0;
    int cnt = 2*xMez*10+1;
    x.reserve(cnt);
    y.reserve(cnt);
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
    int side = qMin(dh, dv)/((1+top));
    painter.setWindow(-r, -(1+top)*r, 2*r, (2+top)*r);
    painter.setViewport(QRect((dh-side)/2, dv-side, side, side));
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::black, 0, Qt::SolidLine));
    // segmented circles
    qreal deltaR = r/4;
    for (int i=1; i<(4+1); i++) {
        painter.drawEllipse(QPointF(0.0,0.0), (qreal)i*deltaR, (qreal)i*deltaR);
    }
    painter.save();
    for (int cnt=0; cnt<12; cnt++) {
        painter.drawLine(QPointF(-r,0), QPointF(-deltaR,0));
        painter.rotate(360.0/12.0);
    }
    painter.restore();

}
