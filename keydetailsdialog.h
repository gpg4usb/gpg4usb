/*
 *      keydetailsdialog.h
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

#ifndef __KEYDETAILSDIALOG_H__
#define __KEYDETAILSDIALOG_H__

#include "context.h"
#include <gpgme.h>

class QDateTime;
class QVBoxLayout;
class QDialogButtonBox;
class QDialog;
class QGroupBox;
class QLabel;
class QGridLayout;
class QPushButton;

class KeyDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    KeyDetailsDialog(GpgME::Context* ctx, gpgme_key_t key);

private slots:
    void exportPrivateKey();

private:
    QString *keyid;
    GpgME::Context *mCtx;

    QGroupBox *ownerBox;
    QGroupBox *keyBox;
    QGroupBox *fingerprintBox;
    QDialogButtonBox *buttonBox;

    QVBoxLayout *mvbox;
    QGridLayout *vboxKD;
    QGridLayout *vboxOD;
    QVBoxLayout *vboxFP;

    QLabel *nameLabel;
    QLabel *emailLabel;
    QLabel *commentLabel;
    QLabel *keySizeLabel;
    QLabel *expireLabel;
    QLabel *createdLabel;
    QLabel *algorithmLabel;
    QLabel *fingerPrintLabel;

    QLabel *nameVarLabel;
    QLabel *emailVarLabel;
    QLabel *commentVarLabel;
    QLabel *keySizeVarLabel;
    QLabel *expireVarLabel;
    QLabel *createdVarLabel;
    QLabel *algorithmVarLabel;


    QString beautifyFingerprint(QString fingerprint);
};

#endif // __KEYDETAILSDIALOG_H__
