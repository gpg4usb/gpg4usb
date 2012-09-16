/*
 * Copyright (C) 2002 Jean-Baptiste Mardelle <bj@altern.org>
 * Copyright (C) 2007,2008,2009,2010,2011,2012
 *               Rolf Eike Beer <kde@opensource.sf-tec.de>
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kgpginterface.h"

#include "gpgproc.h"
#include "core/convert.h"
#include "core/KGpgKeyNode.h"
#include "core/KGpgSignNode.h"
#include "core/KGpgSubkeyNode.h"
#include "core/KGpgUatNode.h"
#include "core/KGpgUidNode.h"

/*#include <KConfig>
#include <KDebug>
#include <KGlobal>
#include <KLocale>
#include <KMessageBox>
#include <KPasswordDialog>
#include <KProcess>
#include <KPushButton>*/
#include <QFile>
#include <QPointer>
#include <QString>
#include <QTextStream>
#include <QDebug>
#include <QInputDialog>

using namespace KgpgCore;

QString KgpgInterface::getGpgSetting(const QString &name, const QString &configfile)
{
	const QString tmp(name.simplified() + QLatin1Char( ' ' ));
	QFile qfile(configfile);

	if (qfile.open(QIODevice::ReadOnly) && (qfile.exists())) {
		QTextStream t(&qfile);
		while (!t.atEnd()) {
			QString result(t.readLine().simplified());
			if (result.startsWith(tmp)) {
				result = result.mid(tmp.length()).simplified();
				return result.section(QLatin1Char( ' ' ), 0, 0);
			}
		}
		qfile.close();
	}

	return QString();
}

void KgpgInterface::setGpgSetting(const QString &name, const QString &value, const QString &url)
{
	QFile qfile(url);

	if (qfile.open(QIODevice::ReadOnly) && (qfile.exists())) {
		const QString temp(name + QLatin1Char( ' ' ));
		QString texttowrite;
		bool found = false;
		QTextStream t(&qfile);

		while (!t.atEnd()) {
			QString result = t.readLine();
			if (result.simplified().startsWith(temp)) {
				if (!value.isEmpty())
					result = temp + QLatin1Char( ' ' ) + value;
				else
					result.clear();
				found = true;
			}

			texttowrite += result + QLatin1Char( '\n' );
		}

		qfile.close();
		if ((!found) && (!value.isEmpty()))
			texttowrite += QLatin1Char( '\n' ) + temp + QLatin1Char( ' ' ) + value;

		if (qfile.open(QIODevice::WriteOnly)) {
			QTextStream t(&qfile);
			t << texttowrite;
			qfile.close();
		}
	}
}

bool KgpgInterface::getGpgBoolSetting(const QString &name, const QString &configfile)
{
	QFile qfile(configfile);
	if (qfile.open(QIODevice::ReadOnly) && (qfile.exists())) {
		QTextStream t(&qfile);
		while (!t.atEnd()) {
			if (t.readLine().simplified().startsWith(name))
				return true;
		}
		qfile.close();
	}
	return false;
}

void KgpgInterface::setGpgBoolSetting(const QString &name, const bool enable, const QString &url)
{
	QFile qfile(url);

	if (qfile.open(QIODevice::ReadOnly) && (qfile.exists())) {
		QString texttowrite;
		bool found = false;
		QTextStream t(&qfile);

		while (!t.atEnd()) {
			QString result(t.readLine());

			if (result.simplified().startsWith(name)) {
				if (enable)
					result = name;
				else
					result.clear();

				found = true;
			}

			texttowrite += result + QLatin1Char( '\n' );
		}
		qfile.close();

		if ((!found) && (enable))
			texttowrite += name;

		if (qfile.open(QIODevice::WriteOnly)) {
			QTextStream t(&qfile);
			t << texttowrite;
			qfile.close();
		}
	}
}

int KgpgInterface::sendPassphrase(const QString &text, KProcess *process, QWidget *widget)
{
    qDebug() << "KgpgInterface::sendPassphrase called";

	QPointer<KProcess> gpgprocess = process;
	QByteArray passphrase;
    //int code;
    bool ok;

    /*QPointer<KPasswordDialog> dlg = new KPasswordDialog(widget);
	QObject::connect(process, SIGNAL(processExited()), dlg->button(KDialog::Cancel), SLOT(click()));
	dlg->setPrompt(text);
    code = dlg->exec();

	if (!dlg.isNull())
		passphrase = dlg->password().toUtf8();
	delete dlg;

	if (code != KPasswordDialog::Accepted)
		return 1;
*/

    QString password = QInputDialog::getText(QApplication::activeWindow(), QObject::tr("Enter Password"),
                       text, QLineEdit::Password,
                       "", &ok);

    if(!ok) return 1;

    passphrase = password.toAscii();

    if (!gpgprocess.isNull()) {
		gpgprocess->write(passphrase + '\n');
    }

	return 0;
}

