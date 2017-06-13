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
#include "frozen.h"

qreal r = 100;
qreal rp; //armpit
qreal top = .3;
qreal imgHeight;
qreal fade = 0.95;
bool planeSet = false;

QVector<QPointF> refProjection, probePos;
QVector<Frozen> freezPoints;
QVector<qreal> alpha;
QVector3D lastPos;
int bufPos=0, bufLen=0;


BreastGraph::BreastGraph(Transform *transform, int fps, int buffSize, QWidget *parent) : QWidget(parent)
{
    this->setObjectName("BreastGraph");
    points = new QVector<QPointF>;
    this->fps = fps;
    refPoints.reserve(3);
    refProjection.reserve(3);
    this->transform = transform;
    if (buffSize>0) {
        probePos = QVector<QPointF>(2*buffSize);
        freezPoints = QVector<Frozen>(buffSize);
        alpha = QVector<qreal>(buffSize);
        this->buffSize = 2*buffSize;
    }
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
    painter.setViewport(QRect((dh-s)/2, .9*(dv-s), s, s));
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
    if (buffSize==0) {
        QColor blueCol = QColor(Qt::blue);
        for (int i=0; i<probePos.size(); i+=2) {
            if (freezPoints.at(i/2)==Frozen::UNFROZEN) {
                blueCol.setAlphaF(alpha[i/2]);
                painter.setPen(QPen(blueCol, 2));
            } else {
                painter.setPen(QPen(Qt::red, 2));
            }
            QPointF p0 = probePos.at(i);
            QPointF pX = probePos.at(i+1);
            painter.drawLine(p0, pX);
            painter.drawEllipse(QPointF(pX.x(), pX.y()), 2, 2);
            if (i>=3) {
                QPointF points[4] = {probePos.at(i-3), probePos.at(i-1), probePos.at(i), probePos.at(i-2)};
                painter.setPen(QPen(Qt::green, 0));
                painter.setBrush(QBrush(Qt::green, Qt::SolidPattern));
                painter.setOpacity(0.5);
                painter.drawPolygon(points, 4);
            }
        }
    } else {
        QColor blueCol = QColor(Qt::blue);
        for (int i=0; i<bufLen; i+=2) {
            if (freezPoints.at(i/2)==Frozen::UNFROZEN) {
                blueCol.setAlphaF(alpha[i/2]);
                painter.setPen(QPen(blueCol, 2));
            } else {
                painter.setPen(QPen(Qt::red, 2));
            }
            QPointF p0 = probePos.at(i);
            QPointF pX = probePos.at(i+1);
            painter.drawLine(p0, pX);
            painter.drawEllipse(QPointF(pX.x(), pX.y()), 2, 2);
            if (i>=3) {
                QPointF points[4] = {probePos.at(i-3), probePos.at(i-1), probePos.at(i), probePos.at(i-2)};
//                QPointF points[4];
//                for (int j=0; j<4; j++)
//                    points[j] = probePos.at(i-3+j);
                painter.setPen(QPen(Qt::green, 0));
                painter.setBrush(QBrush(Qt::green, Qt::SolidPattern));
                painter.setOpacity(0.5);
                painter.drawPolygon(points, 4);
            }
        }
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
    qreal A = qAcos(-q.y()/imgHeight);
    return A<(M_PI_2);
}

bool BreastGraph::checkSpeeed(QVector3D point)
{
    qreal speed = qSqrt(point.distanceToPoint(lastPos))*fps;
    lastPos = point;
    return speed<=220;
}

void BreastGraph::setPosition(QMatrix4x4 trfMatrix)
{
    if (refPoints.length()<3) {
        if (refPoints.length()==0) {
            QVector3D pos0 = transform->getOrig(&trfMatrix);
            QVector3D posY = transform->getY(&trfMatrix);
            QVector3D q = posY-pos0;
            imgHeight = qSqrt(q.lengthSquared());
        }
        QVector3D pos = transform->getLowest(&trfMatrix);
        if (side==Side::ND)
            side = (pos.z()<0) ? Side::LEFT : Side::RIGHT;
        refPoints.append(pos);
        if (!planeSet && refPoints.length()==3) {
            setPlane(refPoints);
            *points<<project(QVector3D(pos));
            lastPos = transform->getC(&trfMatrix);
        }
    } else {
        if (bufLen==0) {
            freeze = Frozen::UNFROZEN;
            bufLen++;
        }
        QVector3D pos0 = transform->getOrig(&trfMatrix);
        QVector3D posX = transform->getX(&trfMatrix);
        QVector3D posY = transform->getY(&trfMatrix);
//        qDebug() << "x: " << pos0.x() << ", y: " << pos0.y() << ", z: " << pos0.z();
        bool dist = checkDistance(pos0);
        bool speed = checkSpeeed(transform->getC(&trfMatrix));
        bool orient = checkOrientation(pos0, posY);
        if (dist && speed && orient) {
            if (buffSize==0) {
                probePos.append(project(pos0));
                probePos.append(project(posX));
                for (int i=0; i<alpha.size(); i++)
                    alpha[i] *= fade;
                alpha.append(1);
                freezPoints.append(freeze);
                if (freezPoints.last()==FROZEN)
                    qDebug() << "last freezpoint frozen";
                if (freeze==FROZEN)
                    qDebug() << "bg: frozen";
                freeze = UNFROZEN;
            } else {
                probePos.replace(bufPos, project(pos0));
                probePos.replace(bufPos+1, project(posX));
                freezPoints.replace(bufPos/2, freeze);
                for (int i=0; i<alpha.size(); i++)
                    alpha[i] *=fade;
                alpha.replace(bufPos/2,1);
                bufPos = (bufPos+2)%buffSize;
                freeze = Frozen::UNFROZEN;
                if (bufLen<(buffSize-1)) {
                    bufLen += 2;
                }
            }
        }

    }
}

void BreastGraph::rcvImgPosition(Image imgPos)
{
//    if (refPoints.length()<3) {
//        QMatrix4x4 trfMatrix = imgPos.getPosition();
//        if (refPoints.length()==0) {
//            imgHeight = transform->getHeight(&trfMatrix);
//        }
//        QVector3D pos = transform->getLowest(&trfMatrix);
//        if (side==Side::ND)
//            side = (pos.z()<0) ? Side::LEFT : Side::RIGHT;
//        refPoints.append(pos);
//        if (!planeSet && refPoints.length()==3) {
//            setPlane(refPoints);
//            *points<<project(QVector3D(pos));
//            lastPos = transform->getC(&trfMatrix);
//        }
//    } else {
//        if (bufLen==0) {
//            freeze = Frozen::UNFROZEN;
//            bufLen++;
//        }
//        QVector3D pos0 = transform->getOrig(&trfMatrix);
//        QVector3D posX = transform->getX(&trfMatrix);
//        QVector3D posY = transform->getY(&trfMatrix);
////        qDebug() << "x: " << pos0.x() << ", y: " << pos0.y() << ", z: " << pos0.z();
//        bool dist = checkDistance(pos0);
//        bool speed = checkSpeeed(transform->getC(&trfMatrix));
//        bool orient = checkOrientation(pos0, posY);
//        if (dist && speed && orient) {
//            if (buffSize==0) {
//                probePos.append(project(pos0));
//                probePos.append(project(posX));
//                freezPoints.append(freeze);
//                if (freezPoints.last()==FROZEN)
//                    qDebug() << "last freezpoint frozen";
//                if (freeze==FROZEN)
//                    qDebug() << "bg: frozen";
//                freeze = UNFROZEN;
//            } else {
//                probePos.replace(bufPos, project(pos0));
//                probePos.replace(bufPos+1, project(posX));
//                freezPoints.replace(bufPos/2, freeze);
//                bufPos = (bufPos+2)%buffSize;
//                freeze = Frozen::UNFROZEN;
//                if (bufLen<buffSize)
//                    bufLen += 2;
//            }
//        }

//    }
}


void BreastGraph::receiveFrozen(int imgNumber)
{
    freeze = FROZEN;
    qDebug() << "receiveFrozen, buflen=" << bufLen;
}
