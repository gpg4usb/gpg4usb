/*
 *      keygendialog.h
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

#ifndef __KEYGENDIALOG_H__
#define __KEYGENDIALOG_H__

#include "keygenthread.h"
#include "gpgcontext.h"
#include <QtGui>

QT_BEGIN_NAMESPACE
class QDateTime;
class QDialogButtonBox;
class QDialog;
class QGridLayout;
QT_END_NAMESPACE

class KeyGenDialog : public QDialog
{
    Q_OBJECT

public:
    KeyGenDialog(GpgME::GpgContext* ctx, QWidget *parent = 0);

private:
    void generateKeyDialog();
    int checkPassWordStrength();

    GpgME::GpgContext *mCtx;
    KeyGenThread *keyGenThread;
    QStringList errorMessages;
    QDialogButtonBox *buttonBox;
    QLabel *nameLabel;
    QLabel *emailLabel;
    QLabel *commentLabel;
    QLabel *keySizeLabel;
    QLabel *passwordLabel;
    QLabel *repeatpwLabel;
    QLabel *errorLabel;
    QLabel *dateLabel;
    QLabel *expireLabel;
    QLabel *pwStrengthLabel;
    QLineEdit *nameEdit;
    QLineEdit *emailEdit;
    QLineEdit *commentEdit;
    QLineEdit *passwordEdit;
    QLineEdit *repeatpwEdit;
    QSpinBox *keySizeSpinBox;
    QDateTimeEdit *dateEdit;
    QCheckBox *expireCheckBox;
    QSlider *pwStrengthSlider;

private slots:
    void expireBoxChanged();
    void passwordEditChanged();
    void keyGenAccept();

};
#endif // __KEYGENDIALOG_H__
