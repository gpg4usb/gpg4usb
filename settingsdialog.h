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


class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(QWidget *parent = 0);

private:
    QGroupBox *iconSizeBox;
    QGroupBox *iconStyleBox;
    QGroupBox *windowSizeBox;
    QGroupBox *saveCheckedKeysBox;
    QGroupBox *rememberPasswordBox;
    QGroupBox *mimeParseBox;
    QDialogButtonBox *buttonBox;

    QButtonGroup *iconSizeGroup;
    QButtonGroup *iconStyleGroup;

    QRadioButton *iconSizeSmall;
    QRadioButton *iconSizeMedium;
    QRadioButton *iconSizeLarge;
    QRadioButton *iconTextButton;
    QRadioButton *iconIconsButton;
    QRadioButton *iconAllButton;
    QCheckBox *windowSizeCheckBox;
    QCheckBox *saveCheckedKeysCheckBox;
    QCheckBox *rememberPasswordCheckBox;
    QCheckBox *mimeParseCheckBox;
    QCheckBox *mimeQPCheckBox;
    QCheckBox *mimeOpenAttachmentCheckBox;
    QComboBox *langSelectBox;
    QHash<QString, QString> lang;

    QHBoxLayout *iconSizeBoxLayout;
    QHBoxLayout *iconStyleBoxLayout;
    QHBoxLayout *windowSizeBoxLayout;
    QHBoxLayout *saveCheckedKeysBoxLayout;
    QHBoxLayout *rememberPasswordBoxLayout;
    QVBoxLayout *mimeParseBoxLayout;
    QVBoxLayout *vbox;
    void setSettings();


private slots:
    void applySettings();

    QHash<QString, QString> listLanguages();
};
#endif  // __SETTINGSDIALOG_H__
