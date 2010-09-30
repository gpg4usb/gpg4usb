/*
 *      fileencryptiondialog.h
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

#ifndef __FILEENCRYPTIONDIALOG_H__
#define __FILEENCRYPTIONDIALOG_H__

#include <QDialog>
#include <QLineEdit>
#include <QRadioButton>

#include "context.h"
#include "keylist.h"

class FileEncryptionDialog : public QDialog
{
    Q_OBJECT

public:
    FileEncryptionDialog(GpgME::Context *ctx, QString iconPath, QStringList keyList , QWidget *parent = 0);

public slots:
    void selectInputFile();
    void selectOutputFile();
    void executeAction();
    void hideKeyList();
    void showKeyList();

private:
    QLineEdit *outputFileEdit;
    QLineEdit *inputFileEdit;
    QRadioButton *radioEnc;
    QRadioButton *radioDec;

protected:
    GpgME::Context *mCtx;
    KeyList *mKeyList;

};
#endif // __FILEENCRYPTIONDIALOG_H__
