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

#ifndef KGPGSIGNTEXT_H
#define KGPGSIGNTEXT_H

#include "kgpgtextorfiletransaction.h"

#include <QUrl>
#include <QObject>
#include <QString>
#include <QStringList>

class QProcess;

/**
 * @brief sign the given text or files
 */
class KGpgSignText: public KGpgTextOrFileTransaction {
	Q_OBJECT

	Q_DISABLE_COPY(KGpgSignText)
	KGpgSignText(); // = delete C++0x
public:
	enum SignOption {
		DefaultSignature = 0,		///< use whatever GnuPGs defaults are
		AsciiArmored = 0x1,		///< output the data as printable ASCII as opposed to binary data
		DetachedSignature = 0x2,	///< save the signature in a separate file
	};
	Q_DECLARE_FLAGS(SignOptions, SignOption);

	/**
	 * @brief sign given text
	 * @param parent parent object
	 * @param signId the key to use for signing
	 * @param text text to sign
	 * @param options signing options
	 */
	KGpgSignText(QObject *parent, const QString &signId, const QString &text = QString(), const SignOptions &options = AsciiArmored, const QStringList &extraOptions = QStringList());

	/**
	 * @brief sign file
	 * @param parent parent object
	 * @param signId the key to use for signing
	 * @param files list of file locations to sign (must only be 1 file)
	 * @param options signing options
	 *
	 * @warning GnuPG can currently handle only one file per invocation for
	 * signing, so files may only contain one single file.
	 */
    KGpgSignText(QObject *parent, const QString &signId, const QList<QUrl> &files, const SignOptions &options = DefaultSignature, const QStringList &extraOptions = QStringList());

	/**
	 * @brief destructor
	 */
	virtual ~KGpgSignText();

	/**
	 * @brief get signing result
	 * @return signed text
	 */
	QStringList signedText() const;

    /**
     * @brief get gpg info message
     * @return the raw messages from gpg during the operation
     */
    const QStringList &getMessages() const;

protected:
	virtual QStringList command() const;
    virtual bool nextLine(const QString &line);

private:
	int m_fileIndex;
	const SignOptions m_options;
	const QString m_signId;
	QStringList m_extraOptions;
    const QString m_text;
    QStringList m_messages;

private slots:
    void postStart();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KGpgSignText::SignOptions);

#endif // KGPGSIGNTEXT_H
