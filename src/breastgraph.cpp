#include <QtCore/qmath.h>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QPointF>
#include <QVector>
#include <QVector3D>
#include <QVector4D>
#include <QMatrix4x4>
#include <QLineF>
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
qreal fade = 0.99;
bool planeSet = false;

QVector<QPointF> refProjection, probePos;
QVector<Image> lines;
QVector<Frozen> freezPoints;
QVector<qreal> alpha;
QVector3D lastPos;
int bufPos=0, bufLen=0;
qreal alphaLast=1;

BreastGraph::BreastGraph(Transform *transform, int fps, int buffSize, qreal radii[], int segments[], QWidget *parent) : QWidget(parent)
{
    this->setObjectName("BreastGraph");
    points = new QVector<QPointF>;
    this->fps = fps;
    this->radii = radii;
    this->segments = segments;
    refPoints.reserve(3);
    refProjection.reserve(3);
    this->transform = transform;
    if (buffSize>0) {
        alpha = QVector<qreal>(buffSize);
        for (int i=0; i<buffSize; i++) {
            alpha[i] = alphaLast;
            alphaLast *= fade;
        }
        this->buffSize = buffSize;
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

void BreastGraph::drawBackground(QPainter *painter, const QColor color)
{
    // background
    painter->save();
    painter->setPen(QPen(color));
    painter->setBrush(QBrush(color, Qt::SolidPattern));
    painter->drawEllipse(QPointF(0.0,0.0), r, r);
    painter->rotate((side==Side::LEFT) ? -angle : angle);
    painter->drawPolygon(lobe);
    painter->restore();
}

void BreastGraph::drawSnake(QPainter *painter, const QColor color, QVector<Image> *lines)
{
    // green "snake"
    painter->save();
    painter->setPen(QPen(color, 0));
    painter->setBrush(QBrush(color, Qt::SolidPattern));
    for (int i=1; i<lines->size(); i++) {
            QLineF line1 = lines->at(i-1).getLine();
            QLineF line2 = lines->at(i).getLine();
            QPointF points[4] = {line1.p1(), line1.p2(), line2.p2(), line2.p1()};
            painter->drawPolygon(points, 4);
    }
    painter->restore();

}

void BreastGraph::drawProbe(QPainter *painter, const QColor probe, const QColor freeze, QVector<Image> *lines)
{
    QColor blueCol = probe;
    painter->save();
    for (int i=0; i<bufLen; i++) {
        if (lines->at(lines->size()-i-1).getStatus()==Frozen::UNFROZEN) {
            blueCol.setAlphaF(alpha[i]);
            painter->setPen(QPen(blueCol, 0));
            QLineF line = lines->at(lines->size()-i-1).getLine();
            painter->drawLine(line);
            painter->drawEllipse(line.p2(), 2, 2);
        }
    }
    painter->setPen(QPen(freeze, 3));
    for (int i=0; i<lines->size(); i++) {
        if (lines->at(i).getStatus()==Frozen::FROZEN) {
            QLineF line = lines->at(i).getLine();
            painter->drawLine(line);
            QPointF point = line.p2();
            painter->drawEllipse(point, 2.0, 2.0);
        }
    }
    painter->restore();
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
    painter.setRenderHint(QPainter::Antialiasing);
    drawBackground(&painter, Qt::red);
    drawSnake(&painter, Qt::green, &lines);
    drawProbe(&painter, Qt::blue, Qt::red, &lines);

    // segmented circles
    painter.setPen(QPen(Qt::black, 0, Qt::SolidLine));
    painter.setBrush(QBrush(Qt::transparent, Qt::SolidPattern));
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


void BreastGraph::rcvImgPosition(Image imgPos)
{
    QMatrix4x4 trfMatrix = imgPos.getPosition();
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
        QVector3D pos0 = transform->getOrig(&trfMatrix);
        QVector3D posX = transform->getX(&trfMatrix);
        QVector3D posY = transform->getY(&trfMatrix);
        bool dist = checkDistance(pos0);
        bool orient = checkOrientation(pos0, posY);
//        if (dist && orient) {
        if (dist) {
            imgPos.setLine(project(pos0), project(posX));
            Image tmpImg = imgPos;
            lines.append(tmpImg);
            if (buffSize==0) {
                alpha.append(alphaLast);
                alphaLast *= fade;
                bufLen++;
            } else {
                bufPos = (bufPos++)%buffSize;
                if (bufLen<(buffSize-1)) {
                    bufLen++;
                }
            }
        }

    }
}

