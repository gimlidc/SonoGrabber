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
#include <QtMath>
#include <math.h>
#include "breastgraph.h"
#include "side.h"

qreal r = 100;
qreal rp; //armpit
qreal top = .3;
bool planeSet = false;

QVector<QPointF> refProjection;

BreastGraph::BreastGraph(Side side, qreal angle, QWidget *parent) : QWidget(parent)
{
    this->side = side;
    BreastGraph(angle, parent);

}

QPolygonF BreastGraph::getLobe(QPointF rp)
{
    qreal n = r*qSqrt(qPow(rp.x(), 2) + qPow(rp.y(), 2));
    angle = qRadiansToDegrees(qAtan2(rp.x(), rp.y()));
//    angle = (rp.x()>0 ? 1 : -1) * (90+angle);
    angle = 180 - angle;

    QPolygonF lobe;
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
    return lobe;
}

BreastGraph::BreastGraph(qreal angle, QWidget *parent) : QWidget(parent)
{
    this->setObjectName("BreastGraph");
    // lobe
    this->angle = angle;
    points = new QVector<QPointF>;
    refPoints.reserve(3);
    refProjection.reserve(3);
}

BreastGraph::~BreastGraph()
{

}

void BreastGraph::paintEvent(QPaintEvent *event)
{
    qDebug() << "paint: len: " << refProjection.size();
    QPainter painter(this);
    QRect v = painter.viewport();
    int dh = width();
    int dv = height();
    int s = qMin(dh, dv)/((1+1.2*top));
    painter.setWindow(-(1+top)*r, -(1+top)*r, 2*(1+top)*r, (2+top)*r);
    painter.setViewport(QRect((dh-s)/2, dv-s, s, s));
//    painter.setViewport(QRect(0, dv-s, dh, s));
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
    painter.setPen(QPen(Qt::red, 0));
//    QVector<QVector4D>::iterator it;
    for (int i = 0; i<refProjection.size(); i++) {
        QPointF point = refProjection.at(i);
        qDebug() << "x: " << point.x() << " y: " << point.y();
        painter.drawEllipse(r*QPointF(point.x(), -point.y()), 2, 2);
//        painter.drawPoint(r*point);
    }
}

void BreastGraph::reset()
{
    this->hide();
    side = Side::ND;
    refPoints.clear();
    refProjection.clear();
    points = new QVector<QPointF>;
}

void BreastGraph::setPlane(QVector<QVector3D> refPoints)
{
    planeSet = true;
    qDebug() << "setPlane: #refPoints: " << refPoints.length();
    QVector3D nipple = refPoints[0];
    QVector3D r2 = refPoints[1];
    QVector3D r3 = refPoints[2];

    QVector3D normal = QVector3D::normal(r2, r3);
    origin = nipple-QVector3D::dotProduct(nipple, normal)*normal;

    qreal unit = origin.distanceToPoint(r2);
    vn = -(r2-origin)/(unit*unit);
//    vn.normalize();
    un = QVector3D::normal(vn, normal);
    un = un/unit;
    refProjection.append(project(nipple));
    refProjection.append(project(r2));
    refProjection.append(project(r3));
    QPointF armpit = refProjection.last();
    qDebug() << "setPlane: refProjection: " << refProjection.size();
    lobe = getLobe(armpit);
}

QPointF BreastGraph::project(QVector3D x)
{
    QPointF out;
    out.setX(QVector3D::dotProduct(un, x-origin));
    out.setY(QVector3D::dotProduct(vn, x-origin));
    return out;
}


void BreastGraph::setPosition(QVector4D pos)
{
    if (side==Side::ND)
        side = (pos.z()<0) ? Side::LEFT : Side::RIGHT;
    if (refPoints.length()<3) {
        refPoints.append(QVector3D(pos));
    }
    qDebug() << "setPos: refPoints: " << refPoints.size() << " " << planeSet;
    if (!planeSet && refPoints.length()==3) {
        setPlane(refPoints);
        *points<<project(QVector3D(pos));
        QPointF r2 = project(refPoints[1]);
        QPointF r3 = project(refPoints[2]);
    }
    if (refPoints.length()>3) {
        *points<<project(QVector3D(pos));
//        *points<<project(QVector3D(pos));
//        this->update();
    }
}
