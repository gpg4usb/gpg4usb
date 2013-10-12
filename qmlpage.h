#ifndef QMLPAGE_H
#define QMLPAGE_H

#include <QWidget>
#include <QDeclarativeContext>
#include <QGraphicsObject>
#include "kgpg/core/kgpgkey.h"

class QMLPage : public QWidget
{

    Q_OBJECT

public:
    QMLPage(KgpgCore::KgpgKey key, QWidget *parent = 0);

public slots:
    void qmlClicked();

private:
    QDeclarativeContext *context;
    QGraphicsObject *obj;

};

#endif // QMLPAGE_H
