#ifndef QMLPAGE_H
#define QMLPAGE_H

#include <QWidget>
#include <QDeclarativeContext>
#include <QGraphicsObject>
#include "kgpg/core/kgpgkey.h"
#include "gpgcontext.h"

class QMLPage : public QWidget
{

    Q_OBJECT

public:
    QMLPage(GpgME::GpgContext *ctx, KgpgCore::KgpgKey key, QWidget *parent = 0);

public slots:
    void qmlClicked();
    void slotExportPublicKey();
    void slotExportPrivateKey();
private:
    QDeclarativeContext *context;
    QGraphicsObject *obj;
    GpgME::GpgContext *mCtx;
};

#endif // QMLPAGE_H
