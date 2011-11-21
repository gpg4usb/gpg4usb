/*
 *      importdetailsdialog.h
 *
 *      Copyright 2008 gpg4usb-team <gpg4usb@cpunk.de>
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

#ifndef __IMPORTDETAILSDIALOG_H__
#define __IMPORTDETAILSDIALOG_H__

#include "keylist.h"
#include <gpgme.h>

QT_BEGIN_NAMESPACE
class QGridLayout;
class QDialogButtonBox;
QT_END_NAMESPACE

class ImportDetailDialog : public QDialog
{
    Q_OBJECT

public:
    ImportDetailDialog(GpgME::GpgContext* ctx, KeyList* keyList, gpgme_import_result_t result, QWidget *parent = 0);

private:
    void createGeneralInfoBox();
    void createKeyInfoBox();

    GpgME::GpgContext *mCtx;
    KeyList *mKeyList;
    QGroupBox *generalInfoBox;
    QGroupBox *keyInfoBox;
    QDialogButtonBox *okButtonBox;
    QVBoxLayout *mvbox;
    QGridLayout *generalInfoBoxLayout;
    QGridLayout *keyInfoBoxLayout;
    gpgme_import_result_t mResult;
};

#endif // __IMPORTDETAILSDIALOG_H__
