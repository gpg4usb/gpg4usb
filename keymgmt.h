/*
 *      keymgmt.h
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

#ifndef __KEYMGMT_H__
#define __KEYMGMT_H__


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

#include "context.h"
#include "keylist.h"
#include "keygenthread.h"
#include "keydetailsdialog.h"

class KeyMgmt : public QMainWindow
{
    Q_OBJECT

public:
    KeyMgmt(GpgME::Context* ctx, QString iconPath);

public slots:
    void importKeyFromFile();
    void importKeyFromClipboard();
    void exportKeyToFile();
    void exportKeyToClipboard();
    void deleteCheckedKeys();
    void deleteSelectedKeys();
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
    QDialog *genkeyDialog;
    QAction *importKeyFromFileAct;
    QAction *importKeyFromEditAct;
    QAction *importKeyFromClipboardAct;
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

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // __KEYMGMT_H__
