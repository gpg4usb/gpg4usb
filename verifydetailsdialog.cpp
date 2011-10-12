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

VerifyDetailsDialog::VerifyDetailsDialog(QWidget *parent, GpgME::Context* ctx, KeyList* keyList, QPlainTextEdit *edit) :
    QDialog(parent)
{
    mCtx = ctx;
    mKeyList = keyList;
    mTextpage = edit;
    this->setWindowTitle(tr("Signaturedetails"));

    connect(mCtx, SIGNAL(keyDBChanged()), this, SLOT(refresh()));

    mVbox = new QVBoxLayout();
    this->setLayout(mVbox);
    refresh();

    this->exec();
}

void VerifyDetailsDialog::refresh()
{
    // At first claer all children widgets
    QList<QLabel *> allChildren = mVbox->findChildren<QLabel *>();
    foreach (QLabel *label,allChildren) {
        label->close();
    }

    // Get signature information of current text
    QByteArray text = mTextpage->toPlainText().toAscii(); // TODO: toUtf8() here?
    mCtx->preventNoDataErr(&text);
    gpgme_signature_t sign = mCtx->verify(text);

    // Get timestamp of signature of current text
    QDateTime timestamp;
    timestamp.setTime_t(sign->timestamp);

    // Set the title widget depending on sign status
    switch (mCtx->textIsSigned(text))
    {
    case 2:
    {
        mVbox->addWidget(new QLabel(tr("Text was completly signed on %1 by:\n").arg(timestamp.toString(Qt::SystemLocaleShortDate))));
        break;
    }
    case 1:
    {
        mVbox->addWidget(new QLabel(tr("Text was partially signed on %1 by:\n").arg(timestamp.toString(Qt::SystemLocaleShortDate))));
        break;
    }
    }

    // Add informationbox for every single key
    while (sign) {
        VerifyKeyDetailBox *sbox = new VerifyKeyDetailBox(this,mCtx,mKeyList,sign);
        sign = sign->next;
        mVbox->addWidget(sbox);
    }

    // Button Box for close button
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(close()));
    mVbox->addWidget(buttonBox);
}
