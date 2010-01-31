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

KeyDetailsDialog::KeyDetailsDialog(gpgme_key_t key) {
	
	setWindowTitle(tr("Key Properties"));
    resize(500, 200);
    setModal(true);
	
	ownerBox = new QGroupBox(tr("Owner details"));
	keyBox = new QGroupBox(tr("Key details"));
	fingerprintBox = new QGroupBox(tr("Fingerprint"));
	buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
	connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));
	
    nameLabel = new QLabel(tr("Name:"));
    emailLabel = new QLabel(tr("E-Mailaddress:"));
    commentLabel = new QLabel(tr("Comment:"));
    keySizeLabel = new QLabel(tr("KeySize:"));
    expireLabel = new QLabel(tr("Expires on: "));
    createdLabel = new QLabel(tr("Created on: "));
    algorithmLabel = new QLabel(tr("Algorithm"));

    nameVarLabel = new QLabel(key->uids->name);
    emailVarLabel = new QLabel(key->uids->email);
    commentVarLabel = new QLabel(key->uids->comment);
    keySizeVarLabel = new QLabel();
    keySizeVarLabel->setNum(int(key->subkeys->length));
    if ( key->subkeys->expires == 0 ) {
		expireVarLabel = new QLabel(tr("Never"));
	 } else {
		expireVarLabel = new QLabel(QDateTime::fromTime_t(key->subkeys->expires).toString("dd. MMM. yyyy"));
	}
	createdVarLabel = new QLabel(QDateTime::fromTime_t(key->subkeys->timestamp).toString("dd. MMM. yyyy"));
    algorithmVarLabel = new QLabel(gpgme_pubkey_algo_name(key->subkeys->pubkey_algo));
   
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

	mvbox->addWidget(buttonBox);
	
	this->setLayout(mvbox);
    this->setWindowTitle(tr("Generate Key"));
	this->show();
	
/*		qDebug() << "is secret: " << key ->secret;
		qDebug() << "can encrypt: " <<key ->can_encrypt;
		qDebug() << "can sign: " <<key ->can_sign;
		qDebug() << "can encrypt: " <<key ->can_encrypt;
		qDebug() << "expires: " << key-> expired;
		qDebug() << "can authenticate: " <<key ->can_authenticate;
		qDebug() << "protocol: " << gpgme_get_protocol_name(key->protocol);
		qDebug() << "algo: " << gpgme_pubkey_algo_name(key->subkeys->pubkey_algo);
	*/	
	exec();
}

QString KeyDetailsDialog::beautifyFingerprint(QString fingerprint) {
	uint len = fingerprint.length();
	if((len>0) && (len%4 == 0))
		for (uint n = 0; 4 * (n + 1) < len; ++n)
			fingerprint.insert(5 * n + 4, ' ');
	return fingerprint;
}
