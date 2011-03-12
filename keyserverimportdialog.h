/*
 *
 *      keyserverimportdialog.h
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

#ifndef KEYSERVERIMPORTDIALOG_H
#define KEYSERVERIMPORTDIALOG_H

#include <QDialog>
#include <QDir>
#include <QNetworkAccessManager>
#include <QUrl>

QT_BEGIN_NAMESPACE
class QNetworkReply;
class QComboBox;
class QLabel;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;
class QLineEdit;
class QPalette;
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE
struct key {
    QString name;
    int fingerprint;
    QStringList uids;
};

class KeyServerImportDialog : public QDialog
{
    Q_OBJECT

public:
    KeyServerImportDialog(QWidget *parent = 0);

private slots:
    void import();
    void importKeyOfItem(int row, int column);
    void searchFinished();
    void search();

private:
    void showKeys(const QStringList &files);
    QPushButton *createButton(const QString &text, const char *member);
    QComboBox *createComboBox(const QString &text = QString());
    void createKeysTree();
    void setMessage(const QString &text, int type);
    QLineEdit *searchLineEdit;
    QComboBox *keyServerComboBox;
    QLabel *searchLabel;
    QLabel *keyServerLabel;
    QLabel *message;
    QPushButton *closeButton;
    QPushButton *importButton;
    QPushButton *searchButton;
    QTreeWidget *keysTree;
    QDir currentDir;
    QUrl url;
    QNetworkAccessManager qnam;
    QNetworkReply *reply;
    //QString *text;
};
#endif
