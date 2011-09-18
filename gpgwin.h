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

#include "attachments.h"
#include "keymgmt.h"
#include "textedit.h"
#include "fileencryptiondialog.h"
#include "settingsdialog.h"
#include "verifynotification.h"

QT_BEGIN_NAMESPACE
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
QT_END_NAMESPACE


/**
 * @brief
 *
 */
class GpgWin : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief
     *
     */
    GpgWin();

protected:
    /**
     * @details Close event shows a save dialog, if there are unsaved documents on exit.
     * @param event
     */
    void closeEvent(QCloseEvent *event);

private slots:
    /**
     * @details encrypt the text of currently active textedit-page
     * with the currently checked keys
     */
    void encrypt();

    /**
     * @details Show a passphrase dialog and decrypt the text of currently active tab.
     */
    void decrypt();

    /**
     * @details Sign the text of currently active tab with the checked private keys
     */
    void sign();

    /**
     * @details Verify the text of currently active tab and show verify information.
     * If document is signed with a key, which is not in keylist, show import missing
     * key from keyserver in Menu of verifynotification.
     */
    void verify();

    /**
     * @details Open File-Dialog and import the keys from the choosen file
     * to keylist if possible.
     */
    void importKeyFromFile();

    /**
     * @details Import keys from currently active tab to keylist if possible.
     */
    void importKeyFromEdit();

    /**
     * @details Import keys from clipboard to keylist if possible.
     */
    void importKeyFromClipboard();

    /**
     * @details Open an "Import key from keyserver"-dialog.
     */
    void importKeyFromKeyServer();

    /**
     * @details Open a dialog, in which you can choose, where keys should be imported from.
     */
    void importKeyDialog();

    /**
     * @details Append the selected keys to currently active textedit.
     */
    void appendSelectedKeys();

    /**
     * @details Copy the mailaddress of selected key to clipboard.
     * Method for keylists contextmenu.
     */
    void copyMailAddressToClipboard();

    /**
     * @details Show detail-dialog for selected key.
     */
    void showKeyDetails();

    /**
     * @details Open key management dialog.
     */
    void openKeyManagement();

    /**
     * @details Open about-dialog.
     */
    void about();

    /**
     * @details Open fileencrytion dialog.
     */
    void fileEncryption();

    /**
     * @details Open settings-dialog.
     */
    void openSettingsDialog();

    /**
     * @details Open online-tutorial in default browser.
     */
    void openTutorial();

    /**
     * @details Show a warn message in status bar, if there are files in attachment folder.
     */
    void checkAttachmentFolder();

    /**
     * @details Open online translation tutorial in default browser.
     */
    void openTranslate();

    /**
     * @details Replace double linebreaks by single linebreaks in currently active tab.
     */
    void cleanDoubleLinebreaks();
//    void dropEvent(QDropEvent *event);

