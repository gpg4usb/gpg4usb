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

    // http://www.flickr.com/photos/laureenp/6141822934/
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/keys2.jpg"));
    setPixmap(QWizard::LogoPixmap, QPixmap(":/logo_small.png"));
    setPixmap(QWizard::BannerPixmap, QPixmap(":/banner.png"));

    QSettings settings;
    setStartId(settings.value("wizard/nextPage", -1).toInt());
    settings.remove("wizard/nextPage");

    connect(this, SIGNAL(accepted()), this, SLOT(wizardAccepted()));

}

void Wizard::wizardAccepted() {
    qDebug() << "noShow: " << field("showWizard").toBool();
    QSettings settings;
    // Don't show is mapped to show -> negation
    settings.setValue("wizard/showWizard", !field("showWizard").toBool());
}

IntroPage::IntroPage(QWidget *parent)
     : QWizardPage(parent)
{
    setTitle(tr("Introduction"));
    setSubTitle("bla");

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
    //setPixmap(QWizard::WatermarkPixmap, QPixmap(":/logo-flipped.png"));
    mCtx=ctx;
    mKeyMgmt=keyMgmt;
    setTitle(tr("Keyring Import"));
    setSubTitle("bla");

    QLabel *topLabel = new QLabel(tr("Import keys and/or settings from older gpg4usb. Just check, what you want to "
                                  "import, click the import button and choose the directory "
                                  "of your old gpg4usb in the appearing file dialog."), this);
    topLabel->setWordWrap(true);

    gpg4usbKeyCheckBox = new QCheckBox();
    gpg4usbKeyCheckBox->setChecked(true);
    QLabel *keyLabel = new QLabel(tr("Keys"));

    gpg4usbConfigCheckBox = new QCheckBox();
    gpg4usbConfigCheckBox->setChecked(true);
    QLabel *configLabel = new QLabel(tr("Configuration"));

    QWidget *importFromGpg4usbButtonBox = new QWidget(this);
    QHBoxLayout  *importFromGpg4usbButtonBoxLayout = new QHBoxLayout(importFromGpg4usbButtonBox);
    importFromGpg4usbButton = new QPushButton(tr("Import from older gpg4usb"));
    connect(importFromGpg4usbButton, SIGNAL(clicked()), this, SLOT(importKeysFromGpg4usb()));
    importFromGpg4usbButtonBox->setLayout(importFromGpg4usbButtonBoxLayout);

    QGridLayout *gpg4usbLayout = new QGridLayout();
    gpg4usbLayout->addWidget(topLabel,1,1,1,2);
    gpg4usbLayout->addWidget(gpg4usbKeyCheckBox,2,1,Qt::AlignRight);
    gpg4usbLayout->addWidget(keyLabel,2,2);
    gpg4usbLayout->addWidget(gpg4usbConfigCheckBox,3,1,Qt::AlignRight);
    gpg4usbLayout->addWidget(configLabel,3,2);
    gpg4usbLayout->addWidget(importFromGpg4usbButton,4,2);

    this->setLayout(gpg4usbLayout);
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
        QMessageBox::critical(0, tr("Import Error"), tr("Couldn't locate any keyring file in %1").arg(dir+"/keydb"));
        return false;
    }

    if (gpg4usbConfigCheckBox->isChecked()) {
        QSettings settings;
        settings.setValue("wizard/nextPage", this->nextId());

        // TODO: edit->maybesave?
        qApp->exit(RESTART_CODE);
    }
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
    //setPixmap(QWizard::WatermarkPixmap, QPixmap(":/logo-flipped.png"));
    mCtx=ctx;
    mKeyMgmt=keyMgmt;
    setTitle(tr("Key import from Gnupg"));
    setSubTitle("bla");

    QGridLayout *layout = new QGridLayout();
    gnupgLabel = new QLabel(tr("Should I try to import keys from a locally installed GnuPG?<br/> The location is read "
                               "from registry in Windows and assumed to be the .gnupg folder in the your home directory in Linux"));
    gnupgLabel->setWordWrap(true);
    layout->addWidget(gnupgLabel,1,1);

    QWidget *importFromGnupgButtonBox = new QWidget(this);
    QHBoxLayout  *importFromGnupgButtonBoxLayout = new QHBoxLayout(importFromGnupgButtonBox);
    importFromGnupgButton = new QPushButton(tr("Import keys from GnuPG"));
    connect(importFromGnupgButton, SIGNAL(clicked()), this, SLOT(importKeysFromGnupg()));
    importFromGnupgButtonBox->setLayout(importFromGnupgButtonBoxLayout);
    layout->addWidget(importFromGnupgButton,2,1);

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
    file.close();

    // try to import public keys
    QString pubRingFile = gnuPGHome+"/pubring.gpg";
    file.setFileName(pubRingFile);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(0, tr("Import error"), tr("Couldn't open public keyringfile: ") + pubRingFile);
        return false;
    }
    inBuffer.append(file.readAll());
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

    //setPixmap(QWizard::WatermarkPixmap, QPixmap(":/logo-flipped.png"));
    mCtx=ctx;
    setTitle(tr("Key-Generating"));
    setSubTitle("bla");
    topLabel = new QLabel(tr("First you've got to create an own keypair.<br/>"
                             "The pair contains a public and a private key.<br/>"
                             "Other users can use the public key to encrypt texts for you<br/>"
                             "and verify texts signed by you.<br/>"
                             "You can use the private key to decrypt and sign texts.<br/>"
                             "For more information have a look in the offline tutorial (which then is shown in the main window:"));
    QLabel *linkLabel = new QLabel("<a href=""docu_keygen.html#content"">"+tr("Offline tutorial")+"</a>");
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
    setSubTitle("bla");

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
