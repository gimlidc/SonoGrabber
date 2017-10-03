#ifndef FREEZEMENU_H
#define FREEZEMENU_H

#include <QWidget>
#include <QGroupBox>

class FreezeMenu : public QWidget
{
    Q_OBJECT
public:
    explicit FreezeMenu(QWidget *parent = nullptr);

private:
    QGroupBox *menu;

signals:

public slots:
};

#endif // FREEZEMENU_H
