/*
 *      gpgwin.h
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

#ifndef __GPGWIN_H__
#define __GPGWIN_H__

#include "context.h"
#include "keylist.h"
#include "attachments.h"
#include "mime.h"
#include "keymgmt.h"
#include "keydetailsdialog.h"
#include "textedit.h"
#include "keyserverimportdialog.h"

class QMainWindow;
class QPlainTextEdit;
class QWidget;
class QVBoxLayout;
class QGridLayout;
class iostream;
class QtGui;
class QString;
class QFileDialog;
class QStringList;
class QIcon;
class QMessageBox;
class QVBoxLayout;
class QAction;
class QMenu;
class QPlainTextEdit;
class QComboBox;
class QPushButton;
class QRadioButton;
class QButtonGroup;
class QApplication;
class QDockWidget;


class GpgWin : public QMainWindow
{
    Q_OBJECT

public:
    GpgWin();

protected:
    void closeEvent(QCloseEvent *event);

public slots:
    void encrypt();
    void decrypt();
    void sign();
    void verify();
    void importKeyFromFile();
    void importKeyFromEdit();
    void importKeyFromClipboard();
    void importKeyFromKeyServer();
    void importKeyDialog();
    //void deleteSelectedKeys();
    void appendSelectedKeys();
    void copyMailAddressToClipboard();
    void showKeyDetails();
    void openKeyManagement();
    void about();
    void fileEncryption();
    void openSettingsDialog();
    void openTutorial();
    void checkAttachmentFolder();
    void openTranslate();
    void cleanDoubleLinebreaks();
//    void dropEvent(QDropEvent *event);

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createDockWindows();
    void restoreSettings();
    void saveSettings();
    void preventNoDataErr(QByteArray *in);
    void parseMime(QByteArray *message);
    int isSigned(const QByteArray &text);

    TextEdit *edit;
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *cryptMenu;
    QMenu *helpMenu;
    QMenu *keyMenu;
    QMenu *viewMenu;
    QMenu *importKeyMenu;
    QToolBar *cryptToolBar;
    QToolBar *editToolBar;
    QToolBar *keyToolBar;
    QDockWidget *dock;
    QDockWidget *aDock;
    QDialog *genkeyDialog;

    QAction *newTabAct;
    QAction *switchTabUpAct;
    QAction *switchTabDownAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *printAct;
    QAction *closeTabAct;
    QAction *quitAct;
    QAction *encryptAct;
    QAction *decryptAct;
    QAction *signAct;
    QAction *verifyAct;
    QAction *importKeyDialogAct;
    QAction *importKeyFromFileAct;
    QAction *importKeyFromEditAct;
    QAction *importKeyFromClipboardAct;
    QAction *importKeyFromKeyServerAct;
    QAction *cleanDoubleLinebreaksAct;

    QAction *appendSelectedKeysAct;
    QAction *copyMailAddressToClipboardAct;
    QAction *showKeyDetailsAct;
    QAction *openKeyManagementAct;
    QAction *copyAct;
    QAction *quoteAct;
    QAction *cutAct;
    QAction *pasteAct;
    QAction *selectallAct;
    QAction *undoAct;
    QAction *redoAct;
    QAction *aboutAct;
    QAction *fileEncryptionAct;
    QAction *openSettingsAct;
    QAction *openTranslateAct;
    QAction *openTutorialAct;
    QLineEdit *nameEdit;
    QLineEdit *emailEdit;
    QLineEdit *commentEdit;
    QLineEdit *passwordEdit;
    QLineEdit *repeatpwEdit;
    QSpinBox *keysizeSpinBox;
    QLabel *nameLabel;
    QLabel *emailLabel;
    QLabel *commentLabel;
    QLabel *keysizeLabel;
    QLabel *passwordLabel;
    QLabel *repeatpwLabel;
    QLabel *errorLabel;
    QLabel *statusBarIcon;

    QSettings settings;
    KeyList *mKeyList;
    Attachments *mAttachments;
    GpgME::Context *mCtx;
    QString iconPath;
    KeyMgmt *keyMgmt;
    KeyServerImportDialog *importDialog;
};

#endif // __GPGWIN_H__