private:
    /**
     * @details Create actions for the main-menu and the context-menu of the keylist.
     */
    void createActions();

    /**
     * @details create the menu of the main-window.
     */
    void createMenus();

    /**
     * @details Create edit-, crypt- and key-toolbars.
     */
    void createToolBars();

    /**
     * @details Create statusbar of mainwindow.
     */
    void createStatusBar();

    /**
     * @details Create keylist- and attachment-dockwindows.
     */
    void createDockWindows();

    /**
     * @details Load settings from ini-file.
     */
    void restoreSettings();

    /**
     * @details Save settings to ini-file.
     */
    void saveSettings();

    /**
     * @details If text contains PGP-message, put a linebreak before the message,
     * so that gpgme can decrypt correctly
     *
     * @param in Pointer to the QBytearray to check.
     */
    void preventNoDataErr(QByteArray *in);

    /**
     * @brief
     *
     * @param message
     */
    void parseMime(QByteArray *message);

    /**
     * @brief
     *
     * @param text
     * @return \li 2, if the text is completly signed,
     *          \li 1, if the text is partially signed,
     *          \li 0, if the text is not signed at all.
     */
    int isSigned(const QByteArray &text);

    /**
     * @details Put a space after every four chars of fingerprint.
     *
     * @param fingerprint
     */
    QString beautifyFingerprint(QString fingerprint);

    TextEdit *edit; /** TODO */
    QMenu *fileMenu; /** Submenu for file-operations*/
    QMenu *editMenu; /** Submenu for text-operations*/
    QMenu *cryptMenu; /** Submenu for crypt-operations */
    QMenu *helpMenu; /** Submenu for help-operations */
    QMenu *keyMenu; /** Submenu for key-operations */
    QMenu *viewMenu; /** View submenu */
    QMenu *importKeyMenu; /**< TODO */
    QToolBar *cryptToolBar; /**< TODO */
    QToolBar *editToolBar; /**< TODO */
    QToolBar *keyToolBar; /**< TODO */
    QDockWidget *dock; /**< TODO */
    QDockWidget *aDock; /**< TODO */
    QDialog *genkeyDialog; /**< TODO */

    QAction *newTabAct; /**< TODO */
    QAction *switchTabUpAct; /**< TODO */
    QAction *switchTabDownAct; /**< TODO */
    QAction *openAct; /**< TODO */
    QAction *saveAct; /**< TODO */
    QAction *saveAsAct; /**< TODO */
    QAction *printAct; /**< TODO */
    QAction *closeTabAct; /**< TODO */
    QAction *quitAct; /**< TODO */
    QAction *encryptAct; /**< TODO */
    QAction *decryptAct; /**< TODO */
    QAction *signAct; /**< TODO */
    QAction *verifyAct; /**< TODO */
    QAction *importKeyDialogAct; /**< TODO */
    QAction *importKeyFromFileAct; /**< TODO */
    QAction *importKeyFromEditAct; /**< TODO */
    QAction *importKeyFromClipboardAct; /**< TODO */
    QAction *importKeyFromKeyServerAct; /**< TODO */
    QAction *cleanDoubleLinebreaksAct; /**< TODO */

    QAction *appendSelectedKeysAct; /**< TODO */
    QAction *copyMailAddressToClipboardAct; /**< TODO */
    QAction *showKeyDetailsAct; /**< TODO */
    QAction *openKeyManagementAct; /**< TODO */
    QAction *copyAct; /**< TODO */
    QAction *quoteAct; /**< TODO */
    QAction *cutAct; /**< TODO */
    QAction *pasteAct; /**< TODO */
    QAction *selectallAct; /**< TODO */
    QAction *undoAct; /**< TODO */
    QAction *redoAct; /**< TODO */
    QAction *aboutAct; /**< TODO */
    QAction *fileEncryptionAct; /**< TODO */
    QAction *openSettingsAct; /**< TODO */
    QAction *openTranslateAct; /**< TODO */
    QAction *openTutorialAct; /**< TODO */
    QLineEdit *nameEdit; /**< TODO */
    QLineEdit *emailEdit; /**< TODO */
    QLineEdit *commentEdit; /**< TODO */
    QLineEdit *passwordEdit; /**< TODO */
    QLineEdit *repeatpwEdit; /**< TODO */
    QSpinBox *keysizeSpinBox; /**< TODO */
    QLabel *nameLabel; /**< TODO */
    QLabel *emailLabel; /**< TODO */
    QLabel *commentLabel; /**< TODO */
    QLabel *keysizeLabel; /**< TODO */
    QLabel *passwordLabel; /**< TODO */
    QLabel *repeatpwLabel; /**< TODO */
    QLabel *errorLabel; /**< TODO */
    QLabel *statusBarIcon; /**< TODO */

    QSettings settings; /**< TODO */
    KeyList *mKeyList; /**< TODO */
    Attachments *mAttachments; /**< TODO */
    GpgME::Context *mCtx; /**< TODO */
    QString iconPath; /**< TODO */
    KeyMgmt *keyMgmt; /**< TODO */
    KeyServerImportDialog *importDialog; /**< TODO */
};

#endif // __GPGWIN_H__
