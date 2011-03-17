/*
 *
 *      keyserverimportdialog.cpp
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

#include <QtGui>
#include <QtNetwork>

#include "keyserverimportdialog.h"

KeyServerImportDialog::KeyServerImportDialog(GpgME::Context *ctx, QWidget *parent)
    : QDialog(parent)
{
    mCtx = ctx;
    message = new QLabel;
    message->setAutoFillBackground(true);

    closeButton = createButton(tr("&Close"), SLOT(close()));
    importButton = createButton(tr("&Import"), SLOT(import()));
    searchButton = createButton(tr("&Search"), SLOT(search()));
    searchLineEdit = new QLineEdit();
    keyServerComboBox = createComboBox("http://pgp.mit.edu");

    searchLabel = new QLabel(tr("Seacrh string:"));
    keyServerLabel = new QLabel(tr("Keyserver:"));
    createKeysTable();
    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(importButton);
    buttonsLayout->addWidget(closeButton);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(searchLabel, 1, 0);
    mainLayout->addWidget(searchLineEdit, 1, 1);
    mainLayout->addWidget(searchButton,1, 2);
    mainLayout->addWidget(keyServerLabel, 2, 0);
    mainLayout->addWidget(keyServerComboBox, 2, 1);
    mainLayout->addWidget(keysTable, 3, 0, 1, 3);
    mainLayout->addWidget(message, 4, 0, 1, 3);
    mainLayout->addLayout(buttonsLayout, 5, 0, 1, 3);
    setLayout(mainLayout);

    setWindowTitle(tr("Import Keys from Keyserver"));
    resize(700, 300);
}

static void updateComboBox(QComboBox *comboBox)
{
    if (comboBox->findText(comboBox->currentText()) == -1)
        comboBox->addItem(comboBox->currentText());
}

QPushButton *KeyServerImportDialog::createButton(const QString &text, const char *member)
{
    QPushButton *button = new QPushButton(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}

QComboBox *KeyServerImportDialog::createComboBox(const QString &text)
{
    QComboBox *comboBox = new QComboBox;
    comboBox->setEditable(true);
    comboBox->addItem(text);
    comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    return comboBox;
}

void KeyServerImportDialog::createKeysTable()
{
    keysTable = new QTableWidget();
    keysTable->setColumnCount(4);
    keysTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    keysTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStringList labels;
    labels << tr("KeyID") << tr("UID") << tr("Keysize") << tr("Creation date");
    keysTable->setHorizontalHeaderLabels(labels);
    keysTable->verticalHeader()->hide();
    keysTable->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);

    connect(keysTable, SIGNAL(cellActivated(int,int)),
            this, SLOT(import()));
}

void KeyServerImportDialog::setMessage(const QString &text, bool error)
{
    message->setText(text);
    QPalette filesFoundPalette = message->palette();
    if (error) {
        filesFoundPalette.setColor(QPalette::Background, "#20ff20");
    } else {
        filesFoundPalette.setColor(QPalette::Background, "#ff0000");
    }
    message->setPalette(filesFoundPalette);
}

void KeyServerImportDialog::search()
{
    QUrl url = keyServerComboBox->currentText()+":11371/pks/lookup?search="+searchLineEdit->text()+"&op=index&options=mr";
    searchreply = qnam.get(QNetworkRequest(url));
    connect(searchreply, SIGNAL(finished()),
            this, SLOT(searchFinished()));
}

void KeyServerImportDialog::searchFinished()
{
    QString firstLine = QString(searchreply->readLine(1024));

    QVariant redirectionTarget = searchreply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (searchreply->error()) {
        setMessage("Error while contacting keyserver!",true);
        return;
    }
    if (firstLine.contains("Error"))
    {
        QString text= QString(searchreply->readLine(1024));
        if (text.contains("Too many responses")) {
            setMessage("Too many responses from keyserver!",true);
        }
        if (text.contains("No keys found")) {
            setMessage("No keys found containing the search string!",true);
        }
    } else {
        keysTable->clearContents();
        int row = 0;
        char buff[1024];
        QList <QTreeWidgetItem*> items;
        while (searchreply->readLine(buff,sizeof(buff)) !=-1) {
            QStringList line= QString(buff).split(":");
            //      qDebug() << line;
            if (line[0] == "pub") {
                keysTable->setRowCount(row+1);
                QStringList line2 = QString(searchreply->readLine()).split(":");
                QStringList l;
                l << line[1] << line2[1];
                QTableWidgetItem *keyid = new QTableWidgetItem(line[1]);
                keysTable->setItem(row, 0, keyid);
                QTableWidgetItem *uid = new QTableWidgetItem(line2[1]);
                keysTable->setItem(row, 1, uid);
                QTableWidgetItem *keysize = new QTableWidgetItem(line[3]);
                keysTable->setItem(row, 2, keysize);
                QTableWidgetItem *creationdate = new QTableWidgetItem(QDateTime::fromTime_t(line[4].toInt()).toString("dd. MMM. yyyy"));
                keysTable->setItem(row, 3, creationdate);
                row++;
            } else {
                if (line[0] == "uid") {
                    QStringList l;
                    l << "" << line[1];
                    int height=keysTable->rowHeight(row-1);
                    keysTable->setRowHeight(row-1,height+16);
                    QString tmp=keysTable->item(row-1,1)->text();
                    tmp.append(QString("\n")+line[1]);
                    QTableWidgetItem *tmp1 = new QTableWidgetItem(tmp);
                    keysTable->setItem(row-1,1,tmp1);
                }
            }
        }
        //keysTree->insertTopLevelItems(0,items);
        keysTable->resizeColumnsToContents();
    }
    searchreply->deleteLater();
    searchreply = 0;
}

void KeyServerImportDialog::import()
{
    updateComboBox(keyServerComboBox);
    QString keyid = keysTable->item(keysTable->currentRow(),0)->text();
    QUrl url = keyServerComboBox->currentText()+":11371/pks/lookup?op=get&search=0x"+keyid+"&options=mr";
    importreply = qnam.get(QNetworkRequest(url));
    connect(importreply, SIGNAL(finished()),
            this, SLOT(importFinished()));
}
void KeyServerImportDialog::importFinished()
{
    QByteArray *key = new QByteArray();
    key->append(importreply->readAll());

    // TODO: die liste erstmal leeren, bevor sie neu betankt wird
    QVariant redirectionTarget = importreply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (importreply->error()) {
        setMessage("Error while contacting keyserver!",true);
    } else {
        qDebug() << "downloaded";
    }
    qDebug() << *key;
    mCtx->importKey(*key);
    setMessage("key imported",false);
    importreply->deleteLater();
    importreply = 0;
}

