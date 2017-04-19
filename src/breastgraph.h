#ifndef BREASTGRAPH_H
#define BREASTGRAPH_H
#include <QWidget>
#include "side.h"


class BreastGraph : public QWidget
{
    Q_OBJECT
public:
    explicit BreastGraph(Side side, QWidget *parent = 0);
    ~BreastGraph();
    void paintEvent(QPaintEvent *);

signals:

public slots:
};

#endif // BREASTGRAPH_H
