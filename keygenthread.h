/*
 *      keygenthread.h
 *
 *      Copyright 2008 gpg4usb-team <gpg4usb@cpunk.de>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

#ifndef __KEYGENTHREAD_H__
#define __KEYGENTHREAD_H__

#include <qthread.h>
#include <iostream>
#include <string>
#include <cmath>
#include <QtGui>

class QMessageBox;

#include "context.h"

class KeyGenThread : public QThread
{
    Q_OBJECT

public:
    KeyGenThread(QString keyGenParams, GpgME::Context *ctx);

signals:
    void keyGenerated();

private:
    QString keyGenParams;
    GpgME::Context *ctx;
    bool abort;
    QMutex mutex;

protected:
    void run();

};
#endif // __KEYGENTHREAD_H__
