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

    // TODO: Set gpgme_language to config
    // http://lavica.fesb.hr/cgi-bin/info2html?%28gpgme%29Locale
    setlocale(LC_ALL, "");
    /** set locale, because tests do also */
    gpgme_set_locale(NULL, LC_CTYPE, setlocale(LC_CTYPE, NULL));
    //qDebug() << "Locale set to" << LC_CTYPE << " - " << setlocale(LC_CTYPE, NULL);
#ifndef _WIN32
    gpgme_set_locale(NULL, LC_MESSAGES, setlocale(LC_MESSAGES, NULL));
#endif

    err = gpgme_new(&mCtx);
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
    /*    err = gpgme_ctx_set_engine_info(mCtx, GPGME_PROTOCOL_OpenPGP,
                                        gpgBin.toUtf8().constData(),
                                        gpgKeys.toUtf8().constData());*/
    err = gpgme_ctx_set_engine_info(mCtx, GPGME_PROTOCOL_OpenPGP,
                                    gpgBin.toLocal8Bit().constData(),
                                    gpgKeys.toLocal8Bit().constData());
    checkErr(err);


    /** Setting the output type must be done at the beginning */
    /** think this means ascii-armor --> ? */
    gpgme_set_armor(mCtx, 1);
    /** passphrase-callback */
    gpgme_set_passphrase_cb(mCtx, passphraseCb, this);

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
    if (mCtx) gpgme_release(mCtx);
    mCtx = 0;
}

/** Import Key from QByteArray
 *
 */
void Context::importKey(QByteArray inBuffer)
{
    err = gpgme_data_new_from_mem(&in, inBuffer.data(), inBuffer.size(), 1);
    checkErr(err);
    err = gpgme_op_import(mCtx, in);
    checkErr(err);
    gpgme_data_release(in);
    emit keyDBChanged();
}

/** Generate New Key with values params
 *
 */
void Context::generateKey(QString *params)
{
    err = gpgme_op_genkey(mCtx, params->toAscii().data(), NULL, NULL);
    checkErr(err);
    emit keyDBChanged();
}

/** Export Key to QByteArray
 *
 */
bool Context::exportKeys(QStringList *uidList, QByteArray *outBuffer)
{
    size_t read_bytes;
    gpgme_data_t out = 0;
    outBuffer->resize(0);

    if (uidList->count() == 0) {
        QMessageBox::critical(0, "Export Keys Error", "No Keys Selected");
        return false;
    }

    for (int i = 0; i < uidList->count(); i++) {
        err = gpgme_data_new(&out);
        checkErr(err);

        err = gpgme_op_export(mCtx, uidList->at(i).toAscii().constData(), 0, out);
        checkErr(err);

        read_bytes = gpgme_data_seek(out, 0, SEEK_END);

        err = readToBuffer(out, outBuffer);
        checkErr(err);
        gpgme_data_release(out);
    }
    return true;
}

gpgme_key_t Context::getKeyDetails(QString uid)
{
    gpgme_key_t key;

    // try secret
    gpgme_get_key(mCtx, uid.toAscii().constData(), &key, 1);
    // ok, its a public key
    if (!key) {
        gpgme_get_key(mCtx, uid.toAscii().constData(), &key, 0);
    }
    return key;
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
    err = gpgme_op_keylist_start(mCtx, NULL, 0);
    checkErr(err);
    while (!(err = gpgme_op_keylist_next(mCtx, &key))) {
        GpgKey gpgkey;

        if (!key->subkeys)
            continue;

        gpgkey.id = key->subkeys->keyid;

        if (key->uids) {
            gpgkey.name = key->uids->name;
            gpgkey.email = key->uids->email;
            gpgkey.fpr = key->subkeys->fpr;
        }
        keys.append(gpgkey);
        gpgme_key_unref(key);
    }
    gpgme_op_keylist_end(mCtx);

    // list only private keys ( the 1 does )
    gpgme_op_keylist_start(mCtx, NULL, 1);
    while (!(err = gpgme_op_keylist_next(mCtx, &key))) {
        if (!key->subkeys)
            continue;
        // iterate keys, mark privates
        GpgKeyList::iterator it = keys.begin();
        while (it != keys.end()) {
            if (key->subkeys->keyid == it->id.toStdString())
                it->privkey = true;
            it++;
        }

        gpgme_key_unref(key);
    }
    gpgme_op_keylist_end(mCtx);

    return keys;
}

