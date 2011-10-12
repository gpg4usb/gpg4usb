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

VerifyNotification::VerifyNotification(QWidget *parent, GpgME::Context *ctx, KeyList *keyList,QPlainTextEdit *edit) :
    QWidget(parent)
{
    mCtx = ctx;
    mKeyList = keyList;
    mTextpage = edit;
    verifyLabel = new QLabel(this);

    connect(mCtx, SIGNAL(keyDBChanged()), this, SLOT(refresh()));

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
    new VerifyDetailsDialog(this, mCtx, mKeyList, mTextpage);
}

bool VerifyNotification::refresh()
{
    verify_label_status verifyStatus=VERIFY_ERROR_OK;

    QByteArray text = mTextpage->toPlainText().toAscii(); // TODO: toUtf8() here?
    mCtx->preventNoDataErr(&text);
    int textIsSigned = mCtx->textIsSigned(text);

    gpgme_signature_t sign = mCtx->verify(text);

    if (sign == NULL) {
        return false;
    }

    QString verifyLabelText;
    bool unknownKeyFound=false;

    while (sign) {
        switch (gpg_err_code(sign->status))
        {
        case GPG_ERR_NO_PUBKEY:
        {
            verifyStatus=VERIFY_ERROR_WARN;
            verifyLabelText.append(tr("Key not present with Fingerprint: ")+mCtx->beautifyFingerprint(QString(sign->fpr)));
            this->keysNotInList->append(sign->fpr);
            unknownKeyFound=true;
            break;
        }
        case GPG_ERR_NO_ERROR:
        {
            QString name = mKeyList->getKeyNameByFpr(sign->fpr);
            QString email =mKeyList->getKeyEmailByFpr(sign->fpr);
            verifyLabelText.append(name);
            if (!email.isEmpty()) {
                verifyLabelText.append("<"+email+">");
            }
            break;
        }
        default:
        {
            verifyStatus=VERIFY_ERROR_WARN;
            verifyLabelText.append(tr("Error for key with fingerprint ")+mCtx->beautifyFingerprint(QString(sign->fpr)));
            break;
        }
        }
        verifyLabelText.append("\n");
        sign = sign->next;
    }

    switch (textIsSigned)
    {
    case 2:
    {
        verifyLabelText.prepend(tr("Text is completly signed by: "));
        break;
    }
    case 1:
    {
        verifyLabelText.prepend(tr("Text is partially signed by: "));
        break;
    }
    }

    // If an unknown key is found, enable the importfromkeyserveraction
    this->showImportAction(unknownKeyFound);

    // Remove the last linebreak
    verifyLabelText.remove(verifyLabelText.length()-1,1);

    this->setVerifyLabel(verifyLabelText,verifyStatus);

    return true;
}
