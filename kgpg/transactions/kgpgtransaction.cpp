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

#include "kgpgtransaction.h"

#include "../gpgproc.h"
#include "../kgpginterface.h"

//#include <KDebug>
#include <QDebug>
//#include <knewpassworddialog.h>
//#include <KLocale>
//#include <KPushButton>
#include <QByteArray>
#include <QStringList>
#include <QWeakPointer>
#include <QWidget>



KGpgTransactionPrivate::KGpgTransactionPrivate(KGpgTransaction *parent, bool allowChaining)
	: QObject(parent),
	m_parent(parent),
	m_process(new GPGProc()),
	m_inputTransaction(NULL),
    //m_passwordDialog(NULL),
	m_success(KGpgTransaction::TS_OK),
	m_tries(3),
	m_chainingAllowed(allowChaining),
	m_passphraseAction(KGpgTransaction::PA_NONE),
	m_inputProcessDone(false),
	m_inputProcessResult(KGpgTransaction::TS_OK),
	m_ownProcessFinished(false),
	m_quitTries(0)
{
}

KGpgTransactionPrivate::~KGpgTransactionPrivate()
{
    /*if (m_passwordDialog) {
		m_passwordDialog->close();
		m_passwordDialog->deleteLater();
    }*/
	if (m_process->state() == QProcess::Running) {
		m_process->closeWriteChannel();
		m_process->terminate();
	}
	delete m_inputTransaction;
	delete m_process;
}

KGpgTransaction::KGpgTransaction(QObject *parent, const bool allowChaining)
	: QObject(parent),
	d(new KGpgTransactionPrivate(this, allowChaining))
{
	connect(d->m_process, SIGNAL(readReady()), SLOT(slotReadReady()));
	connect(d->m_process, SIGNAL(processExited()), SLOT(slotProcessExited()));
	connect(d->m_process, SIGNAL(started()), SLOT(slotProcessStarted()));
}

KGpgTransaction::~KGpgTransaction()
{
	delete d;
}

void
KGpgTransactionPrivate::slotReadReady()
{
	QString line;

	QWeakPointer<GPGProc> process(m_process);
	QWeakPointer<KGpgTransaction> par(m_parent);

	while (!process.isNull() && (m_process->readln(line, true) >= 0)) {
		if (m_quitTries)
			m_quitLines << line;
#ifdef KGPG_DEBUG_TRANSACTIONS
        qDebug() << m_parent << line;
#endif /* KGPG_DEBUG_TRANSACTIONS */
        //qDebug() << "trans-read: " << m_parent << line;


		if (line.startsWith(QLatin1String("[GNUPG:] USERID_HINT "))) {
			m_parent->addIdHint(line);
		} else if (line.startsWith(QLatin1String("[GNUPG:] BAD_PASSPHRASE "))) {
			m_success = KGpgTransaction::TS_BAD_PASSPHRASE;
		} else if (line.startsWith(QLatin1String("[GNUPG:] GET_HIDDEN passphrase.enter"))) {
			// Do not directly assign to the member here, the object could
			// get deleted while waiting for the result
			const KGpgTransaction::ts_passphrase_actions action = m_parent->passphraseRequested();

			if (par.isNull())
				return;

			m_passphraseAction = action;

			switch (action) {
			case KGpgTransaction::PA_USER_ABORTED:
				m_parent->setSuccess(KGpgTransaction::TS_USER_ABORTED);
				// sending "quit" here is useless as it would be interpreted as the passphrase
				m_process->closeWriteChannel();
				break;
			default:
				break;
			}
		} else if ((m_passphraseAction == KGpgTransaction::PA_CLOSE_GOOD) &&
				line.startsWith(QLatin1String("[GNUPG:] GOOD_PASSPHRASE"))) {
			// signal GnuPG that there will be no further input and it can
			// begin sending output.

            // except when signing text...
            if(QString::fromAscii(m_parent->metaObject()->className()) != "KGpgSignText"){
                m_process->closeWriteChannel();
            }
		} else if (line.startsWith(QLatin1String("[GNUPG:] GET_BOOL "))) {
			switch (m_parent->boolQuestion(line.mid(18))) {
			case KGpgTransaction::BA_YES:
				write("YES\n");
				break;
			case KGpgTransaction::BA_NO:
				write("NO\n");
				break;
			case KGpgTransaction::BA_UNKNOWN:
				m_parent->setSuccess(KGpgTransaction::TS_MSG_SEQUENCE);
				m_parent->unexpectedLine(line);
				sendQuit();
			}
		} else if (line.startsWith(QLatin1String("[GNUPG:] CARDCTRL "))) {
			// just ignore them, pinentry should handle that
		} else if (m_parent->nextLine(line)) {
			sendQuit();
		}
	}
}

