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
    // set the text of verifynotification
    void setVerifyLabel(QString text);
    // show the import action in menu
    void showImportAction();
    // hide the import action in menu
    void hideImportAction();
    // List holding the keys in signature, which are not in the keylist
    QStringList *keysNotInList;
    // set text shown in verifydetails dialog
    void setVerifyDetailText(QString text);

signals:

public slots:
    // import missing key from keyserver
    void importFromKeyserver();
    // show verify details
    void showVerifyDetails();

private:
    QMenu *detailMenu; // Menu for te Button in verfiyNotification
    QAction *importFromKeyserverAct;
    QAction *showVerifyDetailsAct;
    QString *verifyDetailText; // Text showed in VerifiyNotification
    QPushButton *detailsButton; // Button shown in verifynotification
    QLabel *verifyLabel; // Label holding the text shown in verifyNotification
    GpgME::Context *mCtx;
    QHBoxLayout *notificationWidgetLayout;
};
#endif // VERIFYNOTIFICATION_H
