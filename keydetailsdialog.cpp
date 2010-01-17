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
 
KeyDetailsDialog::KeyDetailsDialog(gpgme_key_t key) {
	
	setWindowTitle(tr("Key Properties"));
    resize(500, 200);
    setModal(true);

    if (key->uids && key->uids->name)
		qDebug() << "Name:" << key->uids->name;
		qDebug() << "E-Mail: " << key ->uids->email;
		qDebug() << "Komentar: " << key ->uids->comment;
		qDebug() << "Fingerprint:" << key ->subkeys->fpr;
		qDebug() << "Key-Length:" << key ->subkeys->length <<" bit";
		qDebug() << "creation date timestamp: " << key->subkeys->timestamp;
		qDebug() << "is secret: " << key ->secret;
		qDebug() << "can sign: " <<key ->can_sign;
		qDebug() << "can encrypt: " <<key ->can_encrypt;
		qDebug() << "expires: " << key-> expired;
		qDebug() << "can authenticate: " <<key ->can_authenticate;
		qDebug() << "protocol: " << gpgme_get_protocol_name(key->protocol);
		qDebug() << "algo: " << gpgme_pubkey_algo_name(key->subkeys->pubkey_algo);
		
	exec();
}
