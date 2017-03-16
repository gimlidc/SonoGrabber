#include "position.h"
#include <QMatrix4x4>
#include <QDebug>
#include "sessionparams.h"

Position::Position(SessionParams * session) : session(session)
{
    qDebug("Position init");
}

Position::~Position()
{
    qDebug("Positiuon close");
}

void Position::receiveTransform(const igtl::TransformMessage::Pointer &transMsg)
{
    igtl::TimeStamp::Pointer ts;
    ts = igtl::TimeStamp::New();
    transMsg->GetTimeStamp(ts);

    int i = session->getTransNames().indexOf(transMsg->GetDeviceName());
    igtl::Matrix4x4 m;
    transMsg->GetMatrix(m);
    transform.append(QMatrix4x4((const float *)m));
}
