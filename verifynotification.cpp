
#include "verifynotification.h"

VerifyNotification::VerifyNotification(GpgME::Context *ctx, QWidget *parent ) :
    QWidget(parent)
{
    mCtx = ctx;

    //text shown in verify notification
    verifyLabel = new QLabel(this);
  //  QLabel *verifyLabel2 = new QLabel(this);
  //  verifyLabel2->setText("hallo");

    // Text contained in detilsdialog
    verifyDetailText = new QString();

    // List kontaining the signature keys not contained in keylist
    keysNotInList = new QStringList();

    importFromKeyserverAct = new QAction(tr("Import missing key from Keyserver"), this);
    connect(importFromKeyserverAct, SIGNAL(triggered()), this, SLOT(importFromKeyserver()));

    showVerifyDetailsAct = new QAction(tr("Show detailed verify information"), this);
    connect(showVerifyDetailsAct, SIGNAL(triggered()), this, SLOT(showVerifyDetails()));

    // Menu for the details button
    detailMenu = new QMenu(this);
    detailMenu->addAction(showVerifyDetailsAct);
    detailMenu->addAction(importFromKeyserverAct);
    importFromKeyserverAct->setVisible(false);

    // the details button
    detailsButton = new QPushButton("Details",this);
    detailsButton->setMenu(detailMenu);
    verifyLabelList = new QWidget(this);
    verifyLabelListLayout = new QVBoxLayout(this);
    verifyLabelList->setLayout(verifyLabelListLayout);
    //addVerifyLabel(verifyLabel->text(),QString("ok"));
    verifyLabel->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
//    verifyLabelListLayout->addWidget(verifyLabel2);
    //verifyLabel2->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    notificationWidgetLayout = new QHBoxLayout(this);
    verifyLabelListLayout->setContentsMargins(0,0,0,0);
    notificationWidgetLayout->setContentsMargins(0,0,0,0);
    notificationWidgetLayout->addWidget(verifyLabelList);
    notificationWidgetLayout->addWidget(detailsButton);
    detailsButton->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

    this->setLayout(notificationWidgetLayout);
}

void VerifyNotification::addVerifyLabel(QString text, QString status)
{
    QLabel *label = new QLabel(text);
    label->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
//    label->setContentsMargins(0,0,0,0);

    verifyLabelListLayout->addWidget(label);

    label->setObjectName(status);
}

void VerifyNotification::setVerifyLabel(QString text)
{
    verifyLabel->setText(text);
    return;
}

void VerifyNotification::importFromKeyserver()
{
    KeyServerImportDialog *importDialog =new KeyServerImportDialog(mCtx,this);
    foreach (QString keyid, *keysNotInList) {
        importDialog->import(keyid);
    }
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
