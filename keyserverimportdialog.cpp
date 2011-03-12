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
    //text = new QString();

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
    message->setText("hallo");
    message->setAutoFillBackground(true);
    QPalette filesFoundPalette = message->palette();
    filesFoundPalette.setColor(QPalette::Background, "#20ff20");
    message->setPalette(filesFoundPalette);
}

void KeyServerImportDialog::showKeys(const QStringList &keys)
{
    /*for (int i = 0; i < keys.size(); ++i) {
        QFile file(currentDir.absoluteFilePath(keys[i]));
        qint64 size = QFileInfo(file).size();

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(keys[i]);
        fileNameItem->setFlags(fileNameItem->flags() ^ Qt::ItemIsEditable);
        QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB")
                                             .arg(int((size + 1023) / 1024)));
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        sizeItem->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);

        int row = keysTree->rowCount();
        keysTree->insertRow(row);
        keysTree->setItem(row, 0, fileNameItem);
        keysTree->setItem(row, 1, sizeItem);
    }
    message->setText(tr("%1 key(s) found").arg(keys.size()) +
                             (" (Double click on a key to import it)"));*/
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

void KeyServerImportDialog::importKeyOfItem(int row, int /* column */)
{
    /*QTableWidgetItem *item = keysTree->item(row, 0);
    QDesktopServices::openUrl(QUrl::fromLocalFile(currentDir.absoluteFilePath(item->text())));
    */
}

/*
 * newly added
 */
void KeyServerImportDialog::search()
{
    QString keyserver = keyServerComboBox->currentText();
    QString searchstring = searchLineEdit->text();

    QUrl url = keyserver+":11371/pks/lookup?search="+searchstring+"&op=index&options=mr";
    reply = qnam.get(QNetworkRequest(url));
    qDebug() << "error while download";
    connect(reply, SIGNAL(finished()),
            this, SLOT(searchFinished()));
    qDebug() << "error while download";
}

void KeyServerImportDialog::setMessage(const QString &text, int type)
{

}

void KeyServerImportDialog::searchFinished()
{
    QString firstLine = QString(reply->readLine(1024));

    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (reply->error()) {
        message->setAutoFillBackground(true);
        QPalette filesFoundPalette = message->palette();
        filesFoundPalette.setColor(QPalette::Background, "#20ff20");
        message->setPalette(filesFoundPalette);
        message->setText("Error while contacting keyserver!");
    } else {
        qDebug() << "downloaded";
    }

    if (firstLine.contains("Error"))
    {
        QString text= QString(reply->readLine(1024));

        if (text.contains("Too many responses")) {
            message->setAutoFillBackground(true);
            QPalette filesFoundPalette = message->palette();
            filesFoundPalette.setColor(QPalette::Background, "#20ff20");
            message->setPalette(filesFoundPalette);
            message->setText("Too many responses from keyserver!");
            qDebug() << "Too many responses";
        }
        if (text.contains("No keys found")) {
            message->setAutoFillBackground(true);
            QPalette filesFoundPalette = message->palette();
            filesFoundPalette.setColor(QPalette::Background, "#20ff20");
            message->setPalette(filesFoundPalette);
            message->setText("No keys found containing the search string!");
            qDebug() << "No keys found";
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

//                keysTree->insertTopLevelItem(parentItem);

            } else {
                if (line[0] == "uid") {
                    QStringList l;
                    l << "" << line[1];
                    items.last()->addChild(new QTreeWidgetItem((QTreeWidget*) 0, l));
                }
            }

        }
        qDebug() << items.size();
     keysTree->insertTopLevelItems(0,items);
    }
    reply->deleteLater();
    reply = 0;

    //qDebug() << *text;
    // TODO linebreak OS-unabhängig machen
    //QStringList zeilen = QString( *text ).split('\n');

    //qDebug() << zeilen.count();
    // use this line to get the key, if get request with keyid is sent
    //QString key = zeilen[3];
  /*  int count=-1;
    foreach (QString zeile, zeilen) {
        count++;
        if (zeile.startsWith("pub:",Qt::CaseSensitive)) {
            qDebug() << "pub";
        }
        qDebug() << zeile;
    }*/
}

