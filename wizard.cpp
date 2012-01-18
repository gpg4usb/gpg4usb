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
    setPage(Page_Intro,new IntroPage(this));
    setPage(Page_ImportFromGpg4usb,new ImportFromGpg4usbPage(mCtx, mKeyMgmt, this));
    setPage(Page_ImportFromGnupg,new ImportFromGnupgPage(mCtx, mKeyMgmt, this));
    setPage(Page_GenKey,new KeyGenPage(mCtx, this));
    setPage(Page_Conclusion,new ConclusionPage(this));
    qDebug() << Page_Intro;
#ifndef Q_WS_MAC
    setWizardStyle(ModernStyle);
#endif
    setWindowTitle(tr("First Start Wizard"));

    QSettings settings;
    setStartId(settings.value("wizard/page", -1).toInt());
    settings.remove("wizard/page");

    connect(this, SIGNAL(accepted()), this, SLOT(wizardAccepted()));

}

void Wizard::wizardAccepted() {
    qDebug() << "noShow: " << field("showWizard").toBool();
    QSettings settings;
    settings.setValue("wizard/showWizard", field("showWizard").toBool());
}

IntroPage::IntroPage(QWidget *parent)
     : QWizardPage(parent)
{
    setTitle(tr("Introduction"));
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/logo-flipped.png"));

    topLabel = new QLabel(tr("This wizard will help you getting started by importing settings and keys"
                             "from an older version of gpg4usb, import keys from a locally installed Gnupg or to "
                             "generate a new key to encrypt and decrypt."));
    topLabel->setWordWrap(true);

    // QComboBox for language selection
    langLabel = new QLabel(tr("Choose a Language"));
    langLabel->setWordWrap(true);

    languages = SettingsDialog::listLanguages();
    langSelectBox = new QComboBox();
    foreach(QString l, languages) {
        langSelectBox->addItem(l);
    }
    // selected entry from config
    QSettings settings;
    QString langKey = settings.value("int/lang").toString();
    QString langValue = languages.value(langKey);
    if (langKey != "") {
        langSelectBox->setCurrentIndex(langSelectBox->findText(langValue));
    }

    connect(langSelectBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(langChange(QString)));

    // set layout and add widgets
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    layout->addWidget(langLabel);
    layout->addWidget(langSelectBox);
    setLayout(layout);
    //this->setFinalPage(true);
}

void IntroPage::langChange(QString lang) {
    QSettings settings;
    settings.setValue("int/lang", languages.key(lang));
    settings.setValue("wizard/nextPage", this->wizard()->currentId());
    qApp->exit(RESTART_CODE);
}

int IntroPage::nextId() const
{
    return Wizard::Page_ImportFromGpg4usb;
}

ImportFromGpg4usbPage::ImportFromGpg4usbPage(GpgME::GpgContext *ctx, KeyMgmt *keyMgmt, QWidget *parent)
     : QWizardPage(parent)
{
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/logo-flipped.png"));
    mCtx=ctx;
    mKeyMgmt=keyMgmt;
    setTitle(tr("Keyring Import"));

    QGroupBox *gpg4usbBox = new QGroupBox(tr("Import keys and/or settings from older gpg4usb"), this);

    QGridLayout *gpg4usbLayout = new QGridLayout();
    QLabel *gnupgLabel = new QLabel(tr("Point to the folder of last gpg4usb"));
    gpg4usbLayout->addWidget(gnupgLabel,1,3);

    gpg4usbKeyCheckBox = new QCheckBox();
    gpg4usbKeyCheckBox->setChecked(true);
    gpg4usbLayout->addWidget(gpg4usbKeyCheckBox,2,1,Qt::AlignRight);
    QLabel *privateKeyLabel2 = new QLabel(tr("Keys"));
    gpg4usbLayout->addWidget(privateKeyLabel2,2,2);

    gpg4usbConfigCheckBox = new QCheckBox();
    gpg4usbConfigCheckBox->setChecked(true);
    gpg4usbLayout->addWidget(gpg4usbConfigCheckBox,3,1,Qt::AlignRight);
    QLabel *gpg4usbLabel = new QLabel(tr("Configuration"));
    gpg4usbLayout->addWidget(gpg4usbLabel,3,2);

    QWidget *importFromGpg4usbButtonBox = new QWidget(this);
    QHBoxLayout  *importFromGpg4usbButtonBoxLayout = new QHBoxLayout(importFromGpg4usbButtonBox);
    importFromGpg4usbButton = new QPushButton(tr("Import from older gpg4usb"));
    connect(importFromGpg4usbButton, SIGNAL(clicked()), this, SLOT(importKeysFromGpg4usb()));
    importFromGpg4usbButtonBox->setLayout(importFromGpg4usbButtonBoxLayout);
    gpg4usbLayout->addWidget(importFromGpg4usbButton,2,3);

    gpg4usbBox->setLayout(gpg4usbLayout);

    layout = new QVBoxLayout();
    layout->addWidget(gpg4usbBox);

    this->setLayout(layout);
}

