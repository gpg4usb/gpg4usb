/*
 *      keymgmt.h
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

#ifndef __KEYMGMT_H__
#define __KEYMGMT_H__

#include "keylist.h"
#include "keygenthread.h"
#include "keydetailsdialog.h"
#include "keyserverimportdialog.h"
#include <QtGui>

QT_BEGIN_NAMESPACE
class QMainWindow;
class QWidget;
class iostream;
class QtGui;
class QString;
class QFileDialog;
class QStringList;
class QIcon;
class QAction;
class QMenu;
class QApplication;
class QSlider;
QT_END_NAMESPACE

class KeyMgmt : public QMainWindow
{
    Q_OBJECT

public:
    KeyMgmt(GpgME::Context* ctx, QString iconpath);
    QAction *importKeyFromClipboardAct;
    QAction *importKeyFromFileAct;
    QAction *importKeyFromKeyServerAct;

public slots:
    void importKeyFromFile();
    void importKeyFromClipboard();
    void importKeyFromKeyServer();
    void exportKeyToFile();
    void exportKeyToClipboard();
    void deleteSelectedKeys();
    void deleteCheckedKeys();
    void generateKeyDialog();
    void expireBoxChanged();
    void passwordEditChanged();
    void showKeyDetails();

private slots:
    void keyGenAccept();


private:
    void createMenus();
    void createActions();
    void createToolBars();
    int checkPassWordStrength();
    void deleteKeysWithWarning(QStringList *uidList);

    KeyList *mKeyList;
    QString mIconPath;
    GpgME::Context *mCtx;
    QMenu *fileMenu;
    QMenu *keyMenu;
    QMenu *importKeyMenu;
    QDialog *genkeyDialog;
    QAction *exportKeyToFileAct;
    QAction *exportKeyToClipboardAct;
    QAction *deleteCheckedKeysAct;
    QAction *deleteSelectedKeysAct;
    QAction *generateKeyDialogAct;
    QAction *closeAct;
    QAction *showKeyDetailsAct;
    KeyGenThread *keyGenThread;
    QMessageBox msgbox;
    /**
     * Variables For Key-Generation
     */
    QLabel *nameLabel;
    QLabel *emailLabel;
    QLabel *commentLabel;
    QLabel *keySizeLabel;
    QLabel *passwordLabel;
    QLabel *repeatpwLabel;
    QLabel *errorLabel;
    QLabel *dateLabel;
    QLabel *expireLabel;
    QLabel *pwStrengthLabel;
    QLineEdit *nameEdit;
    QLineEdit *emailEdit;
    QLineEdit *commentEdit;
    QLineEdit *passwordEdit;
    QLineEdit *repeatpwEdit;
    QSpinBox *keySizeSpinBox;
    QDateTimeEdit *dateEdit;
    QCheckBox *expireCheckBox;
    QSlider *pwStrengthSlider;
    KeyServerImportDialog *importDialog;

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // __KEYMGMT_H__