/** Delete keys
 */

void Context::deleteKeys(QStringList *uidList)
{
    QString tmp;
    gpgme_key_t key;

    foreach(tmp,  *uidList) {
        gpgme_op_keylist_start(mCtx, tmp.toAscii().constData(), 0);
        gpgme_op_keylist_next(mCtx, &key);
        gpgme_op_keylist_end(mCtx);
        gpgme_op_delete(mCtx, key, 1);
    }
    emit keyDBChanged();
}

/** Encrypt inBuffer for reciepients-uids, write
 *  result to outBuffer
 */
bool Context::encrypt(QStringList *uidList, const QByteArray &inBuffer, QByteArray *outBuffer)
{

    gpgme_data_t in = 0, out = 0;
    outBuffer->resize(0);

    if (uidList->count() == 0) {
        QMessageBox::critical(0, tr("No Key Selected"), tr("No Key Selected"));
        return false;
    }

    //gpgme_encrypt_result_t e_result;
    gpgme_key_t recipients[uidList->count()+1];

    /* get key for user */
    for (int i = 0; i < uidList->count(); i++) {
        // the last 0 is for public keys, 1 would return private keys
        gpgme_op_keylist_start(mCtx, uidList->at(i).toAscii().constData(), 0);
        gpgme_op_keylist_next(mCtx, &recipients[i]);
        gpgme_op_keylist_end(mCtx);
    }
    //Last entry in array has to be NULL
    recipients[uidList->count()] = NULL;

    //If the last parameter isnt 0, a private copy of data is made
    if (mCtx) {
		err = gpgme_data_new_from_mem(&in, inBuffer.data(), inBuffer.size(), 1);
		checkErr(err);
        if (!err) {
			err = gpgme_data_new(&out);
			checkErr(err);
	        if (!err) {
				err = gpgme_op_encrypt(mCtx, recipients, GPGME_ENCRYPT_ALWAYS_TRUST, in, out);
				checkErr(err);
				if (!err) {
					err = readToBuffer(out, outBuffer);
					checkErr(err);
				}
			}
		}
	}
    /* unref all keys */
    for (int i = 0; i <= uidList->count(); i++) {
        gpgme_key_unref(recipients[i]);
    }
    if (in) {
        gpgme_data_release(in);
    }
    if (out) {
        gpgme_data_release(out);
    }
    return (err == GPG_ERR_NO_ERROR);
}

/** Decrypt QByteAarray, return QByteArray
 *  mainly from http://basket.kde.org/ (kgpgme.cpp)
 */
