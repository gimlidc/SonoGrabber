#ifndef POSITION_H
#define POSITION_H

#include "sessionparams.h"
#include "igtlTransformMessage.h"
#include <QObject>
#include <QMatrix4x4>

class Position : public QObject
{
    Q_OBJECT
public:
    explicit Position(SessionParams * session);
    ~Position();

public slots:
    void receiveTransform(const igtl::TransformMessage::Pointer& transMsg);

private:
    SessionParams *session;
    QList<igtl::TransformMessage::Pointer> transMsgList;
    QList<QMatrix4x4> transform;
    QVector<double> transTS;

};

#endif // POSITION_H
