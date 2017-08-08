#ifndef STARTSEQUENCESCENE_H
#define STARTSEQUENCESCENE_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QSvgWidget>

#include "transform.h"

class StartSequenceScene : public QWidget
{
    Q_OBJECT

public:
    StartSequenceScene(Transform *pos, QWidget *parent = 0);
    void reset();

private:
    void initImage(QWidget *parent);
    QGraphicsView *view;
    QGraphicsScene *scene;
    void showEvent(QShowEvent *event);

    QSvgWidget *imageL;
};

#endif // STARTSEQUENCESCENE_H
