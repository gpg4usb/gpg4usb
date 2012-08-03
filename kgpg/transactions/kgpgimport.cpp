/*
 * Copyright (C) 2008,2009,2010,2012 Rolf Eike Beer <kde@opensource.sf-tec.de>
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kgpgimport.h"

//#include "model/kgpgitemmodel.h"
#include "../core/KGpgKeyNode.h"

#include <QDebug>
//#include <KLocale>

KGpgImport::KGpgImport(QObject *parent, const QString &text)
	: KGpgTextOrFileTransaction(parent, text, true)
{
}

KGpgImport::KGpgImport(QObject *parent, const QList<QUrl> &files)
	: KGpgTextOrFileTransaction(parent, files, true)
{
}

KGpgImport::~KGpgImport()
{
}

QStringList
KGpgImport::command() const
{
	QStringList ret;

	ret << QLatin1String( "--import" ) << QLatin1String( "--allow-secret-key-import" );

	return ret;
}

QStringList
KGpgImport::getImportedKeys() const
{
	QStringList res;

	foreach (const QString &str, getMessages())
		if (str.startsWith(QLatin1String("[GNUPG:] IMPORTED ")))
			res << str.mid(18);

	return res;
}

QStringList
KGpgImport::getImportedIds(const QStringList &log, const int reason)
{
	QStringList res;

	foreach (const QString &str, log) {
		if (!str.startsWith(QLatin1String("[GNUPG:] IMPORT_OK ")))
			continue;

		QString tmpstr(str.mid(19).simplified());

		int space = tmpstr.indexOf(QLatin1Char( ' ' ));
		if (space <= 0) {
            qDebug() << __LINE__ << "invalid format:" << str;
			continue;
		}

		bool ok;
		unsigned char code = tmpstr.left(space).toUInt(&ok);
		if (!ok) {
            qDebug() << __LINE__ << "invalid format:" << str << space << tmpstr.left(space - 1);
			continue;
		}

		if ((reason == -1) || ((reason == 0) && (code == 0)) || ((reason & code) != 0))
			res << tmpstr.mid(space + 1);
	}

	return res;
}

QStringList
KGpgImport::getImportedIds(const int reason) const
{
	return getImportedIds(getMessages(), reason);
}

QString
KGpgImport::getImportMessage() const
{
	return getImportMessage(getMessages());
}

QString
KGpgImport::getImportMessage(const QStringList &log)
{
#define RESULT_PARTS 14
	unsigned long rcode[RESULT_PARTS];
	unsigned int i = 0;
	int line = 0;
	bool fine;

	memset(rcode, 0, sizeof(rcode));

	foreach (const QString &str, log) {
		line++;
		if (!str.startsWith(QLatin1String("[GNUPG:] IMPORT_RES ")))
			continue;

		const QStringList rstr(str.mid(20).simplified().split(QLatin1Char( ' ' )));

		fine = (rstr.count() == RESULT_PARTS);

		i = 0;
		while (fine && (i < RESULT_PARTS)) {
			rcode[i] += rstr.at(i).toULong(&fine);
			i++;
		}

		if (!fine)
            return tr("The import result string has an unsupported format in line %1.<br />Please see the detailed log for more information.").arg(line);
	}

	fine = false;
	i = 0;
	while (!fine && (i < RESULT_PARTS)) {
		fine = (rcode[i] != 0);
		i++;
	}

	if (!fine)
        return tr("No key imported.<br />Please see the detailed log for more information.");

    QString resultMessage(tr("<qt>%1 key processed.</qt>", "<qt>%1 keys processed.</qt>", rcode[0]));

	if (rcode[1])
        resultMessage += tr("<qt><br />One key without ID.</qt>", "<qt><br />%1 keys without ID.</qt>", rcode[1]);
	if (rcode[2])
        resultMessage += tr("<qt><br /><b>One key imported:</b></qt>", "<qt><br /><b>%1 keys imported:</b></qt>", rcode[2]);
	if (rcode[3])
        resultMessage += tr("<qt><br />One RSA key imported.</qt>", "<qt><br />%1 RSA keys imported.</qt>", rcode[3]);
	if (rcode[4])
        resultMessage += tr("<qt><br />One key unchanged.</qt>", "<qt><br />%1 keys unchanged.</qt>", rcode[4]);
	if (rcode[5])
        resultMessage += tr("<qt><br />One user ID imported.</qt>", "<qt><br />%1 user IDs imported.</qt>", rcode[5]);
	if (rcode[6])
        resultMessage += tr("<qt><br />One subkey imported.</qt>", "<qt><br />%1 subkeys imported.</qt>", rcode[6]);
	if (rcode[7])
        resultMessage += tr("<qt><br />One signature imported.</qt>", "<qt><br />%1 signatures imported.</qt>", rcode[7]);
	if (rcode[8])
        resultMessage += tr("<qt><br />One revocation certificate imported.</qt>", "<qt><br />%1 revocation certificates imported.</qt>", rcode[8]);
	if (rcode[9])
        resultMessage += tr("<qt><br />One secret key processed.</qt>", "<qt><br />%1 secret keys processed.</qt>", rcode[9]);
	if (rcode[10])
        resultMessage += tr("<qt><br /><b>One secret key imported.</b></qt>", "<qt><br /><b>%1 secret keys imported.</b></qt>", rcode[10]);
	if (rcode[11])
        resultMessage += tr("<qt><br />One secret key unchanged.</qt>", "<qt><br />%1 secret keys unchanged.</qt>", rcode[11]);
	if (rcode[12])
        resultMessage += tr("<qt><br />One secret key not imported.</qt>", "<qt><br />%1 secret keys not imported.</qt>", rcode[12]);

	if (rcode[9])
        resultMessage += tr("<qt><br /><b>You have imported a secret key.</b> <br />"
		"Please note that imported secret keys are not trusted by default.<br />"
		"To fully use this secret key for signing and encryption, you must edit the key (double click on it) and set its trust to Full or Ultimate.</qt>");

	return resultMessage;
}

/*static QString
beautifyKeyList(const QStringList &keyIds, const KGpgItemModel *model)
{
	QString result;

	result.append(QLatin1String("\n"));
	if (model == NULL) {
		result.append(QLatin1String(" ") + keyIds.join(QLatin1String("\n ")));
	} else {
		foreach (const QString &changed, keyIds) {
			const KGpgKeyNode *node = model->findKeyNode(changed);
			QString line;

			if (node == NULL) {
				line = changed;
			} else {
				if (node->getEmail().isEmpty())
                    line = trc("ID: Name", "%1: %2", node->getFingerprint(), node->getName());
				else
                    line = trc("ID: Name <Email>", "%1: %2 &lt;%3&gt;", node->getFingerprint(), node->getName(), node->getEmail());
			}

			result.append(QLatin1String(" ") + line + QLatin1String("\n"));
		}
	}

	return result;
}*/

