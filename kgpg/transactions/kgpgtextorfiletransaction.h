/*
 * Copyright (C) 2008,2009,2010 Rolf Eike Beer <kde@opensource.sf-tec.de>
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KGPGTEXTORFILETRANSACTION_H
#define KGPGTEXTORFILETRANSACTION_H

#include <QObject>
#include <QList>
#include <QString>
#include <QStringList>

#include <QUrl>

#include "kgpgtransaction.h"

/**
 * @brief feed a text or file through gpg
 */
class KGpgTextOrFileTransaction: public KGpgTransaction {
	Q_OBJECT

	Q_DISABLE_COPY(KGpgTextOrFileTransaction)

public:
	/**
	 * @brief additional status codes for KGpgImport
	 */
	enum ts_import {
		TS_KIO_FAILED = TS_COMMON_END + 1	///< download of remote file failed
	};

protected:
	/**
	 * @brief work with given text
	 * @param parent parent object
	 * @param text text to work with
	 */
	explicit KGpgTextOrFileTransaction(QObject *parent = 0, const QString &text = QString(), const bool allowChaining = false);

	/**
	 * @brief work with given file(s)
	 * @param parent parent object
	 * @param keys list of file locations to work with
	 */
    KGpgTextOrFileTransaction(QObject *parent, const QList<QUrl> &files, const bool allowChaining = false);

public:
	/**
	 * @brief destructor
	 */
	virtual ~KGpgTextOrFileTransaction();

	/**
	 * @brief set text to work with
	 * @param text text to work with
	 */
	void setText(const QString &text);
	/**
	 * @brief set file locations to work with
	 * @param keys list of file locations to work with
	 */
    void setUrls(const QList<QUrl> &files);

	/**
	 * @brief get gpg info message
	 * @return the raw messages from gpg during the operation
	 */
	const QStringList &getMessages() const;

protected:
	/**
	 * @brief construct the command line of the process
	 */
	virtual bool preStart();
	virtual bool nextLine(const QString &line);
	/**
	 * @brief implement special handling for GnuPG return codes
	 */
	virtual void finish();

	virtual QStringList command() const = 0;

    const QList<QUrl> &getInputFiles() const;

private:
	QStringList m_tempfiles;
	QStringList m_locfiles;
    QList<QUrl> m_inpfiles;
	QString m_text;
	QStringList m_messages;
	bool m_closeInput;	///< if input channel of GnuPG should be closed after m_text is written

	void cleanUrls();

private slots:
	void postStart();
};

#endif // KGPGTEXTORFILETRANSACTION_H
