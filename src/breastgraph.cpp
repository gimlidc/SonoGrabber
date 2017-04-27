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
}

void BreastGraph::setPlane(QVector<QVector4D> refPoints)
{
    QVector3D nipple = QVector3D(refPoints[0]);
    QVector3D r2 = QVector3D(refPoints[1]);
    QVector3D r3 = QVector3D(refPoints[2]);
    QVector3D normal = QVector3D::normal(r2, r3);
    qreal dist = QVector3D::dotProduct(nipple, normal);
    QVector3D projected = nipple-dist*normal;

    qreal unit = projected.distanceToPoint(r2);
    QVector3D unitX = r2-nipple;
    unitX.normalize();

    QVector3D unitZ = QVector3D::normal(normal, unitX);

    QMatrix4x4 transform = QMatrix4x4();
    transform.setToIdentity();

    QMatrix3x3* rotation = new QMatrix3x3();
    rotation->setToIdentity();
    float_t rotData[16]={};
    for (int i=0; i<3; i++) {
        for (int j=0; j<3;j++) {
            float_t x = 2*normal[i]*normal[j];
            rotData[i*4+j] = x;
        }
        rotData[i*4+3] = 2*normal[i]*dist;
    }
//    for (int j=0; j<3; j++)
//        rotData[3*4+j]=0;
//    rotData[15]=1;
    QMatrix4x4 t1 = QMatrix4x4(rotData);
    transform = transform-QMatrix4x4(rotData);


//    transform->setRow(0, QVector4D(unitX, projected.x()));
//    transform->setRow(1, QVector4D(normal, projected.y()));
//    transform->setRow(2, QVector4D(unitZ, projected.z()));
//    transform->setRow(3, QVector4D(QVector3D(), 1));

    transform.optimize();
    QVector4D orig = transform*QVector4D(projected, 1);

    QMatrix4x4* view = new QMatrix4x4();
    view->setToIdentity();
    view->lookAt(nipple, projected, nipple-r2);
    QVector3D armpit = view->mapVector(QVector3D(r3));
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
    }
}
