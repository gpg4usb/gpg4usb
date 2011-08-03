
#include "verifynotification.h"

VerifyNotification::VerifyNotification(GpgME::Context *ctx, QWidget *parent ) :
    QWidget(parent)
{
    mCtx = ctx;
    verifyLabel = new QLabel("Verified");

    QHBoxLayout *notificationWidgetLayout = new QHBoxLayout();
    notificationWidgetLayout->setContentsMargins(0,0,0,0);
    notificationWidgetLayout->addWidget(verifyLabel,2);

//    notificationWidget = new QWidget(this);

    //this->setStyleSheet("background-color: #CBFDCB;");
    this->setLayout(notificationWidgetLayout);

    QAction *importFromKeyserverAct = new QAction(tr("Import missing key from Keyserver"), this);
    connect(importFromKeyserverAct, SIGNAL(triggered()), this, SLOT(importFromKeyserver()));

    QMenu *detailMenu = new QMenu();
    detailMenu->addAction(importFromKeyserverAct);
    keysNotInList = new QStringList();
    QPushButton *verifyButton = new QPushButton("Details");
    verifyButton->setMenu(detailMenu);
   // notificationWidgetLayout->addStretch(1);
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
