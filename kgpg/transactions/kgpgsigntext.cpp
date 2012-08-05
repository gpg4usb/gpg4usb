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

#include "kgpgsigntext.h"
#include <QDebug>
#include "../gpgproc.h"

//#include "kgpgsettings.h"

KGpgSignText::KGpgSignText(QObject *parent, const QString &signId, const QString &text, const SignOptions &options, const QStringList &extraOptions)
	: KGpgTextOrFileTransaction(parent, text),
	m_fileIndex(-1),
	m_options(options),
	m_signId(signId),
    m_extraOptions(extraOptions),
    m_text(text)
{
}

KGpgSignText::KGpgSignText(QObject *parent, const QString &signId, const QList<QUrl> &files, const SignOptions &options, const QStringList &extraOptions)
	: KGpgTextOrFileTransaction(parent, files),
	m_fileIndex(0),
	m_options(options),
	m_signId(signId),
	m_extraOptions(extraOptions)
{
	/* GnuPG can only handle one file at a time when signing */
	Q_ASSERT(files.count() == 1);
}

KGpgSignText::~KGpgSignText()
{
}

QStringList
KGpgSignText::command() const
{
	QStringList ret = m_extraOptions;

    const QList<QUrl> &files = getInputFiles();
	QString fileName;

	if (!files.isEmpty())
		fileName = files.first().path();

	ret << QLatin1String("-u") << m_signId;

	if (m_options & AsciiArmored) {
		if (fileName.isEmpty())
			ret << QLatin1String("--clearsign");
		else
			ret << QLatin1String("--armor");
	}
    /*if (KGpgSettings::pgpCompatibility())
        ret << QLatin1String("--pgp6");*/

	if (!fileName.isEmpty()) {
		if (m_options & DetachedSignature)
			ret << QLatin1String("--detach-sign") <<
					QLatin1String("--output") << fileName + QLatin1String(".sig");

		ret << fileName;
	}

    // command-fd for pass?
    ret << QLatin1String("--command-fd=0");

	return ret;
}

QStringList
KGpgSignText::signedText() const
{
	QStringList result;

	foreach (const QString &line, getMessages())
		if (!line.startsWith(QLatin1String("[GNUPG:] "))) {
			result.append(line);
        }

	return result;
}

void
KGpgSignText::postStart()
{
    // do nothing, its to early
}

bool KGpgSignText::nextLine(const QString &line)  {

    if (line.startsWith(QLatin1String("[GNUPG:] BEGIN_SIGNING")) &! m_text.isEmpty()) {
        GPGProc *proc = getProcess();
        proc->write(m_text.toUtf8());
        proc->closeWriteChannel();
    } else if (!line.startsWith(QLatin1String("[GNUPG:] SIGEXPIRED")) && !line.startsWith(QLatin1String("[GNUPG:] KEYEXPIRED ")))
        m_messages.append(line);

    return false;
}

const QStringList &
KGpgSignText::getMessages() const
{
    return m_messages;
}

//#include "kgpgsigntext.moc"
