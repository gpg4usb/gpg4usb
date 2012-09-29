/*
 *      gpgcontext.h
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

#ifndef __SGPGMEPP_CONTEXT_H__
#define __SGPGMEPP_CONTEXT_H__

#include "gpgconstants.h"
#include <locale.h>
#include <errno.h>
#include <QLinkedList>
#include <QtGui>
#include "kgpg/core/kgpgkey.h"

QT_BEGIN_NAMESPACE
class QMessageBox;
class sstream;
class QApplication;
class QByteArray;
class QString;
QT_END_NAMESPACE

class GpgKey
{
public:
    GpgKey() {
        privkey = false;
    }
    QString id;
    QString name;
    QString email;
    QString fpr;
    bool privkey;
    bool expired;
    bool revoked;
};

typedef QLinkedList< GpgKey > GpgKeyList;

class GpgImportedKey
{
public:
    QString fpr;
    int importStatus;
};

typedef QLinkedList< GpgImportedKey > GpgImportedKeyList;

class GpgImportInformation
{
public:
    GpgImportInformation() {
        considered = 0;
        no_user_id = 0;
        imported = 0;
        imported_rsa = 0;
        unchanged = 0;
        new_user_ids = 0;
        new_sub_keys = 0;
        new_signatures = 0;
        new_revocations = 0;
        secret_read = 0;
        secret_imported = 0;
        secret_unchanged = 0;
        not_imported = 0;
    }

    int considered;
    int no_user_id;
    int imported;
    int imported_rsa;
    int unchanged;
    int new_user_ids;
    int new_sub_keys;
    int new_signatures;
    int new_revocations;
    int secret_read;
    int secret_imported;
    int secret_unchanged;
    int not_imported;
    GpgImportedKeyList importedKeys;
};

namespace GpgME
{

class GpgContext : public QObject
{
    Q_OBJECT

public:
    GpgContext(); // Constructor
    ~GpgContext(); // Destructor
    GpgKeyList listKeys();
    void clearPasswordCache();
    KgpgCore::KgpgKey getKeyDetails(QString uid);

    /**
     * @details If text contains PGP-message, put a linebreak before the message,
     * so that gpgme can decrypt correctly
     *
     * @param in Pointer to the QBytearray to check.
     */
    void preventNoDataErr(QByteArray *in);

    GpgKey getKeyByFpr(QString fpr);
    GpgKey getKeyById(QString id);

    void emitKeyDBChanged();

    /**
     * @brief
     *
     * @param text
     * @return \li 2, if the text is completly signed,
     *          \li 1, if the text is partially signed,
     *          \li 0, if the text is not signed at all.
     */
    int textIsSigned(const QByteArray &text);
    QString beautifyFingerprint(QString fingerprint);

signals:
    void keyDBChanged();

private slots:
    void refreshKeyList();

private:
    QByteArray mPasswordCache;
    QSettings settings;
    bool debug;
    GpgKeyList mKeyList;
    QString gpgBin;
    QString gpgKeys;

};
} // namespace GpgME

#endif // __SGPGMEPP_CONTEXT_H__
