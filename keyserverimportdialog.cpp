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
#include <QPixmap>
#include "keyserverimportdialog.h"

KeyServerImportDialog::KeyServerImportDialog(GpgME::Context *ctx, QWidget *parent)
    : QDialog(parent)
{
    mCtx = ctx;
    message = new QLabel;
    icon = new QLabel;
 QIcon undoicon = QIcon::fromTheme("dialog-information");
    QPixmap pixmap = undoicon.pixmap(QSize(32,32),QIcon::Normal,QIcon::On);
    icon->setPixmap(pixmap);
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
    mainLayout->addWidget(icon, 4, 0, 1, 3);
    mainLayout->addWidget(message, 4, 1, 1, 3);
    mainLayout->addLayout(buttonsLayout, 5, 0, 1, 3);
    setLayout(mainLayout);

    setWindowTitle(tr("Import Keys from Keyserver"));
    resize(700, 300);
    setModal(true);
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
    keysTable->setColumnCount(3);
    keysTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    keysTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QStringList labels;
    labels  << tr("UID") << tr("Creation date") << tr("KeyID");
    keysTable->setHorizontalHeaderLabels(labels);
    keysTable->verticalHeader()->hide();
    keysTable->horizontalHeader()->setResizeMode(0, QHeaderView::ResizeToContents);
    setMessage("doubleclick on a key to import it", false);
    connect(keysTable, SIGNAL(cellActivated(int,int)),
            this, SLOT(import()));

}

void KeyServerImportDialog::setMessage(const QString &text, bool error)
{
    message->setText(text);
    if (error) {
        QIcon undoicon = QIcon::fromTheme("dialog-error");
        QPixmap pixmap = undoicon.pixmap(QSize(32,32),QIcon::Normal,QIcon::On);
        icon->setPixmap(pixmap);

    } else {
        QIcon undoicon = QIcon::fromTheme("dialog-information");
        QPixmap pixmap = undoicon.pixmap(QSize(32,32),QIcon::Normal,QIcon::On);
        icon->setPixmap(pixmap);

    }
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
        setMessage(tr("Couldn't contact keyserver!"),true);

    }
    if (firstLine.contains("Error"))
    {
        QString text= QString(searchreply->readLine(1024));
        if (text.contains("Too many responses")) {
            setMessage(tr("Too many responses from keyserver!"),true);
        }
        if (text.contains("No keys found")) {
            setMessage(tr("No keys found containing the search string!"),true);
        }
    } else {
        keysTable->clearContents();
        int row = 0;
        char buff[1024];
        QList <QTreeWidgetItem*> items;
        while (searchreply->readLine(buff,sizeof(buff)) !=-1) {
            QStringList line= QString(buff).split(":");
            if (line[0] == "pub") {
                keysTable->setRowCount(row+1);
                QStringList line2 = QString(searchreply->readLine()).split(":");
                QTableWidgetItem *uid = new QTableWidgetItem(line2[1]);
                keysTable->setItem(row, 0, uid);
                QTableWidgetItem *creationdate = new QTableWidgetItem(QDateTime::fromTime_t(line[4].toInt()).toString("dd. MMM. yyyy"));
                keysTable->setItem(row, 1, creationdate);
                QTableWidgetItem *keyid = new QTableWidgetItem(line[1]);
                keysTable->setItem(row, 2, keyid);
                row++;
            } else {
                if (line[0] == "uid") {
                    QStringList l;
                    int height=keysTable->rowHeight(row-1);
                    keysTable->setRowHeight(row-1,height+16);
                    QString tmp=keysTable->item(row-1,0)->text();
                    tmp.append(QString("\n")+line[1]);
                    QTableWidgetItem *tmp1 = new QTableWidgetItem(tmp);
                    keysTable->setItem(row-1,0,tmp1);
                }
            }
            setMessage(tr("%1 keys found. Doubleclick a key to import it.").arg(row),false);
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
    QString keyid = keysTable->item(keysTable->currentRow(),2)->text();
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
        setMessage(tr("Error while contacting keyserver!"),true);
        return;
    }
    mCtx->importKey(*key);
    setMessage("key imported",false);
    importreply->deleteLater();
    importreply = 0;
}

