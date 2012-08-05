/*
 * Copyright (C) 2012 Rolf Eike Beer <kde@opensource.sf-tec.de>
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KGPGVERIFY_H
#define KGPGVERIFY_H

#include "kgpgtextorfiletransaction.h"

#include <QObject>
#include <QString>
#include <QStringList>

#include <QUrl>

//class KGpgItemModel;
class QProcess;

/**
 * @brief verify the signature of the given text or files
 */
class KGpgVerify: public KGpgTextOrFileTransaction {
	Q_OBJECT

	Q_DISABLE_COPY(KGpgVerify)
	KGpgVerify(); // = delete C++0x
public:
	enum ts_verify {
		TS_MISSING_KEY = KGpgTransaction::TS_COMMON_END + 1,	///< signing key not in keyring
		TS_BAD_SIGNATURE = TS_MISSING_KEY + 1			///< the file is signed, but the signature is invalid
	};

	/**
	 * @brief verify signature of given text
	 * @param parent parent object
	 * @param text text to verify
	 */
	explicit KGpgVerify(QObject *parent, const QString &text = QString());

	/**
	 * @brief verify signatures of file(s)
	 * @param parent parent object
	 * @param files list of file locations to verify
	 */
    KGpgVerify(QObject *parent, const QList<QUrl> &files);

	/**
	 * @brief destructor
	 */
	virtual ~KGpgVerify();

	/**
	 * @brief get verification report
	 * @param log the log lines to scan
	 * @param model key model to use for key lookups
	 * @return verification report of GnuPG
	 */
    //static QString getReport(const QStringList &log, const KGpgItemModel *model = NULL);

	/**
	 * @brief get the missing key id
	 * @return key id that signed the message
	 *
	 * This is only valid if the transaction returned with result
	 * TS_MISSING_KEY.
	 */
	QString missingId() const;

protected:
	virtual QStringList command() const;
	virtual bool nextLine(const QString &line);
	virtual void finish();

private:
	int m_fileIndex;
	QString m_currentFile;
	QStringList m_report;
	QString m_missingId;
};

#endif // KGPGVERIFY_H
