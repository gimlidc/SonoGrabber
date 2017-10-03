#ifndef SONOIMAGERESIZABLE_H
#define SONOIMAGERESIZABLE_H

#include <QWidget>
#include <QtWidgets/QLabel>

class SonoImageResizable : public QWidget
{
    Q_OBJECT
public:
    explicit SonoImageResizable(QWidget *parent = nullptr);

private:
    QLabel *image;

signals:

public slots:
};

#endif // SONOIMAGERESIZABLE_H
