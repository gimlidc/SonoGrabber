#ifndef BREASTGRAPH_H
#define BREASTGRAPH_H
#include <QWidget>
#include <QVector>
#include <QPolygonF>
#include "side.h"


class BreastGraph : public QWidget
{
    Q_OBJECT
public:
    explicit BreastGraph(Side side, qreal angle, QWidget *parent = 0);
    ~BreastGraph();
    void paintEvent(QPaintEvent *);

private:
    QPolygonF lobe;
    qreal angle;
//    QVector<QPointF> *lobe;


signals:

public slots:
};

#endif // BREASTGRAPH_H
