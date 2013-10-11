#ifndef QMLPAGE_H
#define QMLPAGE_H

#include <QWidget>
#include "kgpg/core/kgpgkey.h"

class QMLPage : public QWidget
{

    Q_OBJECT

public:
    QMLPage(KgpgCore::KgpgKey key, QWidget *parent = 0);

};

#endif // QMLPAGE_H
