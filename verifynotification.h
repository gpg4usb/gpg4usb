#ifndef VERIFYNOTIFICATION_H
#define VERIFYNOTIFICATION_H

#include <QLabel>
#include <QHBoxLayout>
#include <QMenu>
#include <QPushButton>

#include <QWidget>

#include "keyserverimportdialog.h"
#include "context.h"
#include <gpgme.h>


class VerifyNotification : public QWidget
{
    Q_OBJECT
public:
    explicit VerifyNotification(GpgME::Context *ctx,QWidget *parent = 0 );
    void setVerifyLabel(QString text);
    QStringList *keysNotInList;

signals:

public slots:
    void importFromKeyserver();

private:
    QLabel *verifyLabel;
    GpgME::Context *mCtx;
    QHBoxLayout *notificationWidgetLayout;
};

#endif // VERIFYNOTIFICATION_H