bool Context::decrypt(const QByteArray &inBuffer, QByteArray *outBuffer)
{
    gpgme_data_t in = 0, out = 0;
    gpgme_decrypt_result_t result = 0;

    outBuffer->resize(0);
    if (mCtx) {
        err = gpgme_data_new_from_mem(&in, inBuffer.data(), inBuffer.size(), 1);
        checkErr(err);
        if (!err) {
            err = gpgme_data_new(&out);
            checkErr(err);
            if (!err) {
                err = gpgme_op_decrypt(mCtx, in, out);
                checkErr(err);
                if (!err) {
                    result = gpgme_op_decrypt_result(mCtx);
                    if (result->unsupported_algorithm) {
                        QMessageBox::critical(0, tr("Unsupported algorithm"), result->unsupported_algorithm);
                    } else {
                        err = readToBuffer(out, outBuffer);
                        checkErr(err);
                    }
                }
            }
        }
    }
    if (err != GPG_ERR_NO_ERROR && err != GPG_ERR_CANCELED) {
        QMessageBox::critical(0, tr("Error decrypting:"), gpgme_strerror(err));
        return false;
    }

    if (! settings.value("general/rememberPassword").toBool()) {
        clearPasswordCache();
    }

    if (in) {
        gpgme_data_release(in);
    }
    if (out) {
        gpgme_data_release(out);
    }
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
    gpgme_error_t returnValue = GPG_ERR_CANCELED;
    QString passwordDialogMessage;
    QString gpgHint = uid_hint;
    bool result;

    if (last_was_bad) {
        passwordDialogMessage += "<i>"+tr("Wrong password")+".</i><br><br>\n\n";
        clearPasswordCache();
    }

    /** if uid provided */
    if (!gpgHint.isEmpty()) {
        // remove UID, leave only username & email
        gpgHint.remove(0, gpgHint.indexOf(" "));
        passwordDialogMessage += "<b>Enter Password for</b><br>\n" + gpgHint + "\n";
    }

    if (mPasswordCache.isEmpty()) {
        QString password = QInputDialog::getText(0, tr("Enter Password"),
                           passwordDialogMessage, QLineEdit::Password,
                           "", &result, Qt::Window);

        if (result) mPasswordCache = password.toAscii();
    } else {
        result = true;
    }

    if (result) {

#ifndef _WIN32
        if (write(fd, mPasswordCache.data(), mPasswordCache.length()) == -1) {
            qDebug() << "something is terribly broken";
        }
#else
        DWORD written;
        WriteFile((HANDLE) fd, mPasswordCache.data(), mPasswordCache.length(), &written, 0);
#endif

        returnValue = 0;
    }

#ifndef _WIN32
    if (write(fd, "\n", 1) == -1) {
        qDebug() << "something is terribly broken";
    }
#else
    DWORD written;
    WriteFile((HANDLE) fd, "\n", 1, &written, 0);
#endif

    return returnValue;
}

/** also from kgpgme.cpp, seems to clear password from mem */
void Context::clearPasswordCache()
{
    if (mPasswordCache.size() > 0) {
        mPasswordCache.fill('\0');
        mPasswordCache.truncate(0);
    }
}

// error-handling
int Context::checkErr(gpgme_error_t err, QString comment) const
{
    //if (err != GPG_ERR_NO_ERROR && err != GPG_ERR_CANCELED) {
    if (err != GPG_ERR_NO_ERROR) {
        qDebug() << "[Error " << comment << "] Source: " << gpgme_strsource(err) << " String: " << gpgme_strerror(err);
    }
    return err;
}

int Context::checkErr(gpgme_error_t err) const
{
    //if (err != GPG_ERR_NO_ERROR && err != GPG_ERR_CANCELED) {
    if (err != GPG_ERR_NO_ERROR) {
        qDebug() << "[Error] Source: " << gpgme_strsource(err) << " String: " << gpgme_strerror(err);
    }
    return err;
}


/** export private key, TODO errohandling, e.g. like in seahorse (seahorse-gpg-op.c) **/

void Context::exportSecretKey(QString uid, QByteArray *outBuffer)
{
    QStringList arguments;
    arguments << "--armor" << "--export-secret-key" << uid;
    QByteArray *err = new QByteArray();
    executeGpgCommand(arguments, outBuffer, err);
}

/** return type should be gpgme_error_t*/
void Context::executeGpgCommand(QStringList arguments, QByteArray *stdOut, QByteArray *stdErr)
{
    gpgme_engine_info_t engine = gpgme_ctx_get_engine_info(mCtx);

    QStringList args;
    args << "--homedir" << engine->home_dir << "--batch" << arguments;

    QProcess gpg;
    gpg.start(engine->file_name, args);
    gpg.waitForFinished();

    *stdOut = gpg.readAllStandardOutput();
    *stdErr = gpg.readAllStandardError();
}

/***
  * TODO: return type should contain:
  * -> list of sigs
  * -> valid
  * -> errors
  */
