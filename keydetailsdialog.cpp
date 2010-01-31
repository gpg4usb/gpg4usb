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
#include "QDebug"
#include "QDateTime"
 
KeyDetailsDialog::KeyDetailsDialog(gpgme_key_t key) {
	
	setWindowTitle(tr("Key Properties"));
    resize(500, 200);
    setModal(true);

    nameLabel = new QLabel(tr("Name:"));
    emailLabel = new QLabel(tr("E-Mailaddress::"));
    commentLabel = new QLabel(tr("Comment:"));
    keySizeLabel = new QLabel(tr("KeySize:"));
    expireLabel = new QLabel(tr("Expires on: "));
    fingerPrintLabel = new QLabel(tr("Fingerprint"));
    algorithmLabel = new QLabel(tr("Algorithm"));

    nameVarLabel = new QLabel(key->uids->name);
    emailVarLabel = new QLabel(key->uids->email);
    commentVarLabel = new QLabel(key->uids->comment);
    keySizeVarLabel = new QLabel();
    keySizeVarLabel->setNum(int(key->subkeys->length));
    if ( key->subkeys->expires == 0 ) {
		expireVarLabel = new QLabel(tr("Never"));
	 } else {
		expireVarLabel = new QLabel(QDateTime::fromTime_t(key->subkeys->expires).toString());
	}
	
    fingerPrintVarLabel = new QLabel(key->subkeys->fpr);
    algorithmVarLabel = new QLabel(gpgme_pubkey_algo_name(key->subkeys->pubkey_algo));
    
    QGridLayout *vbox1 = new QGridLayout;
    vbox1->addWidget(nameLabel, 0, 0);
    vbox1->addWidget(emailLabel, 1, 0);
    vbox1->addWidget(commentLabel, 2, 0);
    vbox1->addWidget(keySizeLabel, 3, 0);
    vbox1->addWidget(expireLabel, 4, 0);
    vbox1->addWidget(fingerPrintLabel, 5, 0);
    vbox1->addWidget(algorithmLabel, 6, 0);
    vbox1->addWidget(nameVarLabel, 0, 1);
    vbox1->addWidget(emailVarLabel, 1, 1);
    vbox1->addWidget(commentVarLabel, 2, 1);
    vbox1->addWidget(keySizeVarLabel, 3, 1);
    vbox1->addWidget(expireVarLabel, 4, 1);
    vbox1->addWidget(fingerPrintVarLabel, 5, 1);
    vbox1->addWidget(algorithmVarLabel, 6, 1);

this->setLayout(vbox1);
    
this->setWindowTitle(tr("Generate Key"));
this->show();
/*if (key->uids) { qDebug() <<"UIds: ja";}
		qDebug() << "can encrypt: " <<key ->can_encrypt;
    
		qDebug() << "Name:" << key->uids->name;
		qDebug() << "E-Mail: " << key ->uids->email;
		qDebug() << "Komentar: " << key ->uids->comment;
		qDebug() << "Fingerprint:" << key ->subkeys->fpr;
		qDebug() << "Key-Length:" << key ->subkeys->length <<" bit";
		qDebug() << "creation date timestamp: " << key->subkeys->timestamp;
		qDebug() << "creation date timestamp: " << QDateTime::fromTime_t(key->subkeys->timestamp);
		qDebug() << "is secret: " << key ->secret;
		qDebug() << "can sign: " <<key ->can_sign;
		qDebug() << "can encrypt: " <<key ->can_encrypt;
		qDebug() << "expires: " << key-> expired;
		qDebug() << "can authenticate: " <<key ->can_authenticate;
		qDebug() << "protocol: " << gpgme_get_protocol_name(key->protocol);
		qDebug() << "algo: " << gpgme_pubkey_algo_name(key->subkeys->pubkey_algo);
	*/	
	exec();
}
