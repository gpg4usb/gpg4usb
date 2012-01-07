/*
 *      fileencryptiondialog.h
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

#ifndef __FILEENCRYPTIONDIALOG_H__
#define __FILEENCRYPTIONDIALOG_H__

#include "gpgcontext.h"
#include "keylist.h"

QT_BEGIN_NAMESPACE
class QDialog;
class QLineEdit;
class QWidget;
class QDialogButtonBox;
class QLabel;
class QPushButton;
class QHBoxLayout;
class QVBoxLayout;
class QDebug;
class QFileDialog;
class QRadioButton;
QT_END_NAMESPACE

/**
 * @brief
 *
 * @class FileEncryptionDialog fileencryptiondialog.h "fileencryptiondialog.h"
 */
class FileEncryptionDialog : public QDialog
{
    Q_OBJECT

public:

    enum DialogAction {
        Encrypt,
        Decrypt,
        Both
    };

    /**
     * @brief
     *
     * @fn FileEncryptionDialog
     * @param ctx
     * @param iconPath
     * @param keyList
     * @param parent
     */
    FileEncryptionDialog(GpgME::GpgContext *ctx, QString iconPath, QStringList keyList, QWidget *parent = 0, DialogAction action = Both);

public slots:
    /**
     * @details
     *
     * @fn selectInputFile
     */
    void selectInputFile();
    /**
     * @brief
     *
     * @fn selectOutputFile
     */
    void selectOutputFile();
    /**
     * @brief
     *
     * @fn executeAction
     */
    void executeAction();
    /**
     * @brief
     *
     * @fn hideKeyList
     */
    void hideKeyList();
    /**
     * @brief
     *
     * @fn showKeyList
     */
    void showKeyList();

private:
    QLineEdit *outputFileEdit; /**< TODO */
    QLineEdit *inputFileEdit; /**< TODO */
    QRadioButton *radioEnc; /**< TODO */
    QRadioButton *radioDec; /**< TODO */
    DialogAction mAction; /**< TODO */
protected:
    GpgME::GpgContext *mCtx; /**< TODO */
    KeyList *mKeyList; /**< TODO */

};
#endif // __FILEENCRYPTIONDIALOG_H__
