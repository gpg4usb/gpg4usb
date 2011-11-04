/*
 *      wizard.cpp
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

#include <QtGui>

 #include "wizard.h"

Wizard::Wizard(GpgME::GpgContext *ctx, QWidget *parent)
    : QWizard(parent)
{
    mCtx=ctx;
    IntroPage *introPage = new IntroPage();
    KeyGenPage *keyGenPage = new KeyGenPage(mCtx);
    ImportPage *importPage = new ImportPage(mCtx);
    ConclusionPage *conclusionPage = new ConclusionPage();
    addPage(introPage);
    addPage(keyGenPage);
    addPage(importPage);
    addPage(conclusionPage);

#ifndef Q_WS_MAC
    setWizardStyle(ModernStyle);
#endif

    setWindowTitle(tr("First Start Wizard"));
}

IntroPage::IntroPage(QWidget *parent)
     : QWizardPage(parent)
 {
     setTitle(tr("Introduction"));

     topLabel = new QLabel(tr("This wizard will help you getting started with encrypting and decrypting."));
     topLabel->setWordWrap(true);

     QVBoxLayout *layout = new QVBoxLayout;
     layout->addWidget(topLabel);
     setLayout(layout);
 }

int IntroPage::nextId() const
{
    return 1;
}

KeyGenPage::KeyGenPage(GpgME::GpgContext *ctx, QWidget *parent)
     : QWizardPage(parent)
{
    mCtx=ctx;
    setTitle(tr("Key-Generating"));
    topLabel = new QLabel(tr("First you've got to create an own key."));
    createKeyButton = new QPushButton(tr("Create New Key"));
    layout = new QVBoxLayout();
    layout->addWidget(topLabel);
    layout->addWidget(createKeyButton);
    connect(createKeyButton, SIGNAL(clicked()), this, SLOT(generateKeyDialog()));

    setLayout(layout);
}

int KeyGenPage::nextId() const
{
    return 2;
}

void KeyGenPage::generateKeyDialog()
{
    KeyGenDialog *keyGenDialog = new KeyGenDialog(mCtx, this);
    connect(mCtx, SIGNAL(keyDBChanged()), this, SLOT(showKeyGeneratedMessage()));
    keyGenDialog->show();
}

void KeyGenPage::showKeyGeneratedMessage()
{
    layout->addWidget(new QLabel(tr("key generated. Now you can crypt and sign texts.")));
}

ImportPage::ImportPage(GpgME::GpgContext *ctx, QWidget *parent)
     : QWizardPage(parent)
{
    mCtx=ctx;
    setTitle(tr("Keyring Import from GnuPG-home-directory"));
    topLabel = new QLabel(tr("Should I try to import keys from gnupg?"));

    // Layout for private keys
    privateKeysCheckBox = new QCheckBox();
    QLabel *privateKeysLabel = new QLabel(tr("Private keys"));
    QWidget *privHBox = new QWidget(this);
    QHBoxLayout *privHBoxLayout = new QHBoxLayout();
    privHBoxLayout->addWidget(privateKeysCheckBox);
    privHBoxLayout->addWidget(privateKeysLabel);
    privHBox->setLayout(privHBoxLayout);

    // Layout for public keys
    publicKeysCheckBox = new QCheckBox();
    QLabel *publicKeysLabel = new QLabel(tr("Public keys"));
    QWidget *pubHBox = new QWidget();
    QHBoxLayout *pubHBoxLayout = new QHBoxLayout();
    pubHBoxLayout->addWidget(publicKeysCheckBox);
    pubHBoxLayout->addWidget(publicKeysLabel);
    pubHBox->setLayout(pubHBoxLayout);

    importKeyButton = new QPushButton(tr("Import keys"));
    layout = new QVBoxLayout();
    layout->addWidget(topLabel);
    layout->addWidget(privHBox);
    layout->addWidget(pubHBox);
    layout->addWidget(importKeyButton);
    connect(importKeyButton, SIGNAL(clicked()), this, SLOT(importKeys()));
    setLayout(layout);
}

bool ImportPage::importKeys()
{
    // first get gnupghomedir and check, if it exists
    QString gnuPGHome = getGnuPGHome();
    if (gnuPGHome == NULL) {
        qDebug() << "couldn't locate gnupg-Homedirectory";
        return false;
    }

    // try to import private files, if private key checkbox is checked
    if (privateKeysCheckBox->isChecked()) {
        QString privRingFile = gnuPGHome+"/secring.gpg";
        QFile file;
        file.setFileName(privRingFile);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << tr("Couldn't Open Private Keyringfile: ") + privRingFile;
        }
        QByteArray inBuffer = file.readAll();

        mCtx->importKey(inBuffer);
    }

    // try to import public keys, if public checkbox is checked
    if (publicKeysCheckBox->isChecked()) {
        QString pubRingFile = gnuPGHome+"/pubring.gpg";
        QFile file;
        file.setFileName(pubRingFile);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << tr("Couldn't Open Public Keyringfile: ") + pubRingFile;
        }
        QByteArray inBuffer = file.readAll();

        mCtx->importKey(inBuffer);
    }

    return true;
}

QString ImportPage::getGnuPGHome()
{
    QString gnuPGHome="";
    #ifdef _WIN32
        QSettings gnuPGsettings("HKEY_CURRENT_USER\\Software\\GNU\\GNUPG", QSettings::NativeFormat);
        gnuPGHome = gnuPGsettings.value("HomeDir").toString();
        if (gnuPGHome.isEmpty()) {
            return NULL;
        }

    #else
        gnuPGHome=QDir::homePath()+"/.gnupg";
        if (! QFile(gnuPGHome).exists()) {
            return NULL;
        }
    #endif

    return gnuPGHome;
}

int ImportPage::nextId() const
{
    return 3;
}

ConclusionPage::ConclusionPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Finish Start Wizard"));

    bottomLabel = new QLabel(tr("You're ready to encrypt and decrpt now."));
    bottomLabel->setWordWrap(true);

    showWizardCheckBox = new QCheckBox(tr("Dont show the wizard again."));
    showWizardCheckBox->setChecked(Qt::Checked);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(bottomLabel);
    layout->addWidget(showWizardCheckBox);
    setLayout(layout);
    setVisible(true);
}

int ConclusionPage::nextId() const
{
    if (showWizardCheckBox->isChecked())
    {
        QSettings settings;
        settings.setValue("wizard/showWizard", false);
    }
    return -1;
}