/**
 * @param p the process that reads the GnuPG data
 * @param readNode the node where the signatures are read for
 */
static KgpgCore::KgpgKeyList
readPublicKeysProcess(GPGProc &p, KGpgKeyNode *readNode)
{
	QStringList lsp;
	int items;
	KgpgCore::KgpgKeyList publiclistkeys;
	KgpgCore::KgpgKey *publickey = NULL;
	unsigned int idIndex = 0;
	QString log;
	KGpgSignableNode *currentSNode = NULL;	///< the current (sub)node signatures are read for

	while ((items = p.readln(lsp)) >= 0) {
		if ((lsp.at(0) == QLatin1String( "pub" )) && (items >= 10)) {
			publiclistkeys << KgpgKey(lsp.at(4), lsp.at(2).toUInt(), Convert::toTrust(lsp.at(1)),
					Convert::toAlgo(lsp.at(3).toInt()), QDateTime::fromTime_t(lsp.at(5).toUInt()));

			publickey = &publiclistkeys.last();

			publickey->setOwnerTrust(Convert::toOwnerTrust(lsp.at(8)));

			if (lsp.at(6).isEmpty())
				publickey->setExpiration(QDateTime());
			else
				publickey->setExpiration(QDateTime::fromTime_t(lsp.at(6).toUInt()));

			publickey->setValid((items <= 11) || !lsp.at(11).contains(QLatin1Char( 'D' ), Qt::CaseSensitive));  // disabled key

			idIndex = 0;
		} else if ((lsp.at(0) == QLatin1String( "fpr" )) && (items >= 10)) {
			const QString fingervalue(lsp.at(9));

			publickey->setFingerprint(fingervalue);
		} else if ((lsp.at(0) == QLatin1String( "sub" )) && (items >= 7)) {
			KgpgSubKeyType subtype;

			if (items > 11) {
				if (lsp.at(11).contains(QLatin1Char( 's' )))
					subtype |= SKT_SIGNATURE;
				if (lsp.at(11).contains(QLatin1Char( 'e' )))
					subtype |= SKT_ENCRYPTION;
				if (lsp.at(11).contains(QLatin1Char( 'e' )))
					subtype |= SKT_AUTHENTICATION;
				if (lsp.at(11).contains(QLatin1Char( 'e' )))
					subtype |= SKT_CERTIFICATION;
			}

			KgpgKeySub sub(lsp.at(4), lsp.at(2).toUInt(), Convert::toTrust(lsp.at(1)),
					Convert::toAlgo(lsp.at(3).toInt()), subtype, QDateTime::fromTime_t(lsp.at(5).toUInt()));

			// FIXME: Please see kgpgkey.h, KgpgSubKey class
			if (items <= 11)
				sub.setValid(true);
			else
				sub.setValid(!lsp.at(11).contains(QLatin1Char( 'D' )));

			if (lsp.at(6).isEmpty())
				sub.setExpiration(QDateTime());
			else
				sub.setExpiration(QDateTime::fromTime_t(lsp.at(6).toUInt()));

			publickey->subList()->append(sub);
			if (readNode == NULL)
				currentSNode = NULL;
			else
				currentSNode = new KGpgSubkeyNode(readNode, sub);
		} else if (lsp.at(0) == QLatin1String( "uat" )) {
			idIndex++;
			if (readNode != NULL) {
				currentSNode = new KGpgUatNode(readNode, idIndex, lsp);
			}
		} else if ((lsp.at(0) == QLatin1String( "uid" )) && (items >= 10)) {
			if (idIndex == 0) {
				QString fullname(lsp.at(9));
				QString kmail;
				if (fullname.contains(QLatin1Char( '<' )) ) {
					kmail = fullname;

					if (fullname.contains(QLatin1Char( ')' )) )
						kmail = kmail.section(QLatin1Char( ')' ), 1);

					kmail = kmail.section(QLatin1Char( '<' ), 1);
					kmail.truncate(kmail.length() - 1);

					if (kmail.contains(QLatin1Char( '<' ))) {
						// several email addresses in the same key
						kmail = kmail.replace(QLatin1Char( '>' ), QLatin1Char( ';' ));
						kmail.remove(QLatin1Char( '<' ));
					}
				}

				QString kname(fullname.section( QLatin1String( " <" ), 0, 0));
				QString comment;
				if (fullname.contains(QLatin1Char( '(' )) ) {
					kname = kname.section( QLatin1String( " (" ), 0, 0);
					comment = fullname.section(QLatin1Char( '(' ), 1, 1);
					comment = comment.section(QLatin1Char( ')' ), 0, 0);
				}

				idIndex++;
				publickey->setEmail(kmail);
				publickey->setComment(comment);
				publickey->setName(kname);

				currentSNode = readNode;
			} else {
				idIndex++;
				if (readNode != NULL) {
					currentSNode = new KGpgUidNode(readNode, idIndex, lsp);
				}
			}
		} else if (((lsp.at(0) == QLatin1String( "sig" )) || (lsp.at(0) == QLatin1String( "rev" ))) && (items >= 11)) {
			// there are no strings here that could have a recoded QLatin1Char( ':' ) in them
			const QString signature = lsp.join(QLatin1String(":"));

			if (currentSNode != NULL)
				(void) new KGpgSignNode(currentSNode, lsp);
		} else {
			log += lsp.join(QString(QLatin1Char( ':' ))) + QLatin1Char( '\n' );
		}
	}

	if (p.exitCode() != 0) {
//		KMessageBox::detailedError(NULL, i18n("An error occurred while scanning your keyring"), log);
        qDebug() << "An error occurred while scanning your keyring" << " - " << log;
        log.clear();
	}

	return publiclistkeys;
}

