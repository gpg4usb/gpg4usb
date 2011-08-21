
#include "verifynotification.h"

VerifyNotification::VerifyNotification(GpgME::Context *ctx, QWidget *parent ) :
    QWidget(parent)
{
    mCtx = ctx;
    verifyLabel = new QLabel(this);
    verifyDetailText = new QString();

    importFromKeyserverAct = new QAction(tr("Import missing key from Keyserver"), this);
    connect(importFromKeyserverAct, SIGNAL(triggered()), this, SLOT(importFromKeyserver()));

    showVerifyDetailsAct = new QAction(tr("Show detailed verify information"), this);
    connect(showVerifyDetailsAct, SIGNAL(triggered()), this, SLOT(showVerifyDetails()));

    detailMenu = new QMenu(this);
    detailMenu->addAction(showVerifyDetailsAct);
    detailMenu->addAction(importFromKeyserverAct);
    importFromKeyserverAct->setVisible(false);

    keysNotInList = new QStringList();

    detailsButton = new QPushButton("Details",this);
    detailsButton->setMenu(detailMenu);
    notificationWidgetLayout = new QHBoxLayout(this);
    notificationWidgetLayout->setContentsMargins(0,0,0,0);
    notificationWidgetLayout->addWidget(verifyLabel,2);
    notificationWidgetLayout->addWidget(detailsButton);

    this->setLayout(notificationWidgetLayout);
}

void VerifyNotification::setVerifyDetailText (QString text)
{
    verifyDetailText->clear();
    verifyDetailText->append(text);
    return;
}

void VerifyNotification::importFromKeyserver()
{
    KeyServerImportDialog *importDialog =new KeyServerImportDialog(mCtx,this);
    foreach (QString keyid, *keysNotInList) {
        importDialog->import(keyid);
    }
}

void VerifyNotification::setVerifyLabel(QString text, verify_label_status verifyLabelStatus)
{
    verifyLabel->setText(text);
    switch (verifyLabelStatus) {
    case VERIFY_ERROR_OK:       verifyLabel->setObjectName("ok");
                                break;
    case VERIFY_ERROR_WARN:     verifyLabel->setObjectName("warning");
                                break;
    case VERIFY_ERROR_CRITICAL: verifyLabel->setObjectName("critical");
                                break;
    default:
                                break;
    }
    return;
}

void VerifyNotification::showImportAction()
{
    importFromKeyserverAct->setVisible(true);
    return;
}
void VerifyNotification::hideImportAction()
{
    importFromKeyserverAct->setVisible(false);
    return;
}

void VerifyNotification::showVerifyDetails()
{
    QMessageBox::information(this,tr("Details"),QString(*verifyDetailText), QMessageBox::Ok);
    return;
}
