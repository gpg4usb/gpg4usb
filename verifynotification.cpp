/*
 *      verifynotification.cpp
 *
 *      Copyright 2008 gpg4usb-team <gpg4usb@cpunk.de>
 *
 *      This file is part of gpg4usb.
 *
 *      Gpg4usb is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *
 *      Gpg4usb is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with gpg4usb.  If not, see <http://www.gnu.org/licenses/>
 */

#include "verifynotification.h"

VerifyNotification::VerifyNotification(QWidget *parent, GpgME::GpgContext *ctx, KeyList *keyList,QTextEdit *edit) :
    QWidget(parent)
{
    mCtx = ctx;
    mKeyList = keyList;
    mTextpage = edit;
    verifyLabel = new QLabel(this);

    connect(mCtx, SIGNAL(keyDBChanged()), this, SLOT(refresh()));
    connect(edit, SIGNAL(textChanged()), this, SLOT(close()));

    importFromKeyserverAct = new QAction(tr("Import missing key from Keyserver"), this);
    connect(importFromKeyserverAct, SIGNAL(triggered()), this, SLOT(importFromKeyserver()));

    showVerifyDetailsAct = new QAction(tr("Show detailed verify information"), this);
    connect(showVerifyDetailsAct, SIGNAL(triggered()), this, SLOT(showVerifyDetails()));

    detailMenu = new QMenu(this);
    detailMenu->addAction(showVerifyDetailsAct);
    detailMenu->addAction(importFromKeyserverAct);
    importFromKeyserverAct->setVisible(false);

    keysNotInList = new QStringList();
    detailsButton = new QPushButton(tr("Details"),this);
    detailsButton->setMenu(detailMenu);
    QHBoxLayout *notificationWidgetLayout = new QHBoxLayout(this);
    notificationWidgetLayout->setContentsMargins(10,0,0,0);
    notificationWidgetLayout->addWidget(verifyLabel,2);
    notificationWidgetLayout->addWidget(detailsButton);
    this->setLayout(notificationWidgetLayout);
}

void VerifyNotification::importFromKeyserver()
{
    KeyServerImportDialog *importDialog =new KeyServerImportDialog(mCtx,mKeyList, this);
    importDialog->import(*keysNotInList);
}

void VerifyNotification::setVerifyLabel(QString text, verify_label_status verifyLabelStatus)
{
    QString color;
    verifyLabel->setText(text);
    switch (verifyLabelStatus) {
    case VERIFY_ERROR_OK:       color="#ccffcc";
                                break;
    case VERIFY_ERROR_WARN:     color="#ececba";
                                break;
    case VERIFY_ERROR_CRITICAL: color="#ff8080";
                                break;
    default:
                                break;
    }

    verifyLabel->setAutoFillBackground(true);
    QPalette status = verifyLabel->palette();
    status.setColor(QPalette::Background, color);
    verifyLabel->setPalette(status);
}                    

void VerifyNotification::showImportAction(bool visible)
{
    importFromKeyserverAct->setVisible(visible);
}

void VerifyNotification::showVerifyDetails()
{
    new VerifyDetailsDialog(this, mCtx, mKeyList, mTextpage);
}

