/*
 *      keydetailsdialog.cpp
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

#include "keydetailsdialog.h"
#include "kgpg/transactions/kgpgexport.h"
#include "kgpg/core/convert.h"

KeyDetailsDialog::KeyDetailsDialog(GpgME::GpgContext* ctx, KgpgCore::KgpgKey key, QWidget *parent)
    : QDialog(parent)
{
    mCtx = ctx;
    keyid = new QString(key.id());

    ownerBox = new QGroupBox(tr("Owner details"));
    keyBox = new QGroupBox(tr("Key details"));
    fingerprintBox = new QGroupBox(tr("Fingerprint"));
    additionalUidBox = new QGroupBox(tr("Additional Uids"));
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));

    nameVarLabel = new QLabel(key.name());
    nameVarLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    emailVarLabel = new QLabel(key.email());
    emailVarLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    commentVarLabel = new QLabel(key.comment());
    commentVarLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    keyidVarLabel = new QLabel(key.id());
    keyidVarLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    QString keySizeVal, keyExpireVal, keyCreatedVal, keyAlgoVal;

    if (key.expirationDate().isNull()) {
        keyExpireVal = tr("Never");
    } else {
        keyExpireVal = KgpgCore::Convert::toString(key.expirationDate().date());
    }

    keyAlgoVal = KgpgCore::Convert::toString(key.algorithm()) + QLatin1String( " / " ) + KgpgCore::Convert::toString(key.encryptionAlgorithm());
    keyCreatedVal = KgpgCore::Convert::toString(key.creationDate().date());

    keySizeVal = QString::number(key.size()) + "/" + QString::number(key.encryptionSize());

    keySizeVarLabel = new QLabel(keySizeVal);
    keySizeVarLabel->setWhatsThis(tr("<qt>The left part is the algorithm used by the <b>signature</b> key. The right part is the algorithm used by the <b>encryption</b> key.</qt>"));
    expireVarLabel = new QLabel(keyExpireVal);
    createdVarLabel = new QLabel(keyCreatedVal);
    algorithmVarLabel = new QLabel(keyAlgoVal);

    QVBoxLayout *mvbox = new QVBoxLayout();
    QGridLayout *vboxKD = new QGridLayout();
    QGridLayout *vboxOD = new QGridLayout();

    vboxOD->addWidget(new QLabel(tr("Name:")), 0, 0);
    vboxOD->addWidget(new QLabel(tr("Email address:")), 1, 0);
    vboxOD->addWidget(new QLabel(tr("Comment:")), 2, 0);
    vboxOD->addWidget(nameVarLabel, 0, 1);
    vboxOD->addWidget(emailVarLabel, 1, 1);
    vboxOD->addWidget(commentVarLabel, 2, 1);

    vboxKD->addWidget(new QLabel(tr("Key size:")), 0, 0);
    vboxKD->addWidget(new QLabel(tr("Expires on: ")), 1, 0);
    vboxKD->addWidget(new QLabel(tr("Algorithm: ")), 3, 0);
    vboxKD->addWidget(new QLabel(tr("Created on: ")), 4, 0);
    vboxKD->addWidget(new QLabel(tr("Key ID: ")), 5, 0);
    vboxKD->addWidget(keySizeVarLabel, 0, 1);
    vboxKD->addWidget(expireVarLabel, 1, 1);
    vboxKD->addWidget(algorithmVarLabel, 3, 1);
    vboxKD->addWidget(createdVarLabel, 4, 1);
    vboxKD->addWidget(keyidVarLabel, 5, 1);

    ownerBox->setLayout(vboxOD);
    mvbox->addWidget(ownerBox);

    keyBox->setLayout(vboxKD);
    mvbox->addWidget(keyBox);

    fingerPrintVarLabel = new QLabel(key.fingerprintBeautified());
    fingerPrintVarLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    fingerPrintVarLabel->setStyleSheet("margin-left: 20; margin-right: 20;");
    QHBoxLayout *hboxFP = new QHBoxLayout();

    hboxFP->addWidget(fingerPrintVarLabel);
    QIcon ico(":button_copy.png");

    QPushButton copyFingerprintButton(QIcon(ico.pixmap(12, 12)), "");
    //copyFingerprintButton.setStyleSheet("QPushButton {border: 0px; } QPushButton:Pressed {}  ");
    copyFingerprintButton.setFlat(true);
    copyFingerprintButton.setToolTip(tr("copy fingerprint to clipboard"));
    connect(&copyFingerprintButton, SIGNAL(clicked()), this, SLOT(slotCopyFingerprint()));

    hboxFP->addWidget(&copyFingerprintButton);

    fingerprintBox->setLayout(hboxFP);
    mvbox->addWidget(fingerprintBox);

    // If key has more than primary uid, also show the other uids
    /*gpgme_user_id_t addUserIds = key->uids->next;
    if (addUserIds !=NULL) {
        QVBoxLayout *vboxUID = new QVBoxLayout();
        while (addUserIds != NULL){
            addUserIdsVarLabel = new QLabel(addUserIds->name+ QString(" <")+addUserIds->email+">");
            addUserIdsVarLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
            vboxUID->addWidget(addUserIdsVarLabel);
            addUserIds=addUserIds->next;
        }
        additionalUidBox->setLayout(vboxUID);
        mvbox->addWidget(additionalUidBox);
    }*/

    //if (key.secret()) {
    if(mCtx->isSecretKey(key.id())) {
        QGroupBox *privKeyBox = new QGroupBox(tr("Private Key"));
        QVBoxLayout *vboxPK = new QVBoxLayout();

        QPushButton *exportButton = new QPushButton(tr("Export Private Key"));
        vboxPK->addWidget(exportButton);
        connect(exportButton, SIGNAL(clicked()), this, SLOT(slotExportPrivateKey()));

        privKeyBox->setLayout(vboxPK);
        mvbox->addWidget(privKeyBox);
    }

    /*if((key->expired) || (key->revoked)) {
        QHBoxLayout *expBox = new QHBoxLayout();
        QIcon icon = QIcon::fromTheme("dialog-warning");
        QPixmap pixmap = icon.pixmap(QSize(32,32),QIcon::Normal,QIcon::On);

        QLabel *expLabel = new QLabel();
        QLabel *iconLabel = new QLabel();
        if (key->expired) {
            expLabel->setText(tr("Warning: Key expired"));
        }
        if (key->revoked) {
            expLabel->setText(tr("Warning: Key revoked"));
        }

        iconLabel->setPixmap(pixmap);
        QFont font = expLabel->font();
        font.setBold(true);
        expLabel->setFont(font);
        expBox->addWidget(iconLabel);
        expBox->addWidget(expLabel);
        mvbox->addLayout(expBox);
    }*/

    mvbox->addWidget(buttonBox);

    this->setLayout(mvbox);
    this->setWindowTitle(tr("Keydetails"));
    this->setModal(true);
    this->show();

    exec();
}

