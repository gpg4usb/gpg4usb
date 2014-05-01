/*
 * Copyright (C) 2008,2009,2010,2011,2012 Rolf Eike Beer <kde@opensource.sf-tec.de>
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kgpgtextorfiletransaction.h"

#include "../gpgproc.h"

//#include <KIO/NetAccess>
//#include <KLocale>
#include <QDebug>

KGpgTextOrFileTransaction::KGpgTextOrFileTransaction(QObject *parent, const QString &text, const bool allowChaining)
	: KGpgTransaction(parent, allowChaining),
	m_text(text)
{
}

KGpgTextOrFileTransaction::KGpgTextOrFileTransaction(QObject *parent, const QList<QUrl> &files, const bool allowChaining)
	: KGpgTransaction(parent, allowChaining)
{
	setUrls(files);
}

KGpgTextOrFileTransaction::~KGpgTextOrFileTransaction()
{
	cleanUrls();
}

void
KGpgTextOrFileTransaction::setText(const QString &text)
{
	m_text = text;
	cleanUrls();
}

void
KGpgTextOrFileTransaction::setUrls(const QList<QUrl> &files)
{
	m_text.clear();
	m_inpfiles = files;
    //qDebug() << "files set:";
    foreach(QUrl file, m_inpfiles) {
        qDebug() << file.toString();
    }
}

bool
KGpgTextOrFileTransaction::preStart()
{
	QStringList locfiles;

    foreach (const QUrl &url, m_inpfiles) {
                // qt 4.8 ! todo mac
        //qDebug() << "fileurl: "<< url;
        //qDebug() << "what the loc:" << url.isLocalFile();

		if (url.isLocalFile()) {
			locfiles.append(url.toLocalFile());
        } else {
        /*	QString tmpfile;
        //TODO: QIODevice ...?
                        if (KIO::NetAccess::download(url, tmpfile, 0)) {
				m_tempfiles.append(tmpfile);
			} else {
				m_messages.append(KIO::NetAccess::lastErrorString());
				cleanUrls();
				setSuccess(TS_KIO_FAILED);
				return false;
            }*/
        }
    }

    //qDebug() << "m_text: " << m_text;
    //qDebug() << "hasInputTransaction: " << hasInputTransaction();

	if (locfiles.isEmpty() && m_tempfiles.isEmpty() && m_text.isEmpty() && !hasInputTransaction()) {
		setSuccess(TS_MSG_SEQUENCE);
		return false;
	}

	QStringList args(QLatin1String("--status-fd=1"));

	args << command();
	// if the input is not stdin set command-fd so GnuPG
	// can ask if e.g. the file already exists
	if (!locfiles.isEmpty() && !m_tempfiles.isEmpty()) {
		args << QLatin1String("--command-fd=0");
		m_closeInput = false;
        //qDebug() << "if";
	} else {
		m_closeInput = !args.contains(QLatin1String("--command-fd=0"));
        //qDebug() << "else";
	}
	if (locfiles.count() + m_tempfiles.count() > 1)
		args << QLatin1String("--multifile");
	args << locfiles << m_tempfiles;
	addArguments(args);

	return true;
}

void
KGpgTextOrFileTransaction::postStart()
{
    //qDebug() << "post-start! " << m_text;
	if (!m_text.isEmpty()){
		GPGProc *proc = getProcess();
		proc->write(m_text.toUtf8());
		if (m_closeInput)
			proc->closeWriteChannel();
    }
}

bool
KGpgTextOrFileTransaction::nextLine(const QString &line)
{
    //qDebug() << "nextline called: " << line;
	if (!line.startsWith(QLatin1String("[GNUPG:] SIGEXPIRED")) && !line.startsWith(QLatin1String("[GNUPG:] KEYEXPIRED ")))
		m_messages.append(line);

	return false;
}

void
KGpgTextOrFileTransaction::finish()
{
	if (getProcess()->exitCode() != 0) {
		setSuccess(TS_MSG_SEQUENCE);
	}
}

const QStringList &
KGpgTextOrFileTransaction::getMessages() const
{
	return m_messages;
}

void
KGpgTextOrFileTransaction::cleanUrls()
{
//  TODO
/*	foreach (const QString &u, m_tempfiles)
		KIO::NetAccess::removeTempFile(u);
*/
	m_tempfiles.clear();
	m_locfiles.clear();
	m_inpfiles.clear();
}

const QList<QUrl> &
KGpgTextOrFileTransaction::getInputFiles() const
{
	return m_inpfiles;
}

//#include "kgpgtextorfiletransaction.moc"