bool VerifyNotification::refresh()
{
    verify_label_status verifyStatus=VERIFY_ERROR_OK;

    QByteArray text = mTextpage->toPlainText().toUtf8();
    mCtx->preventNoDataErr(&text);
    int textIsSigned = mCtx->textIsSigned(text);

    //gpgme_signature_t sign = mCtx->verify(text);

    //KGpgVerify *verify = new KGpgVerify(this, message.mid(posstart, posend - posstart));
    KGpgVerify *verify = new KGpgVerify(this, text);
    connect(verify, SIGNAL(done(int)), SLOT(slotVerifyDone(int)));
    verify->start();

    /*
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
                verifyLabelText.append(tr("Key not present with id 0x")+QString(sign->fpr));
                this->keysNotInList->append(sign->fpr);
                unknownKeyFound=true;
                break;
            }
            case GPG_ERR_NO_ERROR:
            {
                GpgKey key = mCtx->getKeyByFpr(sign->fpr);
                verifyLabelText.append(key.name);
                if (!key.email.isEmpty()) {
                    verifyLabelText.append("<"+key.email+">");
                }
                break;
            }
            case GPG_ERR_BAD_SIGNATURE:
            {
                textIsSigned = 3;
                verifyStatus=VERIFY_ERROR_CRITICAL;
                GpgKey key = mCtx->getKeyById(sign->fpr);
                verifyLabelText.append(key.name);
                if (!key.email.isEmpty()) {
                    verifyLabelText.append("<"+key.email+">");
                }
                break;
            }
            default:
            {
                //textIsSigned = 3;
                verifyStatus=VERIFY_ERROR_WARN;
                //GpgKey key = mKeyList->getKeyByFpr(sign->fpr);
                verifyLabelText.append(tr("Error for key with fingerprint ")+mCtx->beautifyFingerprint(QString(sign->fpr)));
                break;
            }
        }
        verifyLabelText.append("\n");
        sign = sign->next;
    }

    switch (textIsSigned)
    {
        case 3:
            {
                verifyLabelText.prepend(tr("Error validating signature by: "));
                break;
            }
        case 2:
            {
                verifyLabelText.prepend(tr("Text was completely signed by: "));
                break;
            }
        case 1:
            {
                verifyLabelText.prepend(tr("Text was partially signed by: "));
                break;
            }
    }

    // If an unknown key is found, enable the importfromkeyserveraction
    this->showImportAction(unknownKeyFound);

    // Remove the last linebreak
    verifyLabelText.remove(verifyLabelText.length()-1,1);

    this->setVerifyLabel(verifyLabelText,verifyStatus);
    */
    return true;
}

void VerifyNotification::slotVerifyDone(int result)
{
    const KGpgVerify * const verify = qobject_cast<KGpgVerify *>(sender());
    sender()->deleteLater();
    Q_ASSERT(verify != NULL);

    if (result == KGpgVerify::TS_MISSING_KEY) {
        qDebug() << "missing keys" << verify->missingId();
        this->keysNotInList->append(verify->missingId());
        this->showImportAction(true);
    }

    const QStringList messages = verify->getMessages();
    foreach(QString mess, messages) {
        qDebug() << "vm: " <<  mess;
    }

    getReport(messages);
    /*emit verifyFinished();

    if (result == KGpgVerify::TS_MISSING_KEY) {
        verifyKeyNeeded(verify->missingId());
        return;
    }

    const QStringList messages = verify->getMessages();

    if (messages.isEmpty())
        return;

    QStringList msglist;
    foreach (QString rawmsg, messages)
        msglist << rawmsg.replace(QLatin1Char('<'), QLatin1String("&lt;"));

    (void) new KgpgDetailedInfo(this, KGpgVerify::getReport(messages, m_model),
            msglist.join(QLatin1String("<br/>")),
            QStringList(), i18nc("Caption of message box", "Verification Finished"));
    */
}

