/*
 *      verifydetailsdialog.cpp
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

#include "verifydetailsdialog.h"

VerifyDetailsDialog::VerifyDetailsDialog(QWidget *parent, GpgME::Context* ctx, KeyList* keyList, gpgme_signature_t signature) :
    QDialog(parent)
{
    mCtx = ctx;
    mKeyList = keyList;

    connect(mCtx, SIGNAL(keyDBChanged()), this, SLOT(refresh()));

    mVbox = new QVBoxLayout();

    // Timestamp of creation of the signature
    QDateTime timestamp;
    timestamp.setTime_t(signature->timestamp);

    // Information for general verify information
    mVbox->addWidget(new QLabel(tr("Text was completly signed on %1 by:\n").arg(timestamp.toString(Qt::SystemLocaleShortDate))));

    // Add informationbox for every single key
    while (signature) {
        VerifyKeyDetailBox *sbox = new VerifyKeyDetailBox(this,mCtx,mKeyList,signature);
        signature = signature->next;
        mVbox->addWidget(sbox);
    }

    // Button Box for close button
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));
    mVbox->addWidget(buttonBox);

    this->setLayout(mVbox);
    this->setWindowTitle(tr("Signaturedetails"));
    this->show();
    this->exec();
}

void VerifyDetailsDialog::refresh()
{
    qDebug() << "refresh detail dialog";
}
