#include <QtCore/qmath.h>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QPointF>
#include <QVector>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>
#include <QPolygonF>
#include <QDebug>
#include <QtMath>
#include <math.h>
#include "breastgraph.h"
#include "side.h"
#include "transform.h"

qreal r = 100;
qreal rp; //armpit
qreal top = .3;
bool planeSet = false;

QVector<QPointF> refProjection, probePos;


BreastGraph::BreastGraph(Transform *transform, Side side, QWidget *parent) : QWidget(parent)
{
    this->side = side;
    BreastGraph(transform, parent);

}

BreastGraph::BreastGraph(Transform *transform, QWidget *parent) : QWidget(parent)
{
    this->setObjectName("BreastGraph");
    // lobe
//    this->angle = angle;
    points = new QVector<QPointF>;
    refPoints.reserve(3);
    refProjection.reserve(3);
    this->transform = transform;
}

QPolygonF BreastGraph::getLobe(QPointF rp)
{
    qreal n = qSqrt(qPow(rp.x(), 2) + qPow(rp.y(), 2));
    angle = qRadiansToDegrees(qAtan2(rp.x(), rp.y()));

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

BreastGraph::~BreastGraph()
{

}

void BreastGraph::paintEvent(QPaintEvent *event)
{
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
    for (int i = 0; i<refProjection.size(); i++) {
        QPointF point = refProjection.at(i);
        painter.drawEllipse(QPointF(point.x(), point.y()), 2, 2);
    }
    painter.setPen(QPen(Qt::blue, 0));
    for (int i=0; i<probePos.size(); i+=2) {
        QPointF p0 = probePos.at(i);
        QPointF pX = probePos.at(i+1);
        painter.drawLine(p0, pX);
        painter.drawEllipse(QPointF(pX.x(), pX.y()), 2, 2);
    }
    update();
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
    out.setX(r*QVector3D::dotProduct(un, x-origin));
    out.setY(-r*QVector3D::dotProduct(vn, x-origin));
    return out;
}

bool BreastGraph::checkDistance(QVector3D point)
{
    return fabs(point.x())<400 && fabs(point.y())<120 && fabs(point.z())<400;
}

bool BreastGraph::checkOrientation(QVector3D p0, QVector3D py)
{
    QVector3D q = py-p0;
    qreal A = qAcos(-q.y()/qSqrt(q.lengthSquared()));
    return A<(M_PI_2);
}

void BreastGraph::setPosition(QMatrix4x4 trfMatrix)
{
    if (refPoints.length()<3) {
        QVector3D pos = transform->getLowest(&trfMatrix);
        if (side==Side::ND)
            side = (pos.z()<0) ? Side::LEFT : Side::RIGHT;
        refPoints.append(pos);
        qDebug() << "setPos: refPoints: " << refPoints.size() << " " << planeSet;
        if (!planeSet && refPoints.length()==3) {
            setPlane(refPoints);
            *points<<project(QVector3D(pos));
        }
    } else {
        QVector3D pos0 = transform->getOrig(&trfMatrix);
        QVector3D posX = transform->getX(&trfMatrix);
        QVector3D posY = transform->getY(&trfMatrix);
        qDebug() << "x: " << pos0.x() << ", y: " << pos0.y() << ", z: " << pos0.z();
//        if (checkDistance(pos0) && checkOrientation(pos0, posY)) {
        if (checkOrientation(pos0, posY)) {
            probePos.append(project(pos0));
            probePos.append(project(posX));
        } else {
            qDebug() << "Orientation";
        }
    }

}
