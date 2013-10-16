/*
 *      keydetailswidget.h
 *
 *      Copyright 2013 gpg4usb-team <gpg4usb@cpunk.de>
 *
 *      This file is part of gpg4usb.
 *
 *      Gpg4usb is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *
 *      Gpg4usb is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with gpg4usb.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef KEYDETAILSWIDGET_H
#define KEYDETAILSWIDGET_H

#include <QWidget>
#include <QDeclarativeContext>
#include <QGraphicsObject>
#include "kgpg/core/kgpgkey.h"
#include "gpgcontext.h"

class KeyDetailsWidget : public QWidget
{

    Q_OBJECT

public:
    KeyDetailsWidget(GpgME::GpgContext *ctx, KgpgCore::KgpgKey key, QWidget *parent = 0);

public slots:
    void qmlClicked();
    void slotExportPublicKey();
    void slotExportPrivateKey();
private:
    QDeclarativeContext *context;
    QGraphicsObject *obj;
    GpgME::GpgContext *mCtx;
};

#endif // KEYDETAILSWIDGET_H
