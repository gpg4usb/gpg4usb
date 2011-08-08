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
    void addImportAction();
    void removeImportAction();
    QStringList *keysNotInList;
    void setVerifyDetailText(QString text);

signals:

public slots:
    void importFromKeyserver();
    void showVerifyDetails();

private:
    QMenu *detailMenu;
    QLabel *verifyLabel;
    GpgME::Context *mCtx;
    QHBoxLayout *notificationWidgetLayout;
    QAction *importFromKeyserverAct;
    QAction *showVerifyDetailsAct;
    QString *verifyDetailText;
};
#endif // VERIFYNOTIFICATION_H