QString
KGpgImport::getDetailedImportMessage(const QStringList &log, const KGpgItemModel *model)
{
	QString result;
	QMap<QString, unsigned int> resultcodes;

	foreach (const QString &keyresult, log) {
		if (!keyresult.startsWith(QLatin1String("[GNUPG:] IMPORT_OK ")))
			continue;

		QStringList rc(keyresult.mid(19).split(QLatin1Char( ' ' )));
		if (rc.count() < 2) {
            qDebug() << "unexpected syntax:" << keyresult;
			continue;
		}

		resultcodes[rc.at(1)] = rc.at(0).toUInt();
	}

	QMap<QString, unsigned int>::const_iterator iterend = resultcodes.constEnd();

	for (unsigned int flag = 1; flag <= 16; flag <<= 1) {
		QStringList thischanged;

		for (QMap<QString, unsigned int>::const_iterator iter = resultcodes.constBegin(); iter != iterend; ++iter) {
			if (iter.value() & flag)
				thischanged << iter.key();
		}

		if (thischanged.isEmpty())
			continue;

		switch (flag) {
		case 1:
            result.append(tr("New Key", "New Keys", thischanged.count()));
			break;
		case 2:
            result.append(tr("Key with new User Id", "Keys with new User Ids", thischanged.count()));
			break;
		case 4:
            result.append(tr("Key with new Signatures", "Keys with new Signatures", thischanged.count()));
			break;
		case 8:
            result.append(tr("Key with new Subkeys", "Keys with new Subkeys", thischanged.count()));
			break;
		case 16:
            result.append(tr("New Private Key", "New Private Keys", thischanged.count()));
			break;
		default:
			Q_ASSERT(flag == 1);
		}

//		result.append(beautifyKeyList(thischanged, model));
		result.append(QLatin1String("\n\n"));
	}

	QStringList unchanged(resultcodes.keys(0));

	if (unchanged.isEmpty()) {
		// remove empty line at end
		result.chop(1);
	} else {
        result.append(tr("Unchanged Key", "Unchanged Keys", unchanged.count()));
//		result.append(beautifyKeyList(unchanged, model));
		result.append(QLatin1String("\n"));
	}

	return result;
}

int
KGpgImport::isKey(const QString &text, const bool incomplete)
{
	int markpos = text.indexOf(QLatin1String("-----BEGIN PGP PUBLIC KEY BLOCK-----"));
	if (markpos >= 0) {
		markpos = text.indexOf(QLatin1String("-----END PGP PUBLIC KEY BLOCK-----"), markpos);
		return ((markpos > 0) || incomplete) ? 1 : 0;
	}

	markpos = text.indexOf(QLatin1String("-----BEGIN PGP PRIVATE KEY BLOCK-----"));
	if (markpos < 0)
		return 0;

	markpos = text.indexOf(QLatin1String("-----END PGP PRIVATE KEY BLOCK-----"), markpos);
	if ((markpos < 0) && !incomplete)
		return 0;

	return 2;
}

//#include "kgpgimport.moc"