void
KGpgTransactionPrivate::slotProcessExited()
{
	Q_ASSERT(m_parent->sender() == m_process);
	m_ownProcessFinished = true;

	if (m_inputProcessDone)
		processDone();
}

void
KGpgTransactionPrivate::slotProcessStarted()
{
	m_parent->postStart();
}

void
KGpgTransactionPrivate::sendQuit(void)
{
	write("quit\n");

#ifdef KGPG_DEBUG_TRANSACTIONS
	if (m_quitTries == 0)
        qDebug() << "sending quit";
#endif /* KGPG_DEBUG_TRANSACTIONS */

	if (m_quitTries++ >= 3) {
        qDebug() << "tried" << m_quitTries << "times to quit the GnuPG session";
        qDebug() << "last input was" << m_quitLines;
        //qDebug() << "please file a bug report at https://bugs.kde.org";
		m_process->closeWriteChannel();
		m_success = KGpgTransaction::TS_MSG_SEQUENCE;
	}
}

void
KGpgTransactionPrivate::slotInputTransactionDone(int result)
{
	Q_ASSERT(m_parent->sender() == m_inputTransaction);

	m_inputProcessDone = true;
	m_inputProcessResult = result;

	if (m_ownProcessFinished)
		processDone();
}

void
KGpgTransactionPrivate::slotPasswordEntered(const QString &password)
{
	sender()->deleteLater();
    //m_passwordDialog = NULL;
	m_process->write(password.toUtf8() + '\n');
	m_parent->newPasswordEntered();
}

void
KGpgTransactionPrivate::slotPasswordAborted()
{
	sender()->deleteLater();
    //m_passwordDialog = NULL;
	m_process->closeWriteChannel();
	m_success = KGpgTransaction::TS_USER_ABORTED;
}

void
KGpgTransactionPrivate::write(const QByteArray &a)
{
	m_process->write(a);
#ifdef KGPG_DEBUG_TRANSACTIONS
    qDebug() << m_parent << a;
#endif /* KGPG_DEBUG_TRANSACTIONS */
    //qDebug() << "trans-write: " << m_parent << a;
}

void
KGpgTransactionPrivate::processDone()
{
	m_parent->finish();
	emit m_parent->infoProgress(100, 100);
	emit m_parent->done(m_success);
}

void
KGpgTransaction::start()
{
	d->m_inputProcessResult = false;
	d->m_inputProcessDone = (d->m_inputTransaction == NULL);

	setSuccess(TS_OK);
	d->m_idhints.clear();
	d->m_tries = 3;
	if (preStart()) {
		d->m_ownProcessFinished = false;
		if (d->m_inputTransaction != NULL)
			d->m_inputTransaction->start();
#ifdef KGPG_DEBUG_TRANSACTIONS
		kDebug(2100) << this << d->m_process->program();
#endif /* KGPG_DEBUG_TRANSACTIONS */
		d->m_process->start();
		emit infoProgress(0, 1);
	} else {
		emit done(d->m_success);
	}
}

