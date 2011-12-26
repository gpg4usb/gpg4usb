/*
 *      wizard.h
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


#ifndef WIZARD_H
#define WIZARD_H

#include <QWizard>
#include "keygendialog.h"
#include "keymgmt.h"

class QCheckBox;
class QLabel;
class QLineEdit;
class QRadioButton;

class Wizard : public QWizard
{
    Q_OBJECT

public:
    Wizard(GpgME::GpgContext *ctx, KeyMgmt *keyMgmt, QWidget *parent = 0);

private:
    GpgME::GpgContext *mCtx;
    KeyMgmt *mKeyMgmt;
};

class IntroPage : public QWizardPage
{
    Q_OBJECT

public:
    IntroPage(QWidget *parent = 0);
    QLabel *topLabel;

    int nextId() const;
};

class KeyGenPage : public QWizardPage
{
    Q_OBJECT

public:
    KeyGenPage(GpgME::GpgContext *ctx, QWidget *parent = 0);
    int nextId() const;

private slots:
    void generateKeyDialog();
    void showKeyGeneratedMessage();

private:
    QLabel *topLabel;
    QPushButton *createKeyButton;
    GpgME::GpgContext *mCtx;
    QVBoxLayout *layout;
};

class ImportPage : public QWizardPage
{
    Q_OBJECT

public:
    ImportPage(GpgME::GpgContext *ctx, KeyMgmt *keyMgmt, QWidget *parent = 0);

private slots:
    /**
      * @details  Import keys from gnupg-homedir, private or/and public depend on the checked boxes
      */
    bool importKeysFromGnupg();
    bool importKeysFromGpg4usb();

private:
    KeyMgmt *mKeyMgmt;
    int nextId() const;

    /**
      * @details  String containing the gnupg-homedir
      * @returns String containg the gnupg-homedir, but NULL, if the in windows registry entry
      * doesn't exist or in linux ~/.gnupg doesn't exist
      */
    QString getGnuPGHome();

    QLabel *gnupgLabel;
    QVBoxLayout *layout;
    GpgME::GpgContext *mCtx;
    QCheckBox *gnupgPrivKeyCheckBox;
    QCheckBox *gnupgpPubKeyCheckBox;
    QCheckBox *gpg4usbPrivKeyCheckBox;
    QCheckBox *gpg4usbPubKeyCheckBox;
    QPushButton *importFromGnupgButton;
    QPushButton *importFromGpg4usbButton;
};

class ConclusionPage : public QWizardPage
{
    Q_OBJECT

public:
    ConclusionPage(QWidget *parent = 0);
    int nextId() const;

private:
    QLabel *bottomLabel;
    QCheckBox *showWizardCheckBox;
};

#endif
