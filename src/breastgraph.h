#ifndef BREASTGRAPH_H
#define BREASTGRAPH_H
#include <QWidget>
#include <QVector>
#include <QVector3D>
#include <QVector4D>
#include <QPolygonF>
#include "side.h"


class BreastGraph : public QWidget
{
    Q_OBJECT
public:
    explicit BreastGraph(Side side, qreal angle, QWidget *parent = 0);
    explicit BreastGraph(qreal angle, QWidget *parent = 0);
    ~BreastGraph();
    void paintEvent(QPaintEvent *);
    void setSide(Side side);
    QPointF project(QVector3D x);

private:
    QPolygonF lobe;
    qreal angle;
    QVector<QVector4D> refPoints;
    Side side = Side::ND;
    QVector3D un, vn;
    QVector3D origin;
    void setPlane(QVector<QVector4D> refPoints);


signals:

public slots:
    void setPosition(QVector4D pos);
};

#endif // BREASTGRAPH_H