/* *      wizard.cpp
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

Wizard::Wizard(GpgME::GpgContext *ctx, KeyMgmt *keyMgmt, QWidget *parent)
    : QWizard(parent)
{
    mCtx=ctx;
    mKeyMgmt=keyMgmt;
    mParent=parent;
    IntroPage *introPage = new IntroPage();
    KeyGenPage *keyGenPage = new KeyGenPage(mCtx);
    ImportPage *importPage = new ImportPage(mCtx,mKeyMgmt);
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
     setPixmap(QWizard::WatermarkPixmap, QPixmap(":/logo-flipped.png"));

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

    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/logo-flipped.png"));
    mCtx=ctx;
    setTitle(tr("Key-Generating"));
    topLabel = new QLabel(tr("First you've got to create an own keypair.<br/>"
                             "The pair contains a public and a private key.<br/>"
                             "Other users can use the public key to encrypt texts for you<br/>"
                             "and verify texts signed by you.<br/>"
                             "You can use the private key to decrypt and sign texts.<br/>"
                             "For more information have a look in the online tutorial:"));
    QLabel *linkLabel = new QLabel("<a href=""http://gpg4usb.cpunk.de/docu.html"">"+tr("Online tutorial")+"</a>");
    linkLabel->setOpenExternalLinks(true);

    QWidget *createKeyButtonBox = new QWidget(this);
    QHBoxLayout  *createKeyButtonBoxLayout = new QHBoxLayout(createKeyButtonBox);
    createKeyButton = new QPushButton(tr("Create New Key"));
    createKeyButtonBoxLayout->addWidget(createKeyButton);
    createKeyButtonBoxLayout->addStretch(1);
    layout = new QVBoxLayout();
    layout->addWidget(topLabel);
    layout->addWidget(linkLabel);
    layout->addWidget(createKeyButtonBox);
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

ImportPage::ImportPage(GpgME::GpgContext *ctx, KeyMgmt *keyMgmt, QWidget *parent)
     : QWizardPage(parent)
{
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/logo-flipped.png"));
    mCtx=ctx;
    mKeyMgmt=keyMgmt;
    setTitle(tr("Keyring Import"));
    QGroupBox *gnupgBox = new QGroupBox(tr("Import from GnuPG"), this);

    QGridLayout *gnupgLayout = new QGridLayout();
    gnupgLabel = new QLabel(tr("Should I try to import keys from GnuPG?"));
    gnupgLayout->addWidget(gnupgLabel,1,1,1,2);

    gnupgPrivKeyCheckBox = new QCheckBox();
    gnupgPrivKeyCheckBox->setChecked(true);
    gnupgLayout->addWidget(gnupgPrivKeyCheckBox,2,1,Qt::AlignRight);
    QLabel *privateKeyLabel = new QLabel(tr("Private Keys"));
    gnupgLayout->addWidget(privateKeyLabel,2,2);

    gnupgpPubKeyCheckBox = new QCheckBox();
    gnupgpPubKeyCheckBox->setChecked(true);
    gnupgLayout->addWidget(gnupgpPubKeyCheckBox,3,1,Qt::AlignRight);
    QLabel *gnupgPrivKeyLabel = new QLabel(tr("Public Keys"));
    gnupgLayout->addWidget(gnupgPrivKeyLabel,3,2);


    QWidget *importFromGnupgButtonBox = new QWidget(this);
    QHBoxLayout  *importFromGnupgButtonBoxLayout = new QHBoxLayout(importFromGnupgButtonBox);
    importFromGnupgButton = new QPushButton(tr("Import keys from GnuPG"));
    connect(importFromGnupgButton, SIGNAL(clicked()), this, SLOT(importKeysFromGnupg()));
    importFromGnupgButtonBox->setLayout(importFromGnupgButtonBoxLayout);
    gnupgLayout->addWidget(importFromGnupgButton,2,3);

    gnupgBox->setLayout(gnupgLayout);

    QGroupBox *gpg4usbBox = new QGroupBox(tr("Import from older gpg4usb"), this);

    QGridLayout *gpg4usbLayout = new QGridLayout();
    QLabel *gnupgLabel = new QLabel(tr("Point to the folder of last gpg4usb"));
    gpg4usbLayout->addWidget(gnupgLabel,1,1,1,2);

    gpg4usbPrivKeyCheckBox = new QCheckBox();
    gpg4usbPrivKeyCheckBox->setChecked(true);
    gpg4usbLayout->addWidget(gpg4usbPrivKeyCheckBox,2,1,Qt::AlignRight);
    QLabel *privateKeyLabel2 = new QLabel(tr("Private Keys"));
    gpg4usbLayout->addWidget(privateKeyLabel2,2,2);

    gpg4usbPubKeyCheckBox = new QCheckBox();
    gpg4usbPubKeyCheckBox->setChecked(true);
    gpg4usbLayout->addWidget(gpg4usbPubKeyCheckBox,3,1,Qt::AlignRight);
    QLabel *gpg4usbLabel = new QLabel(tr("Public Keys"));
    gpg4usbLayout->addWidget(gpg4usbLabel,3,2);

    QWidget *importFromGpg4usbButtonBox = new QWidget(this);
    QHBoxLayout  *importFromGpg4usbButtonBoxLayout = new QHBoxLayout(importFromGpg4usbButtonBox);
    importFromGpg4usbButton = new QPushButton(tr("Import keys from gpg4usb"));
    connect(importFromGpg4usbButton, SIGNAL(clicked()), this, SLOT(importKeysFromGpg4usb()));
    importFromGpg4usbButtonBox->setLayout(importFromGpg4usbButtonBoxLayout);
    gpg4usbLayout->addWidget(importFromGpg4usbButton,2,3);

    gpg4usbBox->setLayout(gpg4usbLayout);

    layout = new QVBoxLayout();
    layout->addWidget(gnupgBox);
    layout->addWidget(gpg4usbBox);

    setLayout(layout);
}

bool ImportPage::importKeysFromGpg4usb()
{
    QString dir = QFileDialog::getExistingDirectory(this,tr("Old gpg4usb directory"));

    // Return, if cancel was hit
    if (dir.isEmpty()) {
        return false;
    }

    importConfFromGpg4usb(dir);

    QFile secRing(dir+"/keydb/secring.gpg");
    QFile pubRing(dir+"/keydb/pubring.gpg");

    qDebug() << pubRing.fileName();
    // Return, if no keyrings are found in subdir of chosen dir
    if (!(pubRing.exists() or secRing.exists())) {
        QMessageBox::critical(0, tr("Import Error"), tr("Couldn't locate any keyring file in choosen directory"));
        return false;
    }

    if (pubRing.exists() and gnupgpPubKeyCheckBox->isChecked()) {
        if (!pubRing.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(0, tr("Import error"), tr("Couldn't open public keyringfile: ") + pubRing.fileName());
            return false;
        }
        QByteArray inBuffer = pubRing.readAll();
        mKeyMgmt->importKeys(inBuffer);
    }

    if (secRing.exists() and gnupgPrivKeyCheckBox->isChecked()) {
        if (!secRing.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(0, tr("Import error"), tr("Couldn't open private keyringfile: ") + secRing.fileName());
            return false;
        }
        QByteArray inBuffer = secRing.readAll();
        mKeyMgmt->importKeys(inBuffer);
    }

    qApp->exit();
    return true;
}

bool ImportPage::importConfFromGpg4usb(QString dir) {
    QString path = dir+"/conf/gpg4usb.ini";
    qDebug() << "import old conf from: " << path;
    QSettings oldconf(path, QSettings::IniFormat, this);
    QSettings actualConf;
    foreach(QString key, oldconf.allKeys()) {
        qDebug() << key << ": " << oldconf.value(key);
        actualConf.setValue(key, oldconf.value(key));
    }

}

bool ImportPage::importKeysFromGnupg()
{
    // first get gnupghomedir and check, if it exists
    QString gnuPGHome = getGnuPGHome();
    if (gnuPGHome == NULL) {
        QMessageBox::critical(0, tr("Import Error"), tr("Couldn't locate GnuPG home directory"));
        return false;
    }

    // try to import private files, if private key checkbox is checked
    if (gnupgPrivKeyCheckBox->isChecked()) {
        QString privRingFile = gnuPGHome+"/secring.gpg";
        QFile file;
        file.setFileName(privRingFile);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(0, tr("Import error"), tr("Couldn't open private keyringfile: ") + privRingFile);
            return false;
        }
        QByteArray inBuffer = file.readAll();

        mKeyMgmt->importKeys(inBuffer);
    }

    // try to import public keys, if public checkbox is checked
    if (gnupgpPubKeyCheckBox->isChecked()) {
        QString pubRingFile = gnuPGHome+"/pubring.gpg";
        QFile file;
        file.setFileName(pubRingFile);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(0, tr("Import error"), tr("Couldn't open public keyringfile: ") + pubRingFile);
            return false;
        }
        QByteArray inBuffer = file.readAll();

        mKeyMgmt->importKeys(inBuffer);
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
