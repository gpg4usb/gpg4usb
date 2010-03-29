/*
 *      settingsdialog.cpp
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

#include <QWidget>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QLabel>
#include <QGridLayout>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QSettings>
#include <QApplication>
#include <QDir>
#include <QTranslator>
#include "settingsdialog.h"

SettingsDialog::SettingsDialog()
{
    setWindowTitle(tr("Settings"));
    resize(500, 200);
    setModal(true);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(applySettings()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    groupBox1 = new QGroupBox(tr("Options"));
    groupBox2 = new QGroupBox(tr("Action"));


	group1 = new QButtonGroup();	
	QRadioButton *iconSizeSmall = new QRadioButton(tr("small"));
	QRadioButton *iconSizeMedium =new QRadioButton(tr("medium"));
	QRadioButton *iconSizeLarge = new QRadioButton(tr("large"));
	group1->addButton(iconSizeSmall,1);
	group1->addButton(iconSizeMedium,2);
	group1->addButton(iconSizeLarge,3);

	group2 = new QButtonGroup();
	QRadioButton *iconTextButton = new QRadioButton(tr("just text"));
	QRadioButton *iconIconsButton =new QRadioButton(tr("just icons"));
	QRadioButton *iconAllButton = new QRadioButton(tr("text and icons"));
	group2->addButton(iconTextButton,1);
	group2->addButton(iconIconsButton,2);
	group2->addButton(iconAllButton,3);

	QHBoxLayout *iconSizeBox = new QHBoxLayout();
	iconSizeBox->addWidget(iconSizeSmall);
	iconSizeBox->addWidget(iconSizeMedium);
	iconSizeBox->addWidget(iconSizeLarge);

	QHBoxLayout *iconStyleBox = new QHBoxLayout();
	iconStyleBox->addWidget(iconTextButton);
	iconStyleBox->addWidget(iconIconsButton);
	iconStyleBox->addWidget(iconAllButton);

	groupBox2->setLayout(iconStyleBox);
	groupBox1->setLayout(iconSizeBox);
    
    /**/
    QGroupBox *langBox = new QGroupBox(tr("Language"));
    QHBoxLayout *hbox2 = new QHBoxLayout();
    QComboBox *langSelectBox = new QComboBox;
    
    QHash<QString, QString> lang = listLanguages();
    
    foreach(QString l , lang) {
        langSelectBox->addItem(l);
        //qDebug() << l;
    }
    
    hbox2->addWidget(langSelectBox);
    langBox->setLayout(hbox2);
    /**/
	
    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->addWidget(groupBox1);
	vbox->addWidget(groupBox2);
    vbox->addWidget(langBox);
    vbox->addWidget(buttonBox);
    setLayout(vbox);
    exec();
}

void SettingsDialog::applySettings()
{
	 QSettings settings;
     //settings.setValue("geometry", saveGeometry());

	switch (group1->checkedId()){
	case 1: settings.setValue("toolbar/iconsize", QSize(12, 12));
		break;
    case 2:settings.setValue("toolbar/iconsize", QSize(24, 24));
		break;
    case 3:settings.setValue("toolbar/iconsize", QSize(32, 32));
		break;
	}

	switch (group2->checkedId()){
	case 1: settings.setValue("toolbar/iconstyle", Qt::ToolButtonTextOnly);
		break;
    case 2:settings.setValue("toolbar/iconstyle", Qt::ToolButtonIconOnly);
		break;
    case 3:settings.setValue("toolbar/iconstyle", Qt::ToolButtonTextUnderIcon);
		break;
	}
	accept();
}

// http://www.informit.com/articles/article.aspx?p=1405555&seqNum=3
QHash<QString, QString> SettingsDialog::listLanguages() {
    
    // translate this String to language used, the language list gets 
    // filled from this
    QString(tr("English"));
    
    //QStringList languages;
    QHash<QString, QString> languages;
    
    QString appPath = qApp->applicationDirPath();
    QDir qmDir = QDir(appPath + "/ts/");
    QStringList fileNames =
            qmDir.entryList(QStringList("gpg4usb_*.qm"));

    for (int i = 0; i < fileNames.size(); ++i) {
        QString locale = fileNames[i];
        locale.remove(0, locale.indexOf('_') + 1);
        locale.chop(3);
        
        QTranslator translator;
        translator.load(fileNames[i], qmDir.absolutePath());
        QString language = translator.translate("SettingsDialog",
                                                "English");
        languages.insert(locale, language);
        
    }
    return languages;
}