void KeyDetailsDialog::slotExportPrivateKey()
{
    // Show a information box with explanation about private key
    int ret = QMessageBox::information(this, tr("Exporting private Key"),
                                       tr("You are about to export your private key.\n"
                                          "This is NOT your public key, so don't give it away.\n"
                                          "Make sure you keep it save."
                                          "Do you really want to export your private key?"),
                                       QMessageBox::Cancel | QMessageBox::Ok);

    // export key, if ok was clicked
    if (ret == QMessageBox::Ok) {
       /* QByteArray *keyArray = new QByteArray();
        mCtx->exportSecretKey(*keyid, keyArray);
        KgpgCore::KgpgKey key = mCtx->getKeyDetails(*keyid);
        QString fileString = key.name() + " " + key.email() + "(" + key.id()+ ")_pub_sec.asc";
        QString fileName = QFileDialog::getSaveFileName(this, tr("Export Key To File"), fileString, tr("Key Files") + " (*.asc *.txt);;All Files (*)");
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(0,tr("Export error"),tr("Couldn't open %1 for writing").arg(fileName));
            return;
        }
        QTextStream stream(&file);
        stream << *keyArray;
        file.close();
        delete keyArray;*/
        KgpgCore::KgpgKey key = mCtx->getKeyDetails(*keyid);
        QString fileString = key.name() + " " + key.email() + "(" + key.id()+ ")_pub_sec.asc";

        QString fileName = QFileDialog::getSaveFileName(this, tr("Export Key To File"), fileString, tr("Key Files") + " (*.asc *.txt);;All Files (*)");

        QStringList expopts;
        expopts.append(QLatin1String( "--armor" ));
        KGpgExport *exp = new KGpgExport(this, QStringList() << *keyid, fileName, expopts, true);
        connect(exp, SIGNAL(done(int)), SLOT(slotExportPrivateKeyDone(int)));
        exp->start();
    }
}

void KeyDetailsDialog::slotExportPrivateKeyDone(int result) {
    KGpgExport *exp = qobject_cast<KGpgExport *>(sender());
    Q_ASSERT(exp != NULL);

    if (result == KGpgTransaction::TS_OK) {
        qDebug() << "export seems ok";
    } else {
       qDebug() << "Your key could not be exported\nCheck the key.";
    }

    exp->deleteLater();
}

QString KeyDetailsDialog::beautifyFingerprint(QString fingerprint)
{
    uint len = fingerprint.length();
    if ((len > 0) && (len % 4 == 0))
        for (uint n = 0; 4 *(n + 1) < len; ++n)
            fingerprint.insert(5 * n + 4, ' ');
    return fingerprint;
}

void KeyDetailsDialog::slotCopyFingerprint() {
    QString fpr = fingerPrintVarLabel->text().trimmed().replace(" ", "");
    QClipboard *cb = QApplication::clipboard();
    cb->setText(fpr);
}
