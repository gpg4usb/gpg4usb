
#include "verifynotification.h"

VerifyNotification::VerifyNotification(GpgME::Context *ctx, QWidget *parent ) :
    QWidget(parent)
{
    mCtx = ctx;
    verifyLabel = new QLabel(this);

    notificationWidgetLayout = new QHBoxLayout(this);
    notificationWidgetLayout->setContentsMargins(0,0,0,0);
    notificationWidgetLayout->addWidget(verifyLabel,2);

    this->setLayout(notificationWidgetLayout);

    QAction *importFromKeyserverAct = new QAction(tr("Import missing key from Keyserver"), this);
    connect(importFromKeyserverAct, SIGNAL(triggered()), this, SLOT(importFromKeyserver()));

    QMenu *detailMenu = new QMenu(this);
    detailMenu->addAction(importFromKeyserverAct);
    keysNotInList = new QStringList();
    QPushButton *verifyButton = new QPushButton("Details",this);
    verifyButton->setMenu(detailMenu);
    notificationWidgetLayout->addWidget(verifyButton);
}

void VerifyNotification::importFromKeyserver(){
    KeyServerImportDialog *importDialog =new KeyServerImportDialog(mCtx,this);
    foreach (QString keyid, *keysNotInList) {
        importDialog->import(keyid);
    }
}

void VerifyNotification::setVerifyLabel(QString text)
{
    verifyLabel->setText(text);
    return;
}
