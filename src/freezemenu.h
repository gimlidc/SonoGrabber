#ifndef FREEZEMENU_H
#define FREEZEMENU_H

#include <QWidget>
#include <QtWidgets>
#include <QGroupBox>

class FreezeMenu : public QWidget
{
    Q_OBJECT
public:
    explicit FreezeMenu(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *);

private:
    QGroupBox *menu;
    QVBoxLayout *buttons, *stopLayout;
    bool record=false;

signals:
    void unfreeze();
    void startRecord();

public slots:
    void showMenu();
    void hideMenu();

private slots:
    void handleRecord();

};


#endif // FREEZEMENU_H
