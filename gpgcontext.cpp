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

    connect(this,SIGNAL(keyDBChanged()),this,SLOT(refreshKeyList()));
    refreshKeyList();
}

/** Destructor
 */
GpgContext::~GpgContext()
{

}

/** Import Key from QByteArray
 *
 */
/*GpgImportInformation GpgContext::importKey(QByteArray inBuffer)
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
}*/

KgpgCore::KgpgKey GpgContext::getKeyDetails(QString uid) {


    // try secret
    KgpgCore::KgpgKeyList keys = KgpgInterface::readSecretKeys(QStringList() << uid);
    if(keys.empty()) {
        // ok try public
        keys = KgpgInterface::readPublicKeys(QStringList() << uid);
        // that should not happen
        /*if(keys.empty()) {
            qDebug() << "error, no key with uid" << uid;
            return ;
        }*/

    }

    KgpgCore::KgpgKey key = keys.first();
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

/** also from kgpgme.cpp, seems to clear password from mem */
void GpgContext::clearPasswordCache()
{
    if (mPasswordCache.size() > 0) {
        mPasswordCache.fill('\0');
        mPasswordCache.truncate(0);
    }
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



void GpgContext::emitKeyDBChanged() {
    emit keyDBChanged();
}

}






