/*
 *      verifynotification.cpp
 *
 *      Copyright 2008 gpg4usb-team <gpg4usb@cpunk.de>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

#include "verifynotification.h"

VerifyNotification::VerifyNotification(QWidget *parent, GpgME::Context *ctx, KeyList *keyList, gpgme_signature_t sign ) :
    QWidget(parent)
{
    mCtx = ctx;
    mKeyList = keyList;
    mSignature = sign;
    verifyLabel = new QLabel(this);

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
    notificationWidgetLayout->setContentsMargins(10,0,0,0);
    notificationWidgetLayout->addWidget(verifyLabel,2);
    notificationWidgetLayout->addWidget(detailsButton);
    this->setLayout(notificationWidgetLayout);
    verifyDetailListLayout = new QVBoxLayout();
}

void VerifyNotification::addVerifyDetailLabel(QString text,verify_label_status status,bool prepend)
{
    if (prepend) {
        verifyDetailStringVector.prepend(text);
        verifyDetailStatusVector.prepend(status);
    } else {
        verifyDetailStringVector.append(text);
        verifyDetailStatusVector.append(status);
    }
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

void VerifyNotification::showImportAction(bool visible)
{
    importFromKeyserverAct->setVisible(visible);
    return;
}

void VerifyNotification::showVerifyDetails()
{
    /*QDialog *verifyDetailsDialog = new QDialog(this);
  //  QLabel *label = new QLabel(*verifyDetailText);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttonBox, SIGNAL(rejected()), verifyDetailsDialog, SLOT(close()));
    for (int i=0;i<verifyDetailStringVector.size();i++) {
        QLabel *label = new QLabel(verifyDetailStringVector[i]);
        verifyDetailListLayout->addWidget(label);
        switch (verifyDetailStatusVector[i]) {
        case VERIFY_ERROR_OK:       label->setObjectName("ok");
                                    break;
        case VERIFY_ERROR_WARN:     label->setObjectName("warning");
                                    break;
        case VERIFY_ERROR_CRITICAL: label->setObjectName("critical");
                                    break;
        default:
                                    break;
        }
    }

//    verifyDetailStatusVector.append(status);

//    verifyDetailListLayout->addWidget(label);

    verifyDetailListLayout->addWidget(buttonBox);

    verifyDetailsDialog->setLayout(verifyDetailListLayout);
    verifyDetailsDialog->show();

    //QMessageBox::information(this,tr("Details"),QString(*verifyDetailText), QMessageBox::Ok);
    return;*/
    new VerifyDetailsDialog(this, mCtx, mKeyList, mSignature);
}
