/*
 *      importdetailsdialog.cpp
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

#include "importdetaildialog.h"

ImportDetailDialog::ImportDetailDialog(GpgME::GpgContext* ctx, KeyList* keyList, gpgme_import_result_t result, QWidget *parent)
    : QDialog(parent)
{
    mCtx = ctx;
    mKeyList = keyList;
    mResult = result;

    mvbox = new QVBoxLayout();

    this->createGeneralInfoBox();
    this->createKeyInfoBox();

    // Create ButtonBox for OK-Button
    okButtonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(okButtonBox, SIGNAL(rejected()), this, SLOT(close()));
    mvbox->addWidget(okButtonBox);

    this->setLayout(mvbox);
    this->setWindowTitle(tr("Key import details"));
    this->setModal(true);
    this->exec();
}

void ImportDetailDialog::createGeneralInfoBox()
{
    // GridBox for general import information
    generalInfoBox = new QGroupBox(tr("Genral key import info"));
    generalInfoBoxLayout = new QGridLayout(generalInfoBox);

    generalInfoBoxLayout->addWidget(new QLabel(tr("Considered:")),1,0);
    generalInfoBoxLayout->addWidget(new QLabel(QString::number(mResult->considered)),1,1);
    int row=2;
    if (mResult->unchanged){
        generalInfoBoxLayout->addWidget(new QLabel(tr("Unchanged:")),row,0);
        generalInfoBoxLayout->addWidget(new QLabel(QString::number(mResult->unchanged)),row,1);
        row++;
    }
    if (mResult->imported){
        generalInfoBoxLayout->addWidget(new QLabel(tr("Imported:")),row,0);
        generalInfoBoxLayout->addWidget(new QLabel(QString::number(mResult->imported)),row,1);
        row++;
    }
    if (mResult->not_imported){
        generalInfoBoxLayout->addWidget(new QLabel(tr("Not imported:")),row,0);
        generalInfoBoxLayout->addWidget(new QLabel(QString::number(mResult->not_imported)),row,1);
        row++;
    }
    if (mResult->secret_read){
        generalInfoBoxLayout->addWidget(new QLabel(tr("Secret read:")),row,0);
        generalInfoBoxLayout->addWidget(new QLabel(QString::number(mResult->secret_read)),row,1);
        row++;
    }
    if (mResult->secret_imported){
        generalInfoBoxLayout->addWidget(new QLabel(tr("Secret imported:")),row,0);
        generalInfoBoxLayout->addWidget(new QLabel(QString::number(mResult->secret_imported)),row,1);
        row++;
    }
    if (mResult->secret_unchanged){
        generalInfoBoxLayout->addWidget(new QLabel(tr("Secret unchanged:")),row,0);
        generalInfoBoxLayout->addWidget(new QLabel(QString::number(mResult->secret_unchanged)),row,1);
        row++;
    }
    mvbox->addWidget(generalInfoBox);
}

void ImportDetailDialog::createKeyInfoBox()
{
    // get details for the imported keys;
    gpgme_import_status_t status = mResult->imports;

    keyInfoBox = new QGroupBox(tr("Detailed key import info"));
    keyInfoBoxLayout = new QGridLayout(keyInfoBox);

    int keyrow=1;

    // process the whole list of keys
    while (status != NULL) {
        GpgKey key = mKeyList->getKeyByFpr(status->fpr);
        keyInfoBoxLayout->addWidget(new QLabel(key.name),keyrow,2);
        keyInfoBoxLayout->addWidget(new QLabel(key.email),keyrow,3);
        keyInfoBoxLayout->addWidget(new QLabel(key.id),keyrow,4);

        status=status->next;
        keyrow++;
    }

    // Add the boxes to mainwidget
    mvbox->addWidget(keyInfoBox);
}