KgpgKeyList KgpgInterface::readPublicKeys(const QStringList &ids)
{
	GPGProc process;
	process <<
			QLatin1String("--with-colons") <<
			QLatin1String("--with-fingerprint") <<
			QLatin1String("--fixed-list-mode") <<
            QLatin1String("--homedir") << GPGProc::getGpgHome("") <<
			QLatin1String("--list-keys") <<
			ids;

	process.setOutputChannelMode(KProcess::MergedChannels);

	process.start();
	process.waitForFinished(-1);
	return readPublicKeysProcess(process, NULL);
}

void KgpgInterface::readSignatures(KGpgKeyNode *node)
{
	GPGProc process;
	process <<
			QLatin1String("--with-colons") <<
			QLatin1String("--with-fingerprint") <<
			QLatin1String("--fixed-list-mode") <<
			QLatin1String("--list-sigs") <<
			node->getId();

	process.setOutputChannelMode(KProcess::MergedChannels);

	process.start();
	process.waitForFinished(-1);

	readPublicKeysProcess(process, node);
}

static KgpgCore::KgpgKeyList
readSecretKeysProcess(GPGProc &p)
{
	QStringList lsp;
	int items;
	bool hasuid = true;
	KgpgCore::KgpgKeyList result;
	KgpgCore::KgpgKey *secretkey = NULL;

	while ( (items = p.readln(lsp)) >= 0 ) {
		if ((lsp.at(0) == QLatin1String( "sec" )) && (items >= 10)) {
			result << KgpgKey(lsp.at(4), lsp.at(2).toUInt(), Convert::toTrust(lsp.at(1)),
				Convert::toAlgo(lsp.at(3).toInt()), QDateTime::fromTime_t(lsp.at(5).toUInt()));

			secretkey = &result.last();

			secretkey->setSecret(true);

			if (lsp.at(6).isEmpty())
				secretkey->setExpiration(QDateTime());
			else
				secretkey->setExpiration(QDateTime::fromTime_t(lsp.at(6).toUInt()));
			hasuid = true;
		} else if ((lsp.at(0) == QLatin1String( "uid" )) && (items >= 10)) {
			if (hasuid)
				continue;

			hasuid = true;

			const QString fullname(lsp.at(9));
			if (fullname.contains(QLatin1Char( '<' ) )) {
				QString kmail(fullname);

				if (fullname.contains(QLatin1Char( ')' ) ))
					kmail = kmail.section(QLatin1Char( ')' ), 1);

				kmail = kmail.section(QLatin1Char( '<' ), 1);
				kmail.truncate(kmail.length() - 1);

				if (kmail.contains(QLatin1Char( '<' ) )) { // several email addresses in the same key
					kmail = kmail.replace(QLatin1Char( '>' ), QLatin1Char( ';' ));
					kmail.remove(QLatin1Char( '<' ));
				}

				secretkey->setEmail(kmail);
			} else {
				secretkey->setEmail(QString());
			}

			QString kname(fullname.section( QLatin1String( " <" ), 0, 0));
			if (fullname.contains(QLatin1Char( '(' ) )) {
				kname = kname.section( QLatin1String( " (" ), 0, 0);
				QString comment = fullname.section(QLatin1Char( '(' ), 1, 1);
				comment = comment.section(QLatin1Char( ')' ), 0, 0);

				secretkey->setComment(comment);
			} else {
				secretkey->setComment(QString());
			}
			secretkey->setName(kname);
		} else if ((lsp.at(0) == QLatin1String( "fpr" )) && (items >= 10)) {
			secretkey->setFingerprint(lsp.at(9));
		}
	}

	return result;
}

KgpgKeyList KgpgInterface::readSecretKeys(const QStringList &ids)
{
	GPGProc process;
	process <<
			QLatin1String("--with-colons") <<
			QLatin1String("--list-secret-keys") <<
			QLatin1String("--with-fingerprint") <<
			QLatin1String("--fixed-list-mode") <<
			ids;

	process.start();
	process.waitForFinished(-1);
	KgpgCore::KgpgKeyList result = readSecretKeysProcess(process);

	return result;
}

//#include "kgpginterface.moc"
