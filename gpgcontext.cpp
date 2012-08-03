/*
 *      gpgcontext.cpp
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

#include "gpgcontext.h"
#include "kgpg/gpgproc.h"
#include "kgpg/kgpginterface.h"
#include "kgpg/klinebufferedprocess.h"
#include "kgpg/core/kgpgkey.h"
#include "kgpg/transactions/kgpgencrypt.h"
#ifdef _WIN32
#include <windows.h>
#include <unistd.h>    /* contains read/write */
#endif


namespace GpgME
{

/** Constructor
 *  Set up gpgme-context, set paths to app-run path
 */
GpgContext::GpgContext()
{
    /** get application path */
    QString appPath = qApp->applicationDirPath();

    /** The function `gpgme_check_version' must be called before any other
     *  function in the library, because it initializes the thread support
     *  subsystem in GPGME. (from the info page) */

#ifdef Q_WS_WIN
    gpgBin = appPath + "/bin/gpg.exe";
    gpgKeys = appPath + "/keydb";
#endif
#ifdef Q_WS_MAC
    gpgBin = appPath + "/bin/gpg-mac.app";

    gpgKeys = appPath + "/keydb";

	qDebug() << "gpg bin:" << gpgBin;
	qDebug() << "gpg keydb: " << gpgKeys;
#endif
#ifdef Q_WS_X11
    gpgBin = appPath + "/bin/gpg";
    gpgKeys = appPath + "/keydb";
#endif

    QStringList args;
    args << "--homedir" << gpgKeys << "--list-keys";

/*    QProcess gpg;
    gpg.setProcessChannelMode(QProcess::MergedChannels);
    gpg.start(gpgBin, args);

    gpg.waitForFinished(-1);

    qDebug() << "huhu" << gpg.readAll();
*/
    connect(this,SIGNAL(keyDBChanged()),this,SLOT(refreshKeyList()));
    refreshKeyList();
}

/** Destructor
 *  Release gpgme-context
 */
GpgContext::~GpgContext()
{
    //if (mCtx) gpgme_release(mCtx);
    //mCtx = 0;
}

/** Import Key from QByteArray
 *
 */
GpgImportInformation GpgContext::importKey(QByteArray inBuffer)
{
    GpgImportInformation *importInformation = new GpgImportInformation();
    err = gpgme_data_new_from_mem(&in, inBuffer.data(), inBuffer.size(), 1);
    checkErr(err);
    err = gpgme_op_import(mCtx, in);
    gpgme_import_result_t result;

    result = gpgme_op_import_result(mCtx);
    if (result->unchanged){
        importInformation->unchanged = result->unchanged;
    }
    if (result->considered){
        importInformation->considered = result->considered;
    }
    if (result->no_user_id){
        importInformation->no_user_id = result->no_user_id;
    }
    if (result->imported){
        importInformation->imported = result->imported;
    }
    if (result->imported_rsa){
        importInformation->imported_rsa = result->imported_rsa;
    }
    if (result->unchanged){
        importInformation->unchanged = result->unchanged;
    }
    if (result->new_user_ids){
        importInformation->new_user_ids = result->new_user_ids;
    }
    if (result->new_sub_keys){
        importInformation->new_sub_keys = result->new_sub_keys;
    }
    if (result->new_signatures){
        importInformation->new_signatures = result->new_signatures;
    }
    if (result->new_revocations){
        importInformation->new_revocations  =result->new_revocations;
    }
    if (result->secret_read){
        importInformation->secret_read = result->secret_read;
    }
    if (result->secret_imported){
        importInformation->secret_imported = result->secret_imported;
    }
    if (result->secret_unchanged){
        importInformation->secret_unchanged = result->secret_unchanged;
    }
    if (result->not_imported){
        importInformation->not_imported = result->not_imported;
    }
    gpgme_import_status_t status = result->imports;
    while (status != NULL) {
        GpgImportedKey key;
        key.importStatus = status->status;
        key.fpr = status->fpr;
        importInformation->importedKeys.append(key);
        status=status->next;
    }
    checkErr(err);
    emit keyDBChanged();
    gpgme_data_release(in);
    return *importInformation;
}

/** Generate New Key with values params
 *
 */
void GpgContext::generateKey(QString *params)
{
    err = gpgme_op_genkey(mCtx, params->toAscii().data(), NULL, NULL);
    checkErr(err);
    emit keyDBChanged();
}

/** Export Key to QByteArray
 *
 */
bool GpgContext::exportKeys(QStringList *uidList, QByteArray *outBuffer)
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

KgpgCore::KgpgKey GpgContext::getKeyDetails(QString uid) {

    //KgpgCore::KgpgKey key;
    // try secret
    /*qDebug() << "blubb";
    KgpgCore::KgpgKey key = KgpgInterface::readSecretKeys(QStringList() << uid).first();

    qDebug() << "bla";
    qDebug() << "id: " << key.id();

    // ok, its a public key
    if (key.id() == "") {*/
        KgpgCore::KgpgKey key = KgpgInterface::readPublicKeys(QStringList() << uid).first();
    //}
    return key;

}



/** List all availabe Keys (VERY much like kgpgme)
 */
GpgKeyList GpgContext::listKeys()
{

    KgpgInterface::readPublicKeys();


    GpgKeyList keys;
    KgpgCore::KgpgKeyList kl = KgpgInterface::readPublicKeys();

    foreach(KgpgCore::KgpgKey kkey, kl) {
        GpgKey key;
        key.email = kkey.email();
        //key.expired = kkey.expirationDate().toString();
        key.expired = false;
        key.fpr = kkey.fingerprint();
        key.id = kkey.id();
        key.name = kkey.name();
        key.revoked = false;
        keys.append(key);
    }

    foreach(KgpgCore::KgpgKey skey, KgpgInterface::readSecretKeys()) {
        // iterate keys, mark privates
        GpgKeyList::iterator it = keys.begin();
        while (it != keys.end()) {
            if (skey.id() == it->id) {
                it->privkey = true;
            }
            it++;
        }
    }

    return keys;
}

/** Delete keys
 */

void GpgContext::deleteKeys(QStringList *uidList)
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
bool GpgContext::encrypt(QStringList *uidList, const QByteArray &inBuffer, QByteArray *outBuffer)
{

    /*gpgme_data_t in = 0, out = 0;
    outBuffer->resize(0);

    if (uidList->count() == 0) {
        QMessageBox::critical(0, tr("No Key Selected"), tr("No Key Selected"));
        return false;
    }

    //gpgme_encrypt_result_t e_result;
    gpgme_key_t recipients[uidList->count()+1];

    // get key for user
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
    // unref all keys
    for (int i = 0; i <= uidList->count(); i++) {
        gpgme_key_unref(recipients[i]);
    }
    if (in) {
        gpgme_data_release(in);
    }
    if (out) {
        gpgme_data_release(out);
    }
    return (err == GPG_ERR_NO_ERROR);*/
    QStringList options;
    KGpgEncrypt::EncryptOptions opts = KGpgEncrypt::DefaultEncryption;

    //KGpgEncrypt *encr = new KGpgEncrypt(this, uidList, toPlainText(), opts, options);
    //encr->start();
    //connect(encr, SIGNAL(done(int)), SLOT(slotEncodeUpdate(int)));
}



/** Decrypt QByteAarray, return QByteArray
 *  mainly from http://basket.kde.org/ (kgpgme.cpp)
 */
bool GpgContext::decrypt(const QByteArray &inBuffer, QByteArray *outBuffer)
{
    gpgme_data_t in = 0, out = 0;
    gpgme_decrypt_result_t result = 0;
    QString errorString;

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

                if(gpg_err_code(err) == GPG_ERR_DECRYPT_FAILED) {
                    errorString.append(gpgErrString(err)).append("<br>");
                    result = gpgme_op_decrypt_result(mCtx);
                    checkErr(result->recipients->status);
                    errorString.append(gpgErrString(result->recipients->status)).append("<br>");
                    errorString.append(tr("<br>No private key with id %1 present in keyring").arg(result->recipients->keyid));
                } else {
                    errorString.append(gpgErrString(err)).append("<br>");
                }

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
    if (gpg_err_code(err) != GPG_ERR_NO_ERROR && gpg_err_code(err) != GPG_ERR_CANCELED) {
        QMessageBox::critical(0, tr("Error decrypting:"), errorString);
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
gpgme_error_t GpgContext::readToBuffer(gpgme_data_t in, QByteArray *outBuffer)
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
gpgme_error_t GpgContext::passphraseCb(void *hook, const char *uid_hint,
                                    const char *passphrase_info,
                                    int last_was_bad, int fd)
{
    GpgContext *gpg = static_cast<GpgContext*>(hook);
    return gpg->passphrase(uid_hint, passphrase_info, last_was_bad, fd);
}

gpgme_error_t GpgContext::passphrase(const char *uid_hint,
                                  const char * /*passphrase_info*/,
                                  int last_was_bad, int fd)
{
    gpgme_error_t returnValue = GPG_ERR_CANCELED;
    QString passwordDialogMessage;
    QString gpgHint = QString::fromUtf8(uid_hint);
    bool result;
#ifdef _WIN32
	DWORD written;
	HANDLE hd = (HANDLE)fd;
#endif

    if (last_was_bad) {
        passwordDialogMessage += "<i>"+tr("Wrong password")+".</i><br><br>\n\n";
        clearPasswordCache();
    }

    /** if uid provided */
    if (!gpgHint.isEmpty()) {
        // remove UID, leave only username & email
        gpgHint.remove(0, gpgHint.indexOf(" "));
        passwordDialogMessage += "<b>"+tr("Enter Password for")+"</b><br>" + gpgHint + "<br>";
    }

    if (mPasswordCache.isEmpty()) {
        QString password = QInputDialog::getText(QApplication::activeWindow(), tr("Enter Password"),
                           passwordDialogMessage, QLineEdit::Password,
                           "", &result);

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
        WriteFile(hd, mPasswordCache.data(), mPasswordCache.length(), &written, 0);
#endif

        returnValue = GPG_ERR_NO_ERROR;
    }

#ifndef _WIN32
    if (write(fd, "\n", 1) == -1) {
        qDebug() << "something is terribly broken";
    }
#else
    WriteFile(hd, "\n", 1, &written, 0);
    
    /* program will hang on cancel if hd not closed */
    if(!result) {
			CloseHandle(hd);
	}
#endif

    return returnValue;
}

/** also from kgpgme.cpp, seems to clear password from mem */
void GpgContext::clearPasswordCache()
{
    if (mPasswordCache.size() > 0) {
        mPasswordCache.fill('\0');
        mPasswordCache.truncate(0);
    }
}

// error-handling
int GpgContext::checkErr(gpgme_error_t err, QString comment) const
{
    //if (err != GPG_ERR_NO_ERROR && err != GPG_ERR_CANCELED) {
    if (err != GPG_ERR_NO_ERROR) {
        qDebug() << "[Error " << comment << "] Source: " << gpgme_strsource(err) << " String: " << gpgErrString(err);
    }
    return err;
}

int GpgContext::checkErr(gpgme_error_t err) const
{
    //if (err != GPG_ERR_NO_ERROR && err != GPG_ERR_CANCELED) {
    if (err != GPG_ERR_NO_ERROR) {
        qDebug() << "[Error] Source: " << gpgme_strsource(err) << " String: " << gpgErrString(err);
    }
    return err;
}

QString GpgContext::gpgErrString(gpgme_error_t err) {
    return QString::fromUtf8(gpgme_strerror(err));
}

/** export private key, TODO errohandling, e.g. like in seahorse (seahorse-gpg-op.c) **/

void GpgContext::exportSecretKey(QString uid, QByteArray *outBuffer)
{
    // export private key to outBuffer
    QStringList arguments;
    arguments << "--armor" << "--export-secret-key" << uid;
    QByteArray *err = new QByteArray();
    executeGpgCommand(arguments, outBuffer, err);

    // append public key to outBuffer
    QByteArray *pubKey = new QByteArray();
    QStringList keyList;
    keyList.append(uid);
    exportKeys(&keyList,pubKey);
    outBuffer->append(*pubKey);
}

/** return type should be gpgme_error_t*/
void GpgContext::executeGpgCommand(QStringList arguments, QByteArray *stdOut, QByteArray *stdErr)
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
gpgme_signature_t GpgContext::verify(QByteArray inBuffer) {

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
//void GpgContext::decryptVerify(QByteArray in) {

/*    gpgme_error_t err;
    gpgme_data_t in, out;

    gpgme_decrypt_result_t decrypt_result;
    gpgme_verify_result_t verify_result;

    err = gpgme_op_decrypt_verify (mCtx, in, out);
    decrypt_result = gpgme_op_decrypt_result (mCtx);

    verify_result = gpgme_op_verify_result (mCtx);
 */
//}

bool GpgContext::sign(QStringList *uidList, const QByteArray &inBuffer, QByteArray *outBuffer ) {

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

     err = gpgme_op_sign (mCtx, in, out, GPGME_SIG_MODE_CLEAR);
     checkErr (err);

     if (err == GPG_ERR_CANCELED) {
         return false;
     }

     if (err != GPG_ERR_NO_ERROR) {
         QMessageBox::critical(0, tr("Error signing:"), QString::fromUtf8(gpgme_strerror(err)));
         return false;
     }

     result = gpgme_op_sign_result (mCtx);
     err = readToBuffer(out, outBuffer);
     checkErr (err);

     gpgme_data_release(in);
     gpgme_data_release(out);

     if (! settings.value("general/rememberPassword").toBool()) {
         clearPasswordCache();
     }

     return (err == GPG_ERR_NO_ERROR);
}

/*
 * if there is no '\n' before the PGP-Begin-Block, but for example a whitespace,
 * GPGME doesn't recognise the Message as encrypted. This function adds '\n'
 * before the PGP-Begin-Block, if missing.
 */
void GpgContext::preventNoDataErr(QByteArray *in)
{
    int block_start = in->indexOf(GpgConstants::PGP_CRYPT_BEGIN);
    if (block_start > 0 && in->at(block_start - 1) != '\n') {
        in->insert(block_start, '\n');
    }
    block_start = in->indexOf(GpgConstants::PGP_SIGNED_BEGIN);
    if (block_start > 0 && in->at(block_start - 1) != '\n') {
        in->insert(block_start, '\n');
    }
}

/*
  * isSigned returns:
  * - 0, if text isn't signed at all
  * - 1, if text is partially signed
  * - 2, if text is completly signed
  */
int GpgContext::textIsSigned(const QByteArray &text) {
    if (text.trimmed().startsWith(GpgConstants::PGP_SIGNED_BEGIN) && text.trimmed().endsWith(GpgConstants::PGP_SIGNED_END)) {
        return 2;
    }
    if (text.contains(GpgConstants::PGP_SIGNED_BEGIN) && text.contains(GpgConstants::PGP_SIGNED_END)) {
        return 1;
    }
    return 0;
}

QString GpgContext::beautifyFingerprint(QString fingerprint)
{
    uint len = fingerprint.length();
    if ((len > 0) && (len % 4 == 0))
        for (uint n = 0; 4 *(n + 1) < len; ++n)
            fingerprint.insert(5 * n + 4, ' ');
    return fingerprint;
}

void GpgContext::refreshKeyList() {
    mKeyList = this->listKeys();
}

/**
 * note: privkey status is not returned
 */
GpgKey GpgContext::getKeyByFpr(QString fpr) {

    //GpgKeyList list = this->listKeys();
    foreach  (GpgKey key, mKeyList) {
        if(key.fpr == fpr) {
            return key;
        }
    }

    return GpgKey();
}

/**
 * note: privkey status is not returned
 */
GpgKey GpgContext::getKeyById(QString id) {

    //GpgKeyList list = this->listKeys();
    foreach  (GpgKey key, mKeyList) {
        if(key.id == id) {
            return key;
        }
    }

    return GpgKey();
}

QString GpgContext::getGpgmeVersion() {
     return QString(gpgme_check_version(NULL));
}

}






