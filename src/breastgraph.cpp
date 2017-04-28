#include <QtCore/qmath.h>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QPointF>
#include <QVector>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix3x3>
#include <QMatrix4x4>
#include <QPolygonF>
#include <QDebug>
#include <math.h>
#include "breastgraph.h"
#include "side.h"

qreal r = 100;
qreal top = .2;

BreastGraph::BreastGraph(Side side, qreal angle, QWidget *parent) : QWidget(parent)
{
    this->side = side;
    BreastGraph(angle, parent);
}

BreastGraph::BreastGraph(qreal angle, QWidget *parent) : QWidget(parent)
{
    this->setObjectName("BreastGraph");
    // lobe
    this->angle = angle;
    qreal n = 2*r;
    // touch points
    qreal p = (-n-qSqrt(n*n - r*r))/(2*(r*r));
//    qreal xMez = round(qSqrt((-(2*p*n+1)-qSqrt(4*(r*r)*(p*p)+4*n*p+1))/(2*(p*p)))*10)/10.0;
    qreal t1 = 4*(r*r)*(p*p)+4*n*p+1;
    t1 = t1>0 ? t1 : 0;
    qreal xMez = qSqrt((-(2*p*n+1)-qSqrt(t1))/(2*(p*p)));
    int cnt = 2*xMez*10+1;
    double tmp = -xMez;
    for (int i=0; i<cnt; i++) {
        lobe << QPointF(tmp, p*tmp*tmp + n);
        tmp+=0.1;
    }
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
    int s = qMin(dh, dv)/((1+top));
    painter.setWindow(-r, -(1+top)*r, 2*r, (2+top)*r);
    painter.setViewport(QRect((dh-s)/2, dv-s, s, s));
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
    painter.save();
    painter.rotate((side==Side::LEFT) ? -angle : angle);
    painter.drawPolyline(lobe);
    painter.restore();
    QVector<QVector4D>::iterator it;
    for (it = refPoints.begin(); it != refPoints.end(); it++) {
        QVector4D point = *it;
        painter.drawPoint(project(r*point.toVector3D()));
    }
}

void BreastGraph::setPlane(QVector<QVector4D> refPoints)
{
    QVector3D nipple = QVector3D(refPoints[0]);
    QVector3D r2 = QVector3D(refPoints[1]);
    QVector3D r3 = QVector3D(refPoints[2]);

    QVector3D normal = QVector3D::normal(r2, r3);
    origin = nipple-QVector3D::dotProduct(nipple, normal)*normal;

    qreal unit = origin.distanceToPoint(r2);
    vn = -(r2-origin)/(unit*unit);
//    vn.normalize();
    un = QVector3D::normal(vn, normal);
    un = un/unit;
}

QPointF BreastGraph::project(QVector3D x)
{
    QPointF out;
    out.setX(QVector3D::dotProduct(un, x-origin));
    out.setY(QVector3D::dotProduct(vn, x-origin));
    return out;
}

void BreastGraph::setSide(Side side)
{
    if (this->side==Side::ND)
        this->side=side;
}

void BreastGraph::setPosition(QVector4D pos)
{
    if (side==Side::ND)
        side = (pos.z()<0) ? Side::LEFT : Side::RIGHT;
    refPoints << pos;
    qDebug() << "refPoints: " << refPoints.length();
    if (refPoints.length()==3) {
        setPlane(refPoints);
        QPointF r2 = project(refPoints[1].toVector3D());
        QPointF r3 = project(refPoints[2].toVector3D());
    }
}
