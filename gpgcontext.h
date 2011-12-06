/*
 *      context.h
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
#include <gpgme.h>
#include <QLinkedList>
#include <QtGui>

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

namespace GpgME
{

class GpgContext : public QObject
{
    Q_OBJECT

public:
    GpgContext(); // Constructor
    ~GpgContext(); // Destructor
    gpgme_import_result_t importKey(QByteArray inBuffer);
    bool exportKeys(QStringList *uidList, QByteArray *outBuffer);
    void generateKey(QString *params);
    GpgKeyList listKeys();
    void deleteKeys(QStringList *uidList);
    bool encrypt(QStringList *uidList, const QByteArray &inBuffer,
                 QByteArray *outBuffer);
    bool decrypt(const QByteArray &inBuffer, QByteArray *outBuffer);
    void clearPasswordCache();
    void exportSecretKey(QString uid, QByteArray *outBuffer);
    gpgme_key_t getKeyDetails(QString uid);
    gpgme_signature_t verify(QByteArray in);
//    void decryptVerify(QByteArray in);
    bool sign(QStringList *uidList, const QByteArray &inBuffer, QByteArray *outBuffer );
    /**
     * @details If text contains PGP-message, put a linebreak before the message,
     * so that gpgme can decrypt correctly
     *
     * @param in Pointer to the QBytearray to check.
     */
    void preventNoDataErr(QByteArray *in);

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
    void sendKeyDBChanged();

signals:
    void keyDBChanged();

private:
    gpgme_ctx_t mCtx;
    gpgme_data_t in, out;
    gpgme_error_t err;
    gpgme_error_t readToBuffer(gpgme_data_t in, QByteArray *outBuffer);
    QByteArray mPasswordCache;
    QSettings settings;
    bool debug;
    int checkErr(gpgme_error_t err) const;
    int checkErr(gpgme_error_t err, QString comment) const;

    static gpgme_error_t passphraseCb(void *hook, const char *uid_hint,
                                      const char *passphrase_info,
                                      int last_was_bad, int fd);
    gpgme_error_t passphrase(const char *uid_hint,
                             const char *passphrase_info,
                             int last_was_bad, int fd);

    void executeGpgCommand(QStringList arguments,
                           QByteArray *stdOut,
                           QByteArray *stdErr);

};
} // namespace GpgME

#endif // __SGPGMEPP_CONTEXT_H__