void
KGpgTransaction::write(const QByteArray &a, const bool lf)
{
	if (lf)
		d->write(a + '\n');
	else
		d->write(a);
}

void
KGpgTransaction::write(const int i)
{
	write(QByteArray::number(i));
}

void
KGpgTransaction::askNewPassphrase(const QString& text)
{
    //qDebug() << "KGpgTransaction::askNewPassphrase called";

    emit statusMessage(tr("Requesting Passphrase"));

    /*d->m_passwordDialog = new KNewPasswordDialog(qobject_cast<QWidget *>(parent()));
	d->m_passwordDialog->setPrompt(text);
	d->m_passwordDialog->setAllowEmptyPasswords(false);
	connect(d->m_passwordDialog, SIGNAL(newPassword(QString)), SLOT(slotPasswordEntered(QString)));
	connect(d->m_passwordDialog, SIGNAL(rejected()), SLOT(slotPasswordAborted()));
	connect(d->m_process, SIGNAL(processExited()), d->m_passwordDialog->button(KDialog::Cancel), SLOT(clicked()));
    d->m_passwordDialog->show();*/


}

int
KGpgTransaction::getSuccess() const
{
	return d->m_success;
}

void
KGpgTransaction::setSuccess(const int v)
{
#ifdef KGPG_DEBUG_TRANSACTIONS
    qDebug() << d->m_success << v;
#endif /* KGPG_DEBUG_TRANSACTIONS */
	d->m_success = v;
}

KGpgTransaction::ts_boolanswer
KGpgTransaction::boolQuestion(const QString& line)
{
	Q_UNUSED(line);

	return BA_UNKNOWN;
}

void
KGpgTransaction::finish()
{
}

void
KGpgTransaction::setDescription(const QString &description)
{
	d->m_description = description;
}

void
KGpgTransaction::waitForInputTransaction()
{
	Q_ASSERT(d->m_inputTransaction != NULL);

	if (d->m_inputProcessDone)
		return;

	d->m_inputTransaction->waitForFinished();
}

void
KGpgTransaction::unexpectedLine(const QString &line)
{
    qDebug() << this << "unexpected input line" << line << "for command" << d->m_process->program();
}

KGpgTransaction::ts_passphrase_actions
KGpgTransaction::passphraseRequested()
{
    //qDebug() << "KGpgTransaction::passphraseRequested called";
	if (!askPassphrase())
		return PA_USER_ABORTED;
	else
		return PA_CLOSE_GOOD;
}

bool
KGpgTransaction::preStart()
{
	return true;
}

void
KGpgTransaction::postStart()
{
}

void
KGpgTransaction::addIdHint(QString txt)
{
	int cut = txt.indexOf(QLatin1Char( ' ' ), 22, Qt::CaseInsensitive);
	txt.remove(0, cut);

	if (txt.contains(QLatin1Char( '(' ), Qt::CaseInsensitive))
		txt = txt.section(QLatin1Char( '(' ), 0, 0) + txt.section(QLatin1Char( ')' ), -1);

	txt.replace(QLatin1Char( '<' ), QLatin1String( "&lt;" ));

	if (!d->m_idhints.contains(txt))
		d->m_idhints << txt;
}

QString
KGpgTransaction::getIdHints() const
{
    return d->m_idhints.join( tr(" or " ));
}

GPGProc *
KGpgTransaction::getProcess()
{
	return d->m_process;
}

int
KGpgTransaction::addArgument(const QString &arg)
{
	int r = d->m_process->program().count();

	*d->m_process << arg;

	return r;
}

int
KGpgTransaction::addArguments(const QStringList &args)
{
	int r = d->m_process->program().count();

	*d->m_process << args;

	return r;
}

void
KGpgTransaction::replaceArgument(const int pos, const QString &arg)
{
	QStringList args(d->m_process->program());
	d->m_process->clearProgram();

	args.replace(pos, arg);

	d->m_process->setProgram(args);
}

