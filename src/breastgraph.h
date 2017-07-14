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
#include "frozen.h"
#include "image.h"

QT_CHARTS_USE_NAMESPACE

class BreastGraph : public QWidget
{
    Q_OBJECT
public:
    explicit BreastGraph(Transform *transform, int fps, int buffSize,
                         QVector<qreal> radii, QVector<int> segments, QWidget *parent = 0);
    ~BreastGraph();
    void paintEvent(QPaintEvent *);
    QPointF project(QVector3D x);
    void reset();

private:
    QPolygonF lobe;
    Transform *transform;
    int buffSize=0;
    int fps;
    Frozen freeze;
    QVector<qreal> radii;
    QVector<int> segments;

    QPolygonF getLobe(QPointF rp);
    qreal angle;
    QVector<QPointF> *points;
    QVector<QVector3D> refPoints;
    Side side = Side::ND;
    QVector3D un, vn;
    QVector3D origin;
    void setPlane(QVector<QVector3D> refPoints);
    void drawBackground(QPainter *painter, const QColor color);
    void drawSnake(QPainter *painter, const QColor color, QVector<Image> *extPoints);
    void drawProbe(QPainter *painter, const QColor probe, const QColor freeze,
                   QVector<Image> *lines);
    void drawGraph(QPainter *painter,
                   QVector<qreal> radii, QVector<int> segments);
    // get speed between the last and before-last element of probe position
    qreal getSpeed(QVector<Image> *probe);
    // get speed between the idx and idx-1 element of probe position
    qreal getSpeed(const QVector<Image> &probe, int idx);
    bool checkDistance(QVector3D point);
    bool checkOrientation(QVector3D p0, QVector3D py);
    bool checkSpeeed(QVector3D point);
    bool frozenLastStatus = false;

signals:

public slots:
    void rcvImgPosition(Image);
};


#endif // BREASTGRAPH_H
