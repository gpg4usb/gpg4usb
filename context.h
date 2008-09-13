/*
 *      context.h
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

#ifndef __SGPGMEPP_CONTEXT_H__
#define __SGPGMEPP_CONTEXT_H__

#include <locale.h>
#include <errno.h>
#include <gpgme.h>
#include <QLinkedList>
#include <QtGui>

class QMessageBox;
class sstream;
class QApplication;
class QByteArray;
class QString;

class GpgKey
{
public:
    GpgKey() {
        privkey=false;
    }
    QString id;
    QString name;
    QString email;
    bool privkey;
};

typedef QLinkedList< GpgKey > GpgKeyList;

namespace GpgME
{

class Context
{

public:
    Context(); // Consttructor
    ~Context(); // Destructor

    void importKeyFromFile(QString pathToFile);
    void importKey(QByteArray inBuffer);
    GpgKeyList listKeys();
    void deleteKeys(QList<QString> *uidList);
    bool encrypt(QList<QString> *uidList, const QByteArray &inBuffer,
                 QByteArray *outBuffer);
    bool decrypt(const QByteArray &inBuffer, QByteArray *outBuffer);
    void clearCache();

private:
    gpgme_ctx_t m_ctx;
    gpgme_data_t in, out;
    gpgme_error_t err;
    gpgme_error_t readToBuffer(gpgme_data_t in, QByteArray *outBuffer);
    QByteArray m_cache;
    bool debug;
    void checkErr(gpgme_error_t err) const;
    void checkErr(gpgme_error_t err, QString comment) const;

    static gpgme_error_t passphraseCb(void *hook, const char *uid_hint,
                                      const char *passphrase_info,
                                      int last_was_bad, int fd);
    gpgme_error_t passphrase(const char *uid_hint,
                             const char *passphrase_info,
                             int last_was_bad, int fd);

};
} // namespace GpgME

#endif // __SGPGMEPP_CONTEXT_H__
