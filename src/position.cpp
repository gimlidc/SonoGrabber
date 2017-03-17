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

