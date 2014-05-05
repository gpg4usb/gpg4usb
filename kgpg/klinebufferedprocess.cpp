/*
 * Copyright (C) 2008 Rolf Eike Beer <kde@opensource.sf-tec.de>
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "klinebufferedprocess.h"


KLineBufferedProcessPrivate::KLineBufferedProcessPrivate(KLineBufferedProcess *parent)
 : m_newlineInStdout(-1),
   m_newlineInStderr(-1),
   m_parent(parent),
#ifdef Q_OS_WIN32	//krazy:exclude=cpp
   m_lineEnd("\r\n")
#else
   m_lineEnd("\n")
#endif
{
}

KLineBufferedProcess::KLineBufferedProcess(QObject *parent)
 : KProcess(parent),
   d(new KLineBufferedProcessPrivate(this))
{
    connect(this, SIGNAL(readyReadStandardOutput()), this, SLOT(_k_receivedStdout()));
    connect(this, SIGNAL(readyReadStandardError()), this, SLOT(_k_receivedStderr()));
}

KLineBufferedProcess::~KLineBufferedProcess()
{
    delete d;
}

void KLineBufferedProcessPrivate::_k_receivedStdout()
{
    QByteArray ndata = m_parent->readAllStandardOutput();
    int oldBufferSize = m_stdoutBuffer.size();
    m_stdoutBuffer.append(ndata);

    if (m_newlineInStdout < 0) {
        m_newlineInStdout = ndata.indexOf(m_lineEnd);
        if (m_newlineInStdout >= 0) {
            m_newlineInStdout += oldBufferSize;
            emit m_parent->lineReadyStandardOutput();
        }
    }
}

void KLineBufferedProcessPrivate::_k_receivedStderr()
{
    QByteArray ndata = m_parent->readAllStandardError();
    int oldBufferSize = m_stderrBuffer.size();
    m_stderrBuffer.append(ndata);

   if (m_newlineInStderr < 0) {
        m_newlineInStderr = ndata.indexOf(m_lineEnd);
        if (m_newlineInStderr >= 0) {
            m_newlineInStderr += oldBufferSize;
            emit m_parent->lineReadyStandardError();
        }
    }
}

bool KLineBufferedProcess::readLineStandardOutput(QByteArray *line)
{
    if (d->m_newlineInStdout < 0) {
        return false;
    }

    // don't copy '\n'
    *line = d->m_stdoutBuffer.left(d->m_newlineInStdout);

#ifdef Q_OS_WIN32
    // sometimes there may be a single "\n", which confuses decrypt on win
    if(d->m_stdoutBuffer.at(d->m_newlineInStdout) == '\r') {
        d->m_stdoutBuffer.remove(0, d->m_newlineInStdout + 2);
    } else {
        d->m_stdoutBuffer.remove(0, d->m_newlineInStdout + 1);
    }

    int lepos = d->m_stdoutBuffer.indexOf("\n");
    if(d->m_stdoutBuffer.at(lepos-1) == '\r') {
        lepos = lepos - 1;
    }
    d->m_newlineInStdout = lepos;
#else
    d->m_stdoutBuffer.remove(0, d->m_newlineInStdout + d->m_lineEnd.length());

    d->m_newlineInStdout = d->m_stdoutBuffer.indexOf(d->m_lineEnd);
#endif
    return true;
}

bool KLineBufferedProcess::readLineStandardError(QByteArray *line)
{
    if (d->m_newlineInStderr < 0) {
        return false;
    }

    // don't copy '\n'
    *line = d->m_stderrBuffer.left(d->m_newlineInStderr);
    d->m_stderrBuffer.remove(0, d->m_newlineInStderr + d->m_lineEnd.length());

    d->m_newlineInStderr = d->m_stderrBuffer.indexOf(d->m_lineEnd);

    return true;
}

bool KLineBufferedProcess::hasLineStandardOutput() const
{
    return d->m_newlineInStdout >= 0;
}

bool KLineBufferedProcess::hasLineStandardError() const
{
    return d->m_newlineInStderr >= 0;
}

//#include "moc_klinebufferedprocess.cpp"