bool ImportFromGpg4usbPage::importKeysFromGpg4usb()
{
    QString dir = QFileDialog::getExistingDirectory(this,tr("Old gpg4usb directory"));

    // Return, if cancel was hit
    if (dir.isEmpty()) {
        return false;
    }

    importConfFromGpg4usb(dir);

    QFile secRing(dir+"/keydb/secring.gpg");
    QFile pubRing(dir+"/keydb/pubring.gpg");

    // Return, if no keyrings are found in subdir of chosen dir
    if (!(pubRing.exists() or secRing.exists())) {
        QMessageBox::critical(0, tr("Import Error"), tr("Couldn't locate any keyring file in choosen directory"));
        return false;
    }

    QSettings settings;
    settings.setValue("wizard/nextPage", this->nextId());

    // TODO: edit->maybesave?
    qApp->exit(RESTART_CODE);
    return true;
}

bool ImportFromGpg4usbPage::importConfFromGpg4usb(QString dir) {
    QString path = dir+"/conf/gpg4usb.ini";
    QSettings oldconf(path, QSettings::IniFormat, this);
    QSettings actualConf;
    foreach(QString key, oldconf.allKeys()) {
        actualConf.setValue(key, oldconf.value(key));
    }
    return true;
}

int ImportFromGpg4usbPage::nextId() const
{
    return Wizard::Page_ImportFromGnupg;
}

ImportFromGnupgPage::ImportFromGnupgPage(GpgME::GpgContext *ctx, KeyMgmt *keyMgmt, QWidget *parent)
     : QWizardPage(parent)
{
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/logo-flipped.png"));
    mCtx=ctx;
    mKeyMgmt=keyMgmt;
    setTitle(tr("Keyring Import"));
    QGroupBox *gnupgBox = new QGroupBox(tr("Import from GnuPG"), this);

    QGridLayout *gnupgLayout = new QGridLayout();
    gnupgLabel = new QLabel(tr("Should I try to import keys from a locally installed GnuPG?"));
    gnupgLayout->addWidget(gnupgLabel,1,1);

    QWidget *importFromGnupgButtonBox = new QWidget(this);
    QHBoxLayout  *importFromGnupgButtonBoxLayout = new QHBoxLayout(importFromGnupgButtonBox);
    importFromGnupgButton = new QPushButton(tr("Import keys from GnuPG"));
    connect(importFromGnupgButton, SIGNAL(clicked()), this, SLOT(importKeysFromGnupg()));
    importFromGnupgButtonBox->setLayout(importFromGnupgButtonBoxLayout);
    gnupgLayout->addWidget(importFromGnupgButton,2,1);

    gnupgBox->setLayout(gnupgLayout);

    layout = new QVBoxLayout();
    layout->addWidget(gnupgBox);

    this->setLayout(layout);
}

bool ImportFromGnupgPage::importKeysFromGnupg()
{
    // first get gnupghomedir and check, if it exists
    QString gnuPGHome = getGnuPGHome();
    if (gnuPGHome == NULL) {
        QMessageBox::critical(0, tr("Import Error"), tr("Couldn't locate GnuPG home directory"));
        return false;
    }

    // try to import private files
    QString privRingFile = gnuPGHome+"/secring.gpg";
    QFile file;
    file.setFileName(privRingFile);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(0, tr("Import error"), tr("Couldn't open private keyringfile: ") + privRingFile);
        return false;
    }
    QByteArray inBuffer = file.readAll();
    mKeyMgmt->importKeys(inBuffer);
    inBuffer.clear();
    file.close();

    // try to import public keys
    QString pubRingFile = gnuPGHome+"/pubring.gpg";
    file.setFileName(pubRingFile);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(0, tr("Import error"), tr("Couldn't open public keyringfile: ") + pubRingFile);
        return false;
    }
    inBuffer = file.readAll();
    mKeyMgmt->importKeys(inBuffer);
    inBuffer.clear();
    file.close();

    return true;
}

QString ImportFromGnupgPage::getGnuPGHome()
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

int ImportFromGnupgPage::nextId() const
{
    return Wizard::Page_GenKey;
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
    QLabel *linkLabel = new QLabel("<a href=""docu_keygen.html#content"">"+tr("Online tutorial")+"</a>");
    //linkLabel->setOpenExternalLinks(true);

    connect(linkLabel, SIGNAL(linkActivated(const QString&)), parentWidget()->parentWidget(), SLOT(openHelp(const QString&)));

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
    return Wizard::Page_Conclusion;
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

ConclusionPage::ConclusionPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Finish Start Wizard"));

    bottomLabel = new QLabel(tr("You're ready to encrypt and decrpt now."));
    bottomLabel->setWordWrap(true);

    dontShowWizardCheckBox = new QCheckBox(tr("Dont show the wizard again."));
    dontShowWizardCheckBox->setChecked(Qt::Checked);

    registerField("showWizard", dontShowWizardCheckBox);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(bottomLabel);
    layout->addWidget(dontShowWizardCheckBox);
    setLayout(layout);
    setVisible(true);
}

int ConclusionPage::nextId() const
{
    return -1;
}
