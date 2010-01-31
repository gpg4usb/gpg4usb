/*
 *      keydetailsdialog.cpp
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
 
#include "keydetailsdialog.h"
//#include "QDebug"

KeyDetailsDialog::KeyDetailsDialog(GpgME::Context* ctx, gpgme_key_t key) {
    
    mCtx = ctx;
    keyid = new QString(key->subkeys->keyid);
	
	ownerBox = new QGroupBox(tr("Owner details"));
	keyBox = new QGroupBox(tr("Key details"));
	fingerprintBox = new QGroupBox(tr("Fingerprint"));
	buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));
	
    nameLabel = new QLabel(tr("Name:"));
    emailLabel = new QLabel(tr("E-Mailaddress:"));
    commentLabel = new QLabel(tr("Comment:"));
    keySizeLabel = new QLabel(tr("Key size:"));
    expireLabel = new QLabel(tr("Expires on: "));
    createdLabel = new QLabel(tr("Created on: "));
    algorithmLabel = new QLabel(tr("Algorithm: "));

    nameVarLabel = new QLabel(key->uids->name);
    emailVarLabel = new QLabel(key->uids->email);
    commentVarLabel = new QLabel(key->uids->comment);
    //keySizeVarLabel = new QLabel();
    QString keySizeVal, keyExpireVal, keyCreatedVal, keyAlgoVal;
    
    if ( key->subkeys->expires == 0 ) {
        keyExpireVal = tr("Never");
    } else {
        keyExpireVal = QDateTime::fromTime_t(key->subkeys->expires).toString("dd. MMM. yyyy");
    }
    
    keyAlgoVal = gpgme_pubkey_algo_name(key->subkeys->pubkey_algo);
    keyCreatedVal = QDateTime::fromTime_t(key->subkeys->timestamp).toString(tr("dd. MMM. yyyy"));
    
    // have el-gamal key?
    if(key->subkeys->next) {
        keySizeVal.sprintf("%d / %d",  int(key->subkeys->length), int(key->subkeys->next->length) );
        if ( key->subkeys->next->expires == 0 ) {
            keyExpireVal += tr(" / Never");
        } else {
            keyExpireVal += " / " + QDateTime::fromTime_t(key->subkeys->next->expires).toString(tr("dd. MMM. yyyy"));
        }
        keyAlgoVal.append(" / ").append(gpgme_pubkey_algo_name(key->subkeys->next->pubkey_algo));
        keyCreatedVal += " / " + QDateTime::fromTime_t(key->subkeys->next->timestamp).toString(tr("dd. MMM. yyyy"));
    } else {
        keySizeVal.setNum( int(key->subkeys->length));
    }
    
    keySizeVarLabel = new QLabel(keySizeVal);
    expireVarLabel = new QLabel(keyExpireVal);
	createdVarLabel = new QLabel(keyCreatedVal);
    algorithmVarLabel = new QLabel(keyAlgoVal);

   
    mvbox = new QVBoxLayout();
    vboxKD = new QGridLayout();
	vboxOD = new QGridLayout();
    
    vboxOD->addWidget(nameLabel, 0, 0);
    vboxOD->addWidget(emailLabel, 1, 0);
    vboxOD->addWidget(commentLabel, 2, 0);
    vboxOD->addWidget(nameVarLabel, 0, 1);
    vboxOD->addWidget(emailVarLabel, 1, 1);
    vboxOD->addWidget(commentVarLabel, 2, 1);

    vboxKD->addWidget(keySizeLabel, 0, 0);
    vboxKD->addWidget(expireLabel, 1, 0);
    vboxKD->addWidget(algorithmLabel, 3, 0);
    vboxKD->addWidget(createdLabel, 4, 0);

    vboxKD->addWidget(keySizeVarLabel, 0, 1);
    vboxKD->addWidget(expireVarLabel, 1, 1);
    vboxKD->addWidget(algorithmVarLabel, 3, 1);
    vboxKD->addWidget(createdVarLabel, 4, 1);
    
	ownerBox->setLayout(vboxOD);
	mvbox->addWidget(ownerBox);
	
	keyBox->setLayout(vboxKD);
	mvbox->addWidget(keyBox);
	    
	vboxFP = new QVBoxLayout();
    fingerPrintLabel = new QLabel(beautifyFingerprint(key->subkeys->fpr));
    fingerPrintLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    vboxFP->addWidget(fingerPrintLabel);	
	fingerprintBox->setLayout(vboxFP);
	mvbox->addWidget(fingerprintBox);

    if(key->secret) {
        QGroupBox *privKeyBox = new QGroupBox(tr("Private Key"));
        QVBoxLayout *vboxPK = new QVBoxLayout();
        
        QPushButton *exportButton = new QPushButton(tr("Export Private Key"));
        vboxPK->addWidget(exportButton);
        connect(exportButton, SIGNAL(clicked()), this, SLOT(exportPrivateKey()));
        
        privKeyBox->setLayout(vboxPK);
        mvbox->addWidget(privKeyBox);
    }

	mvbox->addWidget(buttonBox);
	
	this->setLayout(mvbox);
    this->setWindowTitle(tr("Keydetails"));
	this->show();

	exec();
}

void KeyDetailsDialog::exportPrivateKey() {
    
    int ret = QMessageBox::information(this, tr("Exporting private Key"),
                            tr("You are about to export your private key.\n"
                               "This is NOT your public key, so don't give it away.\n"
                               "Make sure you keep it save."),
                            QMessageBox::Cancel | QMessageBox::Ok);
                            
    if(ret==QMessageBox::Ok) {                       
    
        QByteArray *keyArray = new QByteArray();
        mCtx->exportSecretKey(*keyid, keyArray);
        
        QString fileName = QFileDialog::getSaveFileName(this, tr("Export Key To File"), "", tr("Key Files") + " (*.asc *.txt);;All Files (*)");
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        QTextStream stream(&file);
        stream << *keyArray;
        file.close();
        delete keyArray;
    }
    
}

QString KeyDetailsDialog::beautifyFingerprint(QString fingerprint) {
	uint len = fingerprint.length();
	if((len>0) && (len%4 == 0))
		for (uint n = 0; 4 * (n + 1) < len; ++n)
			fingerprint.insert(5 * n + 4, ' ');
	return fingerprint;
}
