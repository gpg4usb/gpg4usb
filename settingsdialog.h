/*
 *      settingsdialog.h
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

#ifndef __SETTINGSDIALOG_H__
#define __SETTINGSDIALOG_H__

#include <QDialog>
#include <QButtonGroup>
#include <QGroupBox>
#include <QRadioButton>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QHash>

 class QFileInfo;
 class QTabWidget;

 class GeneralTab : public QWidget
 {
     Q_OBJECT

 public:
     GeneralTab(QWidget *parent = 0);
     void setSettings();
     void applySettings();

 private:
     QCheckBox *rememberPasswordCheckBox;
     QCheckBox *saveCheckedKeysCheckBox;
     QComboBox *langSelectBox;
     QHash<QString, QString> lang;

private slots:
    QHash<QString, QString> listLanguages();

 };

 class MimeTab : public QWidget
 {
     Q_OBJECT

 public:
     MimeTab(QWidget *parent = 0);
     void setSettings();
     void applySettings();

 private:
     QCheckBox *mimeParseCheckBox;
     QCheckBox *mimeQPCheckBox;
     QCheckBox *mimeOpenAttachmentCheckBox;
 };

 class AppearanceTab : public QWidget
 {
     Q_OBJECT

 public:
     //void setSettings();
     AppearanceTab(QWidget *parent = 0);
     void setSettings();
     void applySettings();

 private:
     QButtonGroup *iconStyleGroup;
     QRadioButton *iconSizeSmall;
     QRadioButton *iconSizeMedium;
     QRadioButton *iconSizeLarge;
     QButtonGroup *iconSizeGroup;
     QRadioButton *iconTextButton;
     QRadioButton *iconIconsButton;
     QRadioButton *iconAllButton;
     QCheckBox *windowSizeCheckBox;
 };


class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(QWidget *parent = 0);
    GeneralTab *generalTab;
    MimeTab *mimeTab;
    AppearanceTab *appearanceTab;

public slots:
    void accept();

private:
    QTabWidget *tabWidget;
    QDialogButtonBox *buttonBox;
    QVBoxLayout *vbox;
};
#endif  // __SETTINGSDIALOG_H__