QString VerifyNotification::getReport(const QStringList &log)
{
    QString verifyLabelText;
    verify_label_status verifyStatus=VERIFY_ERROR_OK;

    QString result;
    // newer versions of GnuPG emit both VALIDSIG and GOODSIG
    // for a good signature. Since VALIDSIG has more information
    // we use that.
    const QRegExp validsig(QLatin1String("^\\[GNUPG:\\] VALIDSIG([ ]+[^ ]+){10,}.*$"));


//	const bool useGoodSig = (model != NULL) && (log.indexOf(validsig) == -1);
    const bool useGoodSig = false;

    QString sigtime;	// timestamp of signature creation

    foreach (const QString &line, log) {
        if (!line.startsWith(QLatin1String("[GNUPG:] ")))
            continue;

        const QString msg = line.mid(9);

        if (msg.startsWith(QLatin1String("VALIDSIG ")) && !useGoodSig) {
            // from GnuPG source, doc/DETAILS:
            //   VALIDSIG    <fingerprint in hex> <sig_creation_date> <sig-timestamp>
            //                <expire-timestamp> <sig-version> <reserved> <pubkey-algo>
            //                <hash-algo> <sig-class> <primary-key-fpr>
            const QStringList vsig = msg.mid(9).split(QLatin1Char(' '), QString::SkipEmptyParts);
            Q_ASSERT(vsig.count() >= 10);

            qDebug() << "sig: " << vsig[9];

            verifyStatus=VERIFY_ERROR_WARN;
            verifyLabelText.append(tr("Error for key with fingerprint ")+mCtx->beautifyFingerprint(QString(vsig[9])));


        }
    }

    this->setVerifyLabel(verifyLabelText,verifyStatus);
            /*const KGpgKeyNode *node = model->findKeyNode(vsig[9]);

            if (node != NULL) {
                // ignore for now if this is signed with the primary id (vsig[0] == vsig[9]) or not
                if (node->getEmail().isEmpty())
                    result += tr("<qt>Good signature from:<br /><b>%1</b><br />Key ID: %2<br /></qt>")
                            .arg(node->getName()).arg(vsig[9]);
                else
                    result += tr("Good signature from: NAME <EMAIL>, Key ID: HEXID",
                            "<qt>Good signature from:<br /><b>%1 &lt;%2&gt;</b><br />Key ID: %3<br /></qt>")
                            .arg(node->getName()).arg(node->getEmail()).arg(vsig[9]);

                result += sigTimeMessage(vsig[2]);
            } else {
                // this should normally never happen, but one could delete
                // the key just after the verification. Brute force solution:
                // do the whole report generation again, but this time make
                // sure GOODSIG is used.
                return getReport(log, NULL);
            }
        } else if (msg.startsWith(QLatin1String("UNEXPECTED")) ||
                msg.startsWith(QLatin1String("NODATA"))) {
            result += tr("No signature found.") + QLatin1Char('\n');
        } else if (useGoodSig && msg.startsWith(QLatin1String("GOODSIG "))) {
            int sigpos = msg.indexOf( ' ' , 8);
            const QString keyid = msg.mid(8, sigpos - 8);

            // split the name/email pair to give translators more power to handle this
            QString email;
            QString name = msg.mid(sigpos + 1);

            int oPos = name.indexOf(QLatin1Char('<'));
            int cPos = name.indexOf(QLatin1Char('>'));
            if ((oPos >= 0) && (cPos >= 0)) {
                email = name.mid(oPos + 1, cPos - oPos - 1);
                name = name.left(oPos).simplified();
            }

            if (email.isEmpty())
                result += tr("<qt>Good signature from:<br /><b>%1</b><br />Key ID: %2<br /></qt>")
                        .arg(name).arg(keyid);
            else
                result += tr("Good signature from: NAME <EMAIL>, Key ID: HEXID",
                             "<qt>Good signature from:<br /><b>%1 &lt;%2&gt;</b><br />Key ID: %3<br /></qt>")
                        .arg(name).arg(email).arg(keyid);
            if (!sigtime.isEmpty()) {
                result += sigTimeMessage(sigtime);
                sigtime.clear();
            }
        } else if (msg.startsWith(QLatin1String("SIG_ID "))) {
            const QStringList parts = msg.simplified().split(QLatin1Char(' '));
            if (parts.count() > 2)
                sigtime = parts[2];
        } else if (msg.startsWith(QLatin1String("BADSIG"))) {
            int sigpos = msg.indexOf( ' ', 7);
            result += tr("<qt><b>BAD signature</b> from:<br /> %1<br />Key id: %2<br /><br /><b>The file is corrupted</b><br /></qt>")
                    .arg(msg.mid(sigpos + 1).replace(QLatin1Char('<'), QLatin1String("&lt;")))
                    .arg(msg.mid(7, sigpos - 7));
        } else  if (msg.startsWith(QLatin1String("TRUST_UNDEFINED"))) {
            result += tr("<qt>The signature is valid, but the key is untrusted<br /></qt>");
        } else if (msg.startsWith(QLatin1String("TRUST_ULTIMATE"))) {
            result += tr("<qt>The signature is valid, and the key is ultimately trusted<br /></qt>");
        }
    }
    */
    return result;
}

