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

KeyServerImportDialog::KeyServerImportDialog(QWidget *parent)
    : QDialog(parent)
{
    message->setAutoFillBackground(true);

    closeButton = createButton(tr("&Close"), SLOT(close()));
    importButton = createButton(tr("&Import"), SLOT(import()));
    searchButton = createButton(tr("&Search"), SLOT(search()));
    searchLineEdit = new QLineEdit();
    keyServerComboBox = createComboBox("http://pgp.mit.edu");

    searchLabel = new QLabel(tr("Seacrh string:"));
    keyServerLabel = new QLabel(tr("Keyserver:"));
    message = new QLabel;

    createKeysTree();

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
    mainLayout->addWidget(keysTree, 3, 0, 1, 3);
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

void KeyServerImportDialog::import()
{
    updateComboBox(keyServerComboBox);
    setMessage("keys imported",false);
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

void KeyServerImportDialog::createKeysTree()
{
    keysTree = new QTreeWidget();
    keysTree->setColumnCount(2);

    QStringList labels;
    labels << tr("KeyID") << tr("UID");
    keysTree->setHeaderLabels(labels);
    /*keysTree->setSelectionBehavior(QAbstractItemView::SelectRows);
    keysTree->setHorizontalHeaderLabels(labels);
    keysTree->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    keysTree->verticalHeader()->hide();
    keysTree->setShowGrid(false);*/

    //connect(keysTree, SIGNAL(cellActivated(int,int)),
    //        this, SLOT(importKeyOfItem(int,int)));
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

void KeyServerImportDialog::importKeyOfItem(int row, int /* column */)
{
    /*QTableWidgetItem *item = keysTree->item(row, 0);
    QDesktopServices::openUrl(QUrl::fromLocalFile(currentDir.absoluteFilePath(item->text())));
    */
}

void KeyServerImportDialog::search()
{
    QUrl url = keyServerComboBox->currentText()+":11371/pks/lookup?search="+searchLineEdit->text()+"&op=index&options=mr";
    reply = qnam.get(QNetworkRequest(url));
    connect(reply, SIGNAL(finished()),
            this, SLOT(searchFinished()));
}

void KeyServerImportDialog::searchFinished()
{
    QString firstLine = QString(reply->readLine(1024));
    // TODO: die liste erstmal leeren, bevor sie neu betankt wird
    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (reply->error()) {
        setMessage("Error while contacting keyserver!",true);
    } else {
        qDebug() << "downloaded";
    }
    if (firstLine.contains("Error"))
    {
        QString text= QString(reply->readLine(1024));
        if (text.contains("Too many responses")) {
            setMessage("Too many responses from keyserver!",true);
        }
        if (text.contains("No keys found")) {
            setMessage("No keys found containing the search string!",true);
        }
    } else {
        char buff[1024];
        QList <QTreeWidgetItem*> items;
        while (reply->readLine(buff,sizeof(buff)) !=-1) {
            QStringList line= QString(buff).split(":");
            qDebug() << line;
            if (line[0] == "pub") {
                QStringList line2 = QString(reply->readLine()).split(":");
                QStringList l;
                l << line[1] << line2[1];
                qDebug() << "l:" << l;
                items.append(new QTreeWidgetItem((QTreeWidget*) 0,l));
            } else {
                if (line[0] == "uid") {
                    QStringList l;
                    l << "" << line[1];
                    items.last()->addChild(new QTreeWidgetItem((QTreeWidget*) 0, l));
                }
            }
        }
     keysTree->insertTopLevelItems(0,items);
    }
    reply->deleteLater();
    reply = 0;
}
