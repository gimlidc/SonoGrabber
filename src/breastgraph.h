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
#include "transform.h"

QT_CHARTS_USE_NAMESPACE

class BreastGraph : public QWidget
{
    Q_OBJECT
public:
    explicit BreastGraph(Transform *transform, Side side, QWidget *parent = 0);
    explicit BreastGraph(Transform *transform, QWidget *parent = 0);
    ~BreastGraph();
    void paintEvent(QPaintEvent *);
    QPointF project(QVector3D x);
    void reset();

private:
    QPolygonF lobe;
    Transform *transform;
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
    bool checkDistance(QVector3D point);
    bool checkOrientation(QVector3D p0, QVector3D py);

signals:

public slots:
    void setPosition(QMatrix4x4 transform);
};

#endif // BREASTGRAPH_H
