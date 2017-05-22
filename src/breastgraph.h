#ifndef BREASTGRAPH_H
#define BREASTGRAPH_H
#include <QWidget>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QVector>
#include <QVector3D>
#include <QVector4D>
#include <QPolygonF>
#include "side.h"

QT_CHARTS_USE_NAMESPACE

class BreastGraph : public QWidget
{
    Q_OBJECT
public:
    explicit BreastGraph(QRect crop, Side side, qreal angle, QWidget *parent = 0);
    explicit BreastGraph(qreal angle, QWidget *parent = 0);
    ~BreastGraph();
    void paintEvent(QPaintEvent *);
//    void setSide(Side side);
    QPointF project(QVector3D x);
    void reset();

private:
    QPolygonF lobe;
    QPolygonF getLobe(QPointF rp);
    qreal angle;
//    QScatterSeries *points;
//    QChart *chart;
//    QChartView *chartView;
    QVector<QPointF> *points;
    QVector<QVector3D> refPoints;
    Side side = Side::ND;
    QVector3D un, vn;
    QVector3D origin;
    void setPlane(QVector<QVector3D> refPoints);

signals:

public slots:
    void setPosition(QMatrix4x4 transform);
};

#endif // BREASTGRAPH_H
