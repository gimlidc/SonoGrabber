#ifndef CENTRALWINDOW_H
#define CENTRALWINDOW_H

#include <QWidget>
#include "startsequence.h"
#include "startsequencescene.h"
#include "sonoimageresizable.h"
#include "sessionparams.h"
#include "transform.h"
#include "igtlinkclient.h"

class CentralWindow : public QWidget
{
    Q_OBJECT
public:
    explicit CentralWindow(SessionParams * session, IGTLinkClient * client,
                           QWidget *parent = nullptr);

signals:

public slots:

private:
    StartSequence *startSequence;
    StartSequenceScene *startSequenceScene;
    SonoImageResizable *image;

    SessionParams *params;
    Transform *transform;
};

#endif // CENTRALWINDOW_H
