/*
 *      keyimportdetailsdialog.cpp
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

#include "keyimportdetaildialog.h"

KeyImportDetailDialog::KeyImportDetailDialog(GpgME::GpgContext* ctx, KeyList* keyList, gpgme_import_result_t result, QWidget *parent)
    : QDialog(parent)
{
    mCtx = ctx;
    mKeyList = keyList;
    mResult = result;
    // If no key for import found, just ahow a message
    if (mResult->considered == 0) {
        QMessageBox::information(0, tr("Key import details"), tr("No keys found to import"));
        return;
    }

    mvbox = new QVBoxLayout();

    this->createGeneralInfoBox();
    this->createKeysTable();
    this->createButtonBox();

    this->setLayout(mvbox);
    this->setWindowTitle(tr("Key import details"));
    this->setModal(true);
    this->show();
}

void KeyImportDetailDialog::createGeneralInfoBox()
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

void KeyImportDetailDialog::createKeysTable()
{
    // get details for the imported keys;
    gpgme_import_status_t status = mResult->imports;

    keysTable = new QTableWidget(this);
    keysTable->setRowCount(0);
    keysTable->setColumnCount(4);
    keysTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // Nothing is selectable
    keysTable->setSelectionMode(QAbstractItemView::NoSelection);

    QStringList headerLabels;
    headerLabels  << tr("Name") << tr("Email") << tr("KeyID") << tr("Status");
    keysTable->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
    keysTable->horizontalHeader()->setStretchLastSection(true);

    keysTable->verticalHeader()->hide();
    keysTable->setHorizontalHeaderLabels(headerLabels);
    int row = 0;
    while (status != NULL) {
        keysTable->setRowCount(row+1);
        GpgKey key = mKeyList->getKeyByFpr(status->fpr);
        keysTable->setItem(row, 0, new QTableWidgetItem(key.name));
        keysTable->setItem(row, 1, new QTableWidgetItem(key.email));
        keysTable->setItem(row, 2, new QTableWidgetItem(key.id));

        qDebug() << "Keystatus: " << status->status;
        // Set status of key
        int keystatus=status->status;
        QString statusString;
        // if key is private
        if (keystatus > 15) {
            statusString.append("private");
            keystatus=keystatus-16;
        } else {
            statusString.append("public");
        }
        if (keystatus == 0) {
            statusString.append(", "+tr("unchanged"));
        } else {
            if (keystatus == 1) {
                statusString.append(", "+tr("new key"));
            } else {
                if (keystatus > 7) {
                    statusString.append(", "+tr("new subkey"));
                    keystatus=keystatus-8;
                }
                if (keystatus > 3) {
                    statusString.append(", "+tr("new signature"));
                    keystatus=keystatus-4;
                }
                if (keystatus > 1) {
                    statusString.append(", "+tr("new uid"));
                    keystatus=keystatus-2;
                }
            }
        }

//        keysTable->setItem(row, 3, new QTableWidgetItem(QString::number(status->status)));
        keysTable->setItem(row,3,new QTableWidgetItem(statusString));
        status=status->next;
        row++;
    }
    mvbox->addWidget(keysTable);
}

void KeyImportDetailDialog::createButtonBox()
{
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(close()));
    mvbox->addWidget(buttonBox);
}
