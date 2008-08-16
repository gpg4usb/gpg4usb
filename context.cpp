/*
 *      context.cpp
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

#include <locale.h>
#include <sstream>
#include <QApplication>
#include <QtGui>
#include <QMessageBox>
#include <errno.h>
#include "context.h"

#ifdef _WIN32
#include <windows.h>
#include <unistd.h>    /* contains read/write */
#endif


namespace GpgME
{

/** Constructor
 *  Set up gpgme-context, set paths to app-run path
 */
Context::Context()
{

    /** get application path */
    QString appPath = qApp->applicationDirPath();

    /** The function `gpgme_check_version' must be called before any other
     *  function in the library, because it initializes the thread support
     *  subsystem in GPGME. (from the info page) */
    gpgme_check_version(NULL);

    setlocale(LC_ALL, "");
    /** set locale, because tests do also */
    gpgme_set_locale(NULL, LC_CTYPE, setlocale(LC_CTYPE, NULL));
#ifndef _WIN32
    gpgme_set_locale(NULL, LC_MESSAGES, setlocale(LC_MESSAGES, NULL));
#endif

    err = gpgme_new(&m_ctx);
    checkErr(err);
    /** here come the settings, instead of /usr/bin/gpg
     * a executable in the same path as app is used.
     * also lin/win must  be checked, for calling gpg.exe if needed
     */
#ifdef _WIN32
    QString gpgBin = appPath + "/bin/gpg.exe";
#else
    QString gpgBin = appPath + "/bin/gpg";
#endif
    QString gpgKeys = appPath + "/keydb";
    err = gpgme_ctx_set_engine_info(m_ctx, GPGME_PROTOCOL_OpenPGP,
                                    gpgBin.toAscii().constData(),
                                    gpgKeys.toAscii().constData());
    checkErr(err);


    /** Setting the output type must be done at the beginning */
    /** think this means ascii-armor --> ? */
    gpgme_set_armor(m_ctx, 1);
    /** passphrase-callback */
    gpgme_set_passphrase_cb(m_ctx, passphraseCb, this);

    /** check if app is called with -d from command line */
    if (qApp->arguments().contains("-d")) {
        qDebug() << "gpgme_data_t debug on";
        debug = true;
    } else {
        debug = false;
    }

}

/** Destructor
 *  Release gpgme-context
 */
Context::~Context()
{
    if (m_ctx) gpgme_release(m_ctx);
    m_ctx = 0;
}

/** Import Key from QByteArray
 *
 */
void Context::importKey(QByteArray inBuffer)
{
    err = gpgme_data_new_from_mem(&in, inBuffer.data(), inBuffer.size(), 1);
    checkErr(err);
    err = gpgme_op_import(m_ctx, in);
    checkErr(err);
    gpgme_data_release(in);
}

/** List all availabe Keys (VERY much like kgpgme)
 */
GpgKeyList Context::listKeys()
{
    gpgme_error_t err;
    gpgme_key_t key;

    GpgKeyList keys;
    //TODO dont run the loop more often than necessary
    // list all keys ( the 0 is for all )
    err = gpgme_op_keylist_start(m_ctx, NULL, 0);
    checkErr(err);
    while (!(err = gpgme_op_keylist_next(m_ctx, &key))) {
        GpgKey gpgkey;

        if (!key->subkeys)
            continue;

        gpgkey.id = key->subkeys->keyid;

        if (key->uids) {
            gpgkey.name = key->uids->name;
            gpgkey.email = key->uids->email;
        }
        keys.append(gpgkey);
        gpgme_key_unref(key);
    }
    gpgme_op_keylist_end(m_ctx);

    // list only private keys ( the 1 does )
    gpgme_op_keylist_start(m_ctx, NULL, 1);
    while (!(err = gpgme_op_keylist_next(m_ctx, &key))) {
        if (!key->subkeys)
            continue;
        // iterate keys, mark privates
        GpgKeyList::iterator it = keys.begin();
        while (it != keys.end()) {
            if (key->subkeys->keyid == it->id.toStdString())
                it->privkey = 1;
            it++;
        }

        gpgme_key_unref(key);
    }
    gpgme_op_keylist_end(m_ctx);

    return keys;
}

/** Delete keys
 */

void Context::deleteKeys(QList<QString> *uidList)
{

    QString tmp;
    gpgme_key_t key;

    foreach(tmp,  *uidList) {
        gpgme_op_keylist_start(m_ctx, tmp.toAscii().constData(), 0);
        gpgme_op_keylist_next(m_ctx, &key);
        gpgme_op_keylist_end(m_ctx);
        gpgme_op_delete(m_ctx, key, 1);
    }
}

/** Encrypt inBuffer for reciepients-uids, write
 *  result to outBuffer
 */
bool Context::encrypt(QList<QString> *uidList, const QByteArray &inBuffer, QByteArray *outBuffer)
{

    gpgme_data_t in = 0, out = 0;
    outBuffer->resize(0);

    if (uidList->count() == 0) {
        QMessageBox::critical(0, "No Key Selected", "No Key Selected");
        return false;
    }

    //gpgme_encrypt_result_t e_result;
    gpgme_key_t recipients[uidList->count()+1];

    /* get key for user */
    for (int i = 0; i < uidList->count(); i++) {
        // the last 0 is for public keys, 1 would return private keys
        gpgme_op_keylist_start(m_ctx, uidList->at(i).toAscii().constData(), 0);
        gpgme_op_keylist_next(m_ctx, &recipients[i]);
        gpgme_op_keylist_end(m_ctx);
    }
    //Last entry in array has to be NULL
    recipients[uidList->count()] = NULL;

    //If the last parameter isnt 0, a private copy of data is made
    err = gpgme_data_new_from_mem(&in, inBuffer.data(), inBuffer.size(), 1);
    checkErr(err);
    err = gpgme_data_new(&out);
    checkErr(err);

    err = gpgme_op_encrypt(m_ctx, recipients, GPGME_ENCRYPT_ALWAYS_TRUST, in, out);
    checkErr(err);

    err = readToBuffer(out, outBuffer);
    checkErr(err);

    /* unref all keys */
    for (int i = 0; i <= uidList->count(); i++) {
        gpgme_key_unref(recipients[i]);
    }
    gpgme_data_release(in);
    gpgme_data_release(out);

    return true;
}

/** Decrypt QByteAarray, return QByteArray
 *  mainly from http://basket.kde.org/ (kgpgme.cpp)
 */
bool Context::decrypt(const QByteArray &inBuffer, QByteArray *outBuffer)
{
    gpgme_data_t in = 0, out = 0;
    gpgme_decrypt_result_t result = 0;

    outBuffer->resize(0);
    if (m_ctx) {
        err = gpgme_data_new_from_mem(&in, inBuffer.data(), inBuffer.size(), 1);
        checkErr(err);
        if (!err) {
            err = gpgme_data_new(&out);
            checkErr(err);
            if (!err) {
                err = gpgme_op_decrypt(m_ctx, in, out);
                checkErr(err);
                if (!err) {
                    result = gpgme_op_decrypt_result(m_ctx);
                    if (result->unsupported_algorithm) {
                        QMessageBox::critical(0, "Unsupported algorithm", result->unsupported_algorithm);
                    } else {
                        err = readToBuffer(out, outBuffer);
                        checkErr(err);
                    }
                }
            }
        }
    }
    if (err != GPG_ERR_NO_ERROR && err != GPG_ERR_CANCELED) {
        QMessageBox::critical(0, "Error encrypting:", gpgme_strerror(err));
    }
    if (err != GPG_ERR_NO_ERROR)
        clearCache();
    if (in)
        gpgme_data_release(in);
    if (out)
        gpgme_data_release(out);
    return (err == GPG_ERR_NO_ERROR);
}

/**  Read gpgme-Data to QByteArray
 *   mainly from http://basket.kde.org/ (kgpgme.cpp)
 */
#define BUF_SIZE (32 * 1024)
gpgme_error_t Context::readToBuffer(gpgme_data_t in, QByteArray *outBuffer)
{
    int ret;
    gpgme_error_t err = GPG_ERR_NO_ERROR;

    ret = gpgme_data_seek(in, 0, SEEK_SET);
    if (ret) {
        err = gpgme_err_code_from_errno(errno);
        checkErr(err, "failed dataseek in readToBuffer");
    } else {
        char *buf = new char[BUF_SIZE + 2];

        if (buf) {
            while ((ret = gpgme_data_read(in, buf, BUF_SIZE)) > 0) {
                uint size = outBuffer->size();
                outBuffer->resize(size + ret);
                memcpy(outBuffer->data() + size, buf, ret);
            }
            if (ret < 0) {
                err = gpgme_err_code_from_errno(errno);
                checkErr(err, "failed data_read in readToBuffer");
            }
            delete[] buf;
        }
    }
    return err;
}

/** The Passphrase window, if not provided by env-Var GPG_AGENT_INFO
 *  originally copied from http://basket.kde.org/ (kgpgme.cpp), but modified
 */
gpgme_error_t Context::passphraseCb(void *hook, const char *uid_hint,
                                    const char *passphrase_info,
                                    int last_was_bad, int fd)
{
    Context *gpg = static_cast<Context*>(hook);
    return gpg->passphrase(uid_hint, passphrase_info, last_was_bad, fd);
}

gpgme_error_t Context::passphrase(const char *uid_hint,
                                  const char * /*passphrase_info*/,
                                  int last_was_bad, int fd)
{
    gpgme_error_t res = GPG_ERR_CANCELED;
    QString s;
    QString gpg_hint = uid_hint;
    bool result;

    if (last_was_bad) {
        s += "<i>Wrong password.</i><br><br>\n\n";
        clearCache();
    }

    /** if uid provided */
    if (!gpg_hint.isEmpty()) {
        // remove UID, leave only username & email
        gpg_hint.remove(0, gpg_hint.indexOf(" "));
        s += "<b>Enter Password for</b><br>\n" + gpg_hint + "\n";
    }

    if (m_cache.isEmpty()) {
        QString password = QInputDialog::getText(0, "Enter Password",
                           s, QLineEdit::Password,
                           "", &result);

        if (result == 1) m_cache = password.toAscii();
    } else
        result = 0;

    if (result == 1) {
#ifndef _WIN32
        write(fd, m_cache.data(), m_cache.length());
#else
        DWORD written;
        WriteFile((HANDLE) fd, m_cache.data(), m_cache.length(), &written, 0);
#endif
        res = 0;
    }
#ifndef _WIN32
    write(fd, "\n", 1);
#else
    DWORD written;
    WriteFile((HANDLE) fd, "\n", 1, &written, 0);
#endif

    return res;
}

/** also from kgpgme.cpp, seems to clear password from mem */
void Context::clearCache()
{
    if (m_cache.size() > 0) {
        m_cache.fill('\0');
        m_cache.truncate(0);
    }
}

// error-handling
void Context::checkErr(gpgme_error_t err, QString comment) const
{
    if (err != GPG_ERR_NO_ERROR && err != GPG_ERR_CANCELED) {
        qDebug() << "[Error " << comment << "] Source: " << gpgme_strsource(err) << " String: " << gpgme_strerror(err);
    }
}

void Context::checkErr(gpgme_error_t err) const
{
    if (err != GPG_ERR_NO_ERROR && err != GPG_ERR_CANCELED) {
        qDebug() << "[Error] Source: " << gpgme_strsource(err) << " String: " << gpgme_strerror(err);
    }
}

}