void
KGpgTransaction::insertArgument(const int pos, const QString &arg)
{
	insertArguments(pos, QStringList(arg));
}

void
KGpgTransaction::insertArguments(const int pos, const QStringList &args)
{
	QStringList tmp(d->m_process->program());

	int tmppos = pos;
	foreach (const QString &s, args) {
		tmp.insert(tmppos++, s);
	}
	d->m_process->setProgram(tmp);

	int move = args.count();
	foreach (int *ref, d->m_argRefs) {
		if (*ref >= pos)
			*ref += move;
	}
}

void
KGpgTransaction::addArgumentRef(int *ref)
{
	d->m_argRefs.append(ref);
}

bool
KGpgTransaction::askPassphrase(const QString &message)
{
   // qDebug() << "KGpgTransaction::askPassphrase called";

	if (d->m_passphraseAction == PA_USER_ABORTED)
		return false;

	QString passdlgmessage;
	QString userIDs(getIdHints());
	if (userIDs.isEmpty())
        userIDs = tr("[No user id found]");
	else
		userIDs.replace(QLatin1Char( '<' ), QLatin1String( "&lt;" ));

	if (d->m_tries < 3)
        passdlgmessage = tr("<p><b>Bad passphrase</b>. You have 1 try left.</p>", "<p><b>Bad passphrase</b>. You have %1 tries left.</p>", d->m_tries);
	if (message.isEmpty()) {
        passdlgmessage += tr("Enter passphrase for <b>%1</b>").arg(userIDs);
	} else {
		passdlgmessage += message;
	}

	--d->m_tries;

    emit statusMessage(tr("Requesting Passphrase"));
	return (KgpgInterface::sendPassphrase(passdlgmessage, d->m_process, qobject_cast<QWidget *>(parent())) == 0);
}

void
KGpgTransaction::setGnuPGHome(const QString &home)
{
	QStringList tmp(d->m_process->program());

	Q_ASSERT(tmp.count() > 3);
	int homepos = tmp.indexOf(QLatin1String("--options"), 1);
	if (homepos == -1)
		homepos = tmp.indexOf(QLatin1String("--homedir"), 1);
	Q_ASSERT(homepos != -1);
	Q_ASSERT(homepos + 1 < tmp.count());

	tmp[homepos] = QLatin1String("--homedir");
	tmp[homepos + 1] = home;

	d->m_process->setProgram(tmp);
}

int
KGpgTransaction::waitForFinished(const int msecs)
{
	int ret = TS_OK;

	if (d->m_inputTransaction != NULL) {
		int ret = d->m_inputTransaction->waitForFinished(msecs);
		if ((ret != TS_OK) && (msecs != -1))
			return ret;
	}

	bool b = d->m_process->waitForFinished(msecs);

	if (ret != TS_OK)
		return ret;

	if (!b)
		return TS_USER_ABORTED;
	else
		return getSuccess();
}

const QString &
KGpgTransaction::getDescription() const
{
	return d->m_description;
}

void
KGpgTransaction::setInputTransaction(KGpgTransaction *ta)
{
	Q_ASSERT(d->m_chainingAllowed);

	if (d->m_inputTransaction != NULL)
		clearInputTransaction();
	d->m_inputTransaction = ta;

	GPGProc *proc = ta->getProcess();
	proc->setStandardOutputProcess(d->m_process);
	connect(ta, SIGNAL(done(int)), SLOT(slotInputTransactionDone(int)));
}

void
KGpgTransaction::clearInputTransaction()
{
	disconnect(d->m_inputTransaction, SIGNAL(done(int)), this, SLOT(slotInputTransactionDone(int)));
	d->m_inputTransaction = NULL;
}

bool
KGpgTransaction::hasInputTransaction() const
{
	return (d->m_inputTransaction != NULL);
}

void
KGpgTransaction::kill()
{
	d->m_process->kill();
}

void
KGpgTransaction::newPasswordEntered()
{
}

//#include "moc_kgpgtransaction.cpp"
