/*
 *      keydetailsdialog.h
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

#ifndef __KEYDETAILSDIALOG_H__
#define __KEYDETAILSDIALOG_H__

#include "context.h"
#include <gpgme.h>

QT_BEGIN_NAMESPACE
class QDateTime;
class QVBoxLayout;
class QHBoxLayout;
class QDialogButtonBox;
class QDialog;
class QGroupBox;
class QLabel;
class QGridLayout;
class QPushButton;
QT_END_NAMESPACE

class KeyDetailsDialog : public QDialog
{
    Q_OBJECT

public:
    KeyDetailsDialog(GpgME::Context* ctx, gpgme_key_t key, QWidget *parent = 0);
    static QString beautifyFingerprint(QString fingerprint);

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
    QLabel *keyidLabel;

    QLabel *nameVarLabel;
    QLabel *emailVarLabel;
    QLabel *commentVarLabel;
    QLabel *keySizeVarLabel;
    QLabel *expireVarLabel;
    QLabel *createdVarLabel;
    QLabel *algorithmVarLabel;
    QLabel *keyidVarLabel;

};

#endif // __KEYDETAILSDIALOG_H__