gpgme_signature_t Context::verify(QByteArray inBuffer) {

    int error=0;
    gpgme_data_t in;
    gpgme_error_t err;
    gpgme_signature_t sign;
    gpgme_verify_result_t result;

    err = gpgme_data_new_from_mem(&in, inBuffer.data(), inBuffer.size(), 1);
    checkErr(err);

    err = gpgme_op_verify (mCtx, in, NULL, in);
    error = checkErr(err);

    if (error != 0) {
        return NULL;
    }

    result = gpgme_op_verify_result (mCtx);
    sign = result->signatures;
    return sign;
}

/***
  * return type should contain:
  * -> list of sigs
  * -> valid
  * -> decrypted message
  */
//void Context::decryptVerify(QByteArray in) {

/*    gpgme_error_t err;
    gpgme_data_t in, out;

    gpgme_decrypt_result_t decrypt_result;
    gpgme_verify_result_t verify_result;

    err = gpgme_op_decrypt_verify (mCtx, in, out);
    decrypt_result = gpgme_op_decrypt_result (mCtx);

    verify_result = gpgme_op_verify_result (mCtx);
 */
//}

void Context::sign(const QByteArray &inBuffer, QByteArray *outBuffer) {

    gpgme_error_t err;
    gpgme_data_t in=NULL, out=NULL;
    gpgme_sign_result_t result;

/*
    `GPGME_SIG_MODE_NORMAL'
          A normal signature is made, the output includes the plaintext
          and the signature.

    `GPGME_SIG_MODE_DETACH'
          A detached signature is made.

    `GPGME_SIG_MODE_CLEAR'
          A clear text signature is made.  The ASCII armor and text
          mode settings of the context are ignored.
*/

    //err = gpgme_op_sign (mCtx, in, out, GPGME_SIG_MODE_NORMAL);
    err = gpgme_op_sign (mCtx, in, out, GPGME_SIG_MODE_CLEAR);
    checkErr(err);
    result = gpgme_op_sign_result (mCtx);

    err = readToBuffer(out, outBuffer);
}

bool Context::sign(QStringList *uidList, const QByteArray &inBuffer, QByteArray *outBuffer ) {

    gpgme_error_t err;
    gpgme_data_t in, out;
    gpgme_sign_result_t result;

    if (uidList->count() == 0) {
        QMessageBox::critical(0, tr("Key Selection"), tr("No Private Key Selected"));
        return false;
    }

    // at start or end?
    gpgme_signers_clear(mCtx);

    //gpgme_encrypt_result_t e_result;
    gpgme_key_t signers[uidList->count()+1];


    // TODO: do we really need array? adding one key in loop should be ok
    for (int i = 0; i < uidList->count(); i++) {
        // the last 0 is for public keys, 1 would return private keys
        gpgme_op_keylist_start(mCtx, uidList->at(i).toAscii().constData(), 0);
        gpgme_op_keylist_next(mCtx, &signers[i]);
        gpgme_op_keylist_end(mCtx);

        err = gpgme_signers_add (mCtx, signers[i]);
        checkErr(err);
    }

     err = gpgme_data_new_from_mem(&in, inBuffer.data(), inBuffer.size(), 1);
     checkErr(err);
     err = gpgme_data_new (&out);
     checkErr(err);

     err = gpgme_op_sign (mCtx, in, out, GPGME_SIG_MODE_CLEAR);
     checkErr (err);
     result = gpgme_op_sign_result (mCtx);
     err = readToBuffer(out, outBuffer);

     gpgme_data_release(in);
     gpgme_data_release(out);

     return (err == GPG_ERR_NO_ERROR);
}

/*
 * if there is no '\n' before the PGP-Begin-Block, but for example a whitespace,
 * GPGME doesn't recognise the Message as encrypted. This function adds '\n'
 * before the PGP-Begin-Block, if missing.
 */
void Context::preventNoDataErr(QByteArray *in)
{
    int block_start = in->indexOf("-----BEGIN PGP MESSAGE-----");
    if (block_start > 0 && in->at(block_start - 1) != '\n') {
        in->insert(block_start, '\n');
    }
    block_start = in->indexOf("-----BEGIN PGP SIGNED MESSAGE-----");
    if (block_start > 0 && in->at(block_start - 1) != '\n') {
        in->insert(block_start, '\n');
    }
}

}





