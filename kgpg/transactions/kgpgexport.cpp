/*
 * Copyright (C) 2009,2012 Rolf Eike Beer <kde@opensource.sf-tec.de>
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kgpgexport.h"

#include "../gpgproc.h"

#include <QFile>
#include <QProcess>

KGpgExport::KGpgExport(QObject *parent, const QStringList &ids, QProcess *outp, const QStringList &options, const bool secret)
	: KGpgTransaction(parent),
	m_keyids(ids),
	m_outp(outp),
	m_outputmode(ModeProcess)
{
	procSetup(options, secret);
}

KGpgExport::KGpgExport(QObject *parent, const QStringList &ids, const QString &file, const QStringList &options, const bool secret)
	: KGpgTransaction(parent),
	m_keyids(ids),
	m_outp(NULL),
	m_outf(file),
	m_outputmode(ModeFile)
{
	procSetup(options, secret);
}

KGpgExport::KGpgExport(QObject *parent, const QStringList &ids, const QStringList &options, const bool secret)
	: KGpgTransaction(parent),
	m_keyids(ids),
	m_outp(NULL),
	m_outputmode(ModeStdout)
{
	procSetup(options, secret);
}

KGpgExport::KGpgExport(QObject *parent, const QStringList &ids, KGpgTransaction *outt, const QStringList &options, const bool secret)
	: KGpgTransaction(parent),
	m_keyids(ids),
	m_outp(NULL),
	m_outputmode(ModeTransaction)
{
	procSetup(options, secret);
	outt->setInputTransaction(this);
}

KGpgExport::~KGpgExport()
{
}

void
KGpgExport::setKeyId(const QString &id)
{
	m_keyids.clear();
	m_keyids.append(id);
}

void
KGpgExport::setKeyIds(const QStringList &ids)
{
	m_keyids = ids;
}

const QStringList &
KGpgExport::getKeyIds() const
{
	return m_keyids;
}

void
KGpgExport::setOutputProcess(QProcess *outp)
{
	m_outf.clear();
	m_outp = outp;
	m_outputmode = ModeProcess;
}

void
KGpgExport::setOutputFile(const QString &filename)
{
	m_outp = NULL;
	m_outf = filename;
	if (filename.isEmpty())
		m_outputmode = ModeStdout;
	else
		m_outputmode = ModeFile;
}

void
KGpgExport::setOutputTransaction(KGpgTransaction *outt)
{
	m_outp = NULL;
	m_outf.clear();
	m_outputmode = ModeTransaction;
	outt->setInputTransaction(this);
}

const QString &
KGpgExport::getOutputFile() const
{
	return m_outf;
}

const QByteArray &
KGpgExport::getOutputData() const
{
	return m_data;
}

bool
KGpgExport::preStart()
{
	setSuccess(TS_OK);

	switch (m_outputmode) {
	case ModeFile:
		{
		Q_ASSERT(!m_outf.isEmpty());
		Q_ASSERT(m_outp == NULL);

		addArgument(QLatin1String( "--output" ));
		addArgument(m_outf);

		QFile ofile(m_outf);
		if (ofile.exists())
			ofile.remove();

		break;
		}
	case ModeProcess:
		Q_ASSERT(m_outf.isEmpty());
		Q_ASSERT(m_outp != NULL);

		getProcess()->setStandardOutputProcess(m_outp);

		break;
	case ModeStdout:
		Q_ASSERT(m_outf.isEmpty());
		Q_ASSERT(m_outp == NULL);
		break;
	case ModeTransaction:
		Q_ASSERT(m_outf.isEmpty());
		Q_ASSERT(m_outp == NULL);
		break;
	default:
		Q_ASSERT(0);
	}

	addArguments(m_keyids);

	m_data.clear();

	return true;
}

bool
KGpgExport::nextLine(const QString &line)
{
	// key exporting does not send any messages

	m_data.append(line.toAscii() + '\n');

	if (m_outputmode != 2)
		setSuccess(TS_MSG_SEQUENCE);

	return false;
}

void
KGpgExport::procSetup(const QStringList &options, const bool secret)
{
	getProcess()->resetProcess();

	if (secret)
		addArgument(QLatin1String( "--export-secret-key" ));
	else
		addArgument(QLatin1String( "--export" ));

	if ((m_outputmode == 2) && !options.contains(QLatin1String( "--armor" )))
		addArgument(QLatin1String( "--armor" ));

	addArguments(options);
}

//#include "kgpgexport.moc"
