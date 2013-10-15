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
class QMessageBox;

namespace GpgME
{

/** Constructor
 *  Set up gpgme-context, set paths to app-run path
 */
GpgContext::GpgContext()
{
    connect(this,SIGNAL(keyDBChanged()),this,SLOT(slotRefreshKeyList()));
    slotRefreshKeyList();
}

/** Destructor
 */
GpgContext::~GpgContext()
{

}

KgpgCore::KgpgKey GpgContext::getKeyDetails(QString uid) {

    KgpgCore::KgpgKeyList keys;
    keys = KgpgInterface::readPublicKeys(QStringList() << uid);
    KgpgCore::KgpgKey key = keys.first();
    return key;

}



/** List all availabe Keys (VERY much like kgpgme)
 */
GpgKeyList GpgContext::listKeys()
{

    GpgKeyList keys;

    foreach(KgpgCore::KgpgKey kkey, KgpgInterface::readPublicKeys()) {
        GpgKey key;
        key.email = kkey.email();
        //key.expired = kkey.expirationDate().toString();
        key.expired = (kkey.expirationDate().date() < QDate::currentDate() &! kkey.expirationDate().isNull());
        key.fpr = kkey.fingerprint();
        key.id = kkey.id();
        key.fullid = kkey.fullId();
        key.name = kkey.name();
        // TODO
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

// TODO: do we really need to call gpg or could we use an already existing list
bool GpgContext::isSecretKey(QString uid) {

    foreach(KgpgCore::KgpgKey skey, KgpgInterface::readSecretKeys()) {
        if(skey.id() == uid) return true;
    }

    return false;

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

void GpgContext::slotRefreshKeyList() {
    mKeyList = this->listKeys();
}

/**
 * note: privkey status is not returned
 */
GpgKey GpgContext::getKeyByFpr(QString fpr) {

    foreach  (GpgKey key, mKeyList) {
        if(key.fpr == fpr) {
            return key;
        }
    }

    return GpgKey();
}

/**
 * note: privkey status is not returned
 * TODO: Long Ids also should be supported
 */
GpgKey GpgContext::getKeyById(QString id) {

    foreach  (GpgKey key, mKeyList) {
        if(key.id == id || key.fullid == id) {
            return key;
        }
    }

    return GpgKey();
}

void GpgContext::emitKeyDBChanged() {
    emit keyDBChanged();
}

// TODO: from kgpgverify, merge back
static QString
sigTimeMessage(const QString &sigtime)
{
    QDateTime stamp;
    if (sigtime.contains(QLatin1Char('T'))) {
        stamp = QDateTime::fromString(sigtime, Qt::ISODate);
    } else {
        bool ok;
        qint64 secs = sigtime.toLongLong(&ok);
        if (ok)
            stamp = QDateTime::fromMSecsSinceEpoch(secs * 1000);
    }

    if (!stamp.isValid())
        return QString();

    return QObject::tr("The signature was created at %1 %2")
            .arg(stamp.date().toString())
            .arg(stamp.time().toString() + QLatin1String("<br/>")
                 , "first argument is formatted date, second argument is formatted time");

}

void GpgContext::exportKeyToFile(const QStringList &keyList)
{
    if (keyList.size() < 1) return;

    QStringList expopts;
    KGpgExport *exp = new KGpgExport(this, keyList, expopts);
    connect(exp, SIGNAL(done(int)), SLOT(slotExportKeyToFileReady(int)));
    exp->start();
}

void GpgContext::slotExportKeyToFileReady(int result)
{
    KGpgExport *exp = qobject_cast<KGpgExport *>(sender());
    Q_ASSERT(exp != NULL);

    if (result == KGpgTransaction::TS_OK) {
        GpgKey key = this->getKeyById(exp->getKeyIds().first());

        QString fileString = key.name + " " + key.email + "(" + key.id+ ")_pub.asc";

        QString fileName = QFileDialog::getSaveFileName(NULL, tr("Export Key To File"), fileString, tr("Key Files") + " (*.asc *.txt);;All Files (*)");
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;
        QTextStream stream(&file);
        QByteArray keyArray = exp->getOutputData();
        qDebug() << *keyArray;
        stream << keyArray;
        file.close();
        //emit statusBarChanged(QString(tr("key(s) exported")));
    } else {
        //KMessageBox::sorry(this, i18n("Your public key could not be exported\nCheck the key."));
        qDebug() << "Your public key could not be exported\nCheck the key.";
    }

    exp->deleteLater();
}

void GpgContext::exportPrivateKey(const QString &keyid)
{
    // Show a information box with explanation about private key
    int ret = QMessageBox::information(NULL, tr("Exporting private Key"),
                                       tr("You are about to export your private key.\n"
                                          "This is NOT your public key, so don't give it away.\n"
                                          "Make sure you keep it save."
                                          "Do you really want to export your private key?"),
                                       QMessageBox::Cancel | QMessageBox::Ok);

    // export key, if ok was clicked
    if (ret == QMessageBox::Ok) {
       /* QByteArray *keyArray = new QByteArray();
        mCtx->exportSecretKey(*keyid, keyArray);
        KgpgCore::KgpgKey key = mCtx->getKeyDetails(*keyid);
        QString fileString = key.name() + " " + key.email() + "(" + key.id()+ ")_pub_sec.asc";
        QString fileName = QFileDialog::getSaveFileName(this, tr("Export Key To File"), fileString, tr("Key Files") + " (*.asc *.txt);;All Files (*)");
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(0,tr("Export error"),tr("Couldn't open %1 for writing").arg(fileName));
            return;
        }
        QTextStream stream(&file);
        stream << *keyArray;
        file.close();
        delete keyArray;*/
        KgpgCore::KgpgKey key = this->getKeyDetails(keyid);
        QString fileString = key.name() + " " + key.email() + "(" + key.id()+ ")_pub_sec.asc";

        QString fileName = QFileDialog::getSaveFileName(NULL, tr("Export Key To File"), fileString, tr("Key Files") + " (*.asc *.txt);;All Files (*)");

        QStringList expopts;
        expopts.append(QLatin1String( "--armor" ));
        KGpgExport *exp = new KGpgExport(this, QStringList() << keyid, fileName, expopts, true);
        connect(exp, SIGNAL(done(int)), SLOT(slotExportPrivateKeyDone(int)));
        exp->start();
    }
}

void GpgContext::slotExportPrivateKeyDone(int result) {
    KGpgExport *exp = qobject_cast<KGpgExport *>(sender());
    Q_ASSERT(exp != NULL);

    if (result == KGpgTransaction::TS_OK) {
        qDebug() << "export seems ok";
    } else {
       qDebug() << "Your key could not be exported\nCheck the key.";
    }

    exp->deleteLater();
}

// import for text based keys
void GpgContext::slotImportKeys(QString text)
{
    KGpgImport *imp =  new KGpgImport(this, text);
    connect(imp, SIGNAL(done(int)), SLOT(slotImportDone(int)));
    imp->start();
}

void GpgContext::slotImportDone(int result)
{
    KGpgImport *import = qobject_cast<KGpgImport *>(sender());

    Q_ASSERT(import != NULL);
    const QStringList rawmsgs(import->getMessages());

    if (result != 0) {
        /*KMessageBox::detailedSorry(this, i18n("Key importing failed. Please see the detailed log for more information."),
                rawmsgs.join( QLatin1String( "\n")) , i18n("Key Import" ));*/
        qDebug() << "Key importing failed. Please see the detailed log for more information." << rawmsgs.join( QLatin1String( "\n"));
    }

    QStringList keys(import->getImportedIds(0x1f));
    const bool needsRefresh = !keys.isEmpty();
    keys << import->getImportedIds(0);
/*
    if (!keys.isEmpty()) {
        const QString msg(import->getImportMessage());
        const QStringList keynames(import->getImportedKeys());

        new KgpgDetailedInfo(this, msg, rawmsgs.join( QLatin1String( "\n") ), keynames, i18n("Key Import" ));
        if (needsRefresh)
            imodel->refreshKeys(keys);
        else
            changeMessage(i18nc("Application ready for user input", "Ready"));
    } else{
        changeMessage(i18nc("Application ready for user input", "Ready"));
    }
*/
    //changeMessage(tr("Application ready for user input", "Ready"));
    emitKeyDBChanged();
    import->deleteLater();
}

void GpgContext::slotimportKeyFromFile()
{
    QString fileName = QFileDialog::getOpenFileName(NULL, tr("Open Key"), "", tr("Key Files") + " (*.asc *.txt);;"+tr("Keyring files")+" (*.gpg);;All Files (*)");
    if (! fileName.isNull()) {

        QList<QUrl> urlList;
        urlList << QUrl::fromLocalFile(fileName);

        KGpgImport *imp =  new KGpgImport(this, urlList);
        connect(imp, SIGNAL(done(int)), SLOT(slotImportDone(int)));
        imp->start();
    }
}

void GpgContext::slotImportKeyFromClipboard()
{
    QClipboard *cb = QApplication::clipboard();
    slotImportKeys(cb->text(QClipboard::Clipboard).toAscii());
}

QString GpgContext::getReport(const QStringList &log)
{
    QString result;
    // newer versions of GnuPG emit both VALIDSIG and GOODSIG
    // for a good signature. Since VALIDSIG has more information
    // we use that.
    const QRegExp validsig(QLatin1String("^\\[GNUPG:\\] VALIDSIG([ ]+[^ ]+){10,}.*$"));
    //const bool useGoodSig = (model != NULL) && (log.indexOf(validsig) == -1);
    const bool useGoodSig = log.indexOf(validsig) == -1;
    QString sigtime;	// timestamp of signature creation

    foreach (const QString &line, log) {
        if (!line.startsWith(QLatin1String("[GNUPG:] ")))
            continue;

        const QString msg = line.mid(9);

        if (msg.startsWith(QLatin1String("VALIDSIG ")) && !useGoodSig) {
            // from GnuPG source, doc/DETAILS:
            //   VALIDSIG    <fingerprint in hex> <sig_creation_date> <sig-timestamp>
            //                <expire-timestamp> <sig-version> <reserved> <pubkey-algo>
            //                <hash-algo> <sig-class> <primary-key-fpr>
            const QStringList vsig = msg.mid(9).split(QLatin1Char(' '), QString::SkipEmptyParts);
            Q_ASSERT(vsig.count() >= 10);

            //result += vsig[9];

            GpgKey key = getKeyByFpr(vsig[9]);

            // ignore for now if this is signed with the primary id (vsig[0] == vsig[9]) or not
            if (key.email.isEmpty()) {
                result += tr("<qt>Good signature from:<br /><b>%1</b><br />Key ID: %2<br /></qt>")
                        .arg(key.name).arg(vsig[9]);
            } else {
                result += tr("Good signature from: NAME <EMAIL>, Key ID: HEXID",
                        "<qt>Good signature from:<br /><b>%1 &lt;%2&gt;</b><br />Key ID: %3<br /></qt>")
                        .arg(key.name).arg(key.email).arg(vsig[9]);
            }
            result += sigTimeMessage(vsig[2]);


/*			const KGpgKeyNode *node = model->findKeyNode(vsig[9]);

            if (node != NULL) {
                // ignore for now if this is signed with the primary id (vsig[0] == vsig[9]) or not
                if (node->getEmail().isEmpty())
                    result += tr("<qt>Good signature from:<br /><b>%1</b><br />Key ID: %2<br /></qt>")
                            .arg(node->getName()).arg(vsig[9]);
                else
                    result += tr("Good signature from: NAME <EMAIL>, Key ID: HEXID",
                            "<qt>Good signature from:<br /><b>%1 &lt;%2&gt;</b><br />Key ID: %3<br /></qt>")
                            .arg(node->getName()).arg(node->getEmail()).arg(vsig[9]);

                result += sigTimeMessage(vsig[2]);
            } else {
                // this should normally never happen, but one could delete
                // the key just after the verification. Brute force solution:
                // do the whole report generation again, but this time make
                // sure GOODSIG is used.
                return getReport(log, NULL);
            }*/
        } else if (msg.startsWith(QLatin1String("UNEXPECTED")) ||
                msg.startsWith(QLatin1String("NODATA"))) {
            result += tr("No signature found.") + QLatin1Char('\n');
        } else if (useGoodSig && msg.startsWith(QLatin1String("GOODSIG "))) {
            int sigpos = msg.indexOf( ' ' , 8);
            const QString keyid = msg.mid(8, sigpos - 8);

            // split the name/email pair to give translators more power to handle this
            QString email;
            QString name = msg.mid(sigpos + 1);

            int oPos = name.indexOf(QLatin1Char('<'));
            int cPos = name.indexOf(QLatin1Char('>'));
            if ((oPos >= 0) && (cPos >= 0)) {
                email = name.mid(oPos + 1, cPos - oPos - 1);
                name = name.left(oPos).simplified();
            }

            if (email.isEmpty())
                result += tr("<qt>Good signature from:<br /><b>%1</b><br />Key ID: %2<br /></qt>")
                        .arg(name).arg(keyid);
            else
                result += tr("Good signature from: NAME <EMAIL>, Key ID: HEXID",
                             "<qt>Good signature from:<br /><b>%1 &lt;%2&gt;</b><br />Key ID: %3<br /></qt>")
                        .arg(name).arg(email).arg(keyid);
            if (!sigtime.isEmpty()) {
                result += sigTimeMessage(sigtime);
                sigtime.clear();
            }
        } else if (msg.startsWith(QLatin1String("SIG_ID "))) {
            const QStringList parts = msg.simplified().split(QLatin1Char(' '));
            if (parts.count() > 2)
                sigtime = parts[2];
        } else if (msg.startsWith(QLatin1String("BADSIG"))) {
            int sigpos = msg.indexOf( ' ', 7);
            result += tr("<qt><b>BAD signature</b> from:<br /> %1<br />Key id: %2<br /><br /><b>The file is corrupted</b><br /></qt>")
                    .arg(msg.mid(sigpos + 1).replace(QLatin1Char('<'), QLatin1String("&lt;")))
                    .arg(msg.mid(7, sigpos - 7));
        } else  if (msg.startsWith(QLatin1String("TRUST_UNDEFINED"))) {
            result += tr("<qt>The signature is valid, but the key is untrusted<br /></qt>");
        } else if (msg.startsWith(QLatin1String("TRUST_ULTIMATE"))) {
            result += tr("<qt>The signature is valid, and the key is ultimately trusted<br /></qt>");
        }
    }

    return result;
}

}






