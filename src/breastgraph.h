#ifndef BREASTGRAPH_H
#define BREASTGRAPH_H
#include <QWidget>
#include <QVector>
#include <QVector4D>
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
    QVector<QVector4D> refPoints;


signals:

public slots:
    void setPosition(QVector4D pos);
};

#endif // BREASTGRAPH_H
