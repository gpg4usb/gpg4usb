/*
 *      mainwindow.h
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

#ifndef __GPGWIN_H__
#define __GPGWIN_H__

#include "gpgconstants.h"
#include "attachments.h"
#include "keymgmt.h"
#include "textedit.h"
#include "fileencryptiondialog.h"
#include "settingsdialog.h"
#include "verifynotification.h"
#include "wizard.h"

QT_BEGIN_NAMESPACE
class QMainWindow;
class QTextEdit;
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
class QTextEdit;
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
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief
     *
     */
    MainWindow();
public slots:
    void setStatusBarText(QString text);

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
     * @details Show the details of the first of the first of selected keys
     */
     void showKeyDetails();
 
    /**
     * @details Refresh key information of selected keys from default keyserver
     */
     void refreshKeysFromKeyserver();
 
    /**
      * @details upload the selected key to the keyserver
      */
     void uploadKeyToServer();
 
    /**
      * @details start the wizard
      */
     void startWizard();

    /**
     * @details Import keys from currently active tab to keylist if possible.
     */
    void importKeyFromEdit();

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
     * @details Open dialog for encrypting file.
     */
    void fileEncrypt();

    /**
     * @details Open dialog for decrypting file.
     */
    void fileDecrypt();

    /**
     * @details Open settings-dialog.
     */
    void openSettingsDialog();

    /**
     * @details Open online-tutorial in default browser.
     */
    void openTutorial();

    /**
     * @details Open integrated help in new tab.
     */
    void openHelp();

    /**
     * @details Open integrated help in new tab with the specified page.
     */
    void openHelp(const QString page);

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

    /**
     * @details Cut the existing PGP header and footer from current tab.
     */
    void cutPgpHeader();

    /**
     * @details Add PGP header and footer to current tab.
     */
    void addPgpHeader();

//    void dropEvent(QDropEvent *event);

    /**
     * @details Disable tab related actions, if number of tabs is 0.
     * @param number number of the opened tabs and -1, if no tab is opened
     */
    void disableTabActions(int number);

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
     * @details Create attachment-dockwindow.
     */
    void createAttachmentDock();

    /**
     * @details close attachment-dockwindow.
     */
    void closeAttachmentDock();

    /**
     * @details Load settings from ini-file.
     */
    void restoreSettings();

    /**
     * @details Save settings to ini-file.
     */
    void saveSettings();

    /**
     * @brief
     *
     * @param message
     */
    void parseMime(QByteArray *message);

    TextEdit *edit; /** Tabwidget holding the edit-windows */
    QMenu *fileMenu; /** Submenu for file-operations*/
    QMenu *editMenu; /** Submenu for text-operations*/
    QMenu *cryptMenu; /** Submenu for crypt-operations */
    QMenu *helpMenu; /** Submenu for help-operations */
    QMenu *keyMenu; /** Submenu for key-operations */
    QMenu *viewMenu; /** Submenu for view operations */
    QMenu *importKeyMenu; /** Sumenu for import operations */
    QMenu *steganoMenu; /** Submenu for steganographic operations*/
    QToolBar *cryptToolBar; /** Toolbar holding crypt actions */
    QToolBar *fileToolBar; /** Toolbar holding file actions */
    QToolBar *editToolBar; /** Toolbar holding edit actions */
    QToolBar *specialEditToolBar; /** Toolbar holding special edit actions */
    QToolBar *keyToolBar; /** Toolbar holding key operations */
    QToolButton* importButton; /** Toolbutton for import dropdown menu in toolbar */
    QToolButton* fileEncButton; /** Toolbutton for file cryption dropdown menu in toolbar */
    QDockWidget *keylistDock; /** Encrypt Dock*/
    QDockWidget *attachmentDock; /** Attachment Dock */
    QDialog *genkeyDialog; /** Dialog for key generation */

    QAction *newTabAct; /** Action to create new tab */
    QAction *switchTabUpAct; /** Action to switch tab up*/
    QAction *switchTabDownAct; /** Action to switch tab down */
    QAction *openAct; /** Action to open file */
    QAction *saveAct; /** Action to save file */
    QAction *saveAsAct; /** Action to save file as */
    QAction *printAct; /** Action to print */
    QAction *closeTabAct; /** Action to print */
    QAction *quitAct; /** Action to quit application */
    QAction *encryptAct; /** Action to encrypt text */
    QAction *decryptAct; /** Action to decrypt text */
    QAction *signAct; /** Action to sign text */
    QAction *verifyAct; /** Action to verify text */
    QAction *importKeyFromEditAct; /** Action to import key from edit */
    QAction *cleanDoubleLinebreaksAct; /** Action to remove double line breaks */

    QAction *appendSelectedKeysAct; /** Action to append selected keys to edit */
    QAction *copyMailAddressToClipboardAct; /** Action to copy mail to clipboard */
    QAction *openKeyManagementAct; /** Action to open key management */
    QAction *copyAct; /** Action to copy text */
    QAction *quoteAct; /** Action to quote text */
    QAction *cutAct; /** Action to cut text */
    QAction *pasteAct; /** Action to paste text */
    QAction *selectallAct; /** Action to select whole text */
    QAction *undoAct; /** Action to undo last action */
    QAction *redoAct; /** Action to redo last action */
    QAction *zoomInAct; /** Action to zoom in */
    QAction *zoomOutAct; /** Action to zoom out */
    QAction *aboutAct; /** Action to open about dialog */
    QAction *fileEncryptionAct; /** Action to open file-encryption dialog */
    QAction *fileEncryptAct; /** Action to open dialog for encrypting file */
    QAction *fileDecryptAct; /** Action to open dialog for decrypting file */
    QAction *openSettingsAct; /** Action to open settings dialog */
    QAction *openTranslateAct; /** Action to open translate doc*/
    QAction *openTutorialAct; /** Action to open tutorial */
    QAction *openHelpAct; /** Action to open tutorial */
    QAction *showKeyDetailsAct; /** Action to open key-details dialog */
    QAction *refreshKeysFromKeyserverAct; /** Action to refresh a key from keyserver */
    QAction *uploadKeyToServerAct; /** Action to append selected keys to edit */
    QAction *startWizardAct; /** Action to open the wizard */
    QAction *cutPgpHeaderAct; /** Action for cutting the PGP header */
    QAction *addPgpHeaderAct; /** Action for adding the PGP header */

    QLabel *statusBarIcon; /**< TODO */
    QSettings settings; /**< TODO */
    KeyList *mKeyList; /**< TODO */
    Attachments *mAttachments; /**< TODO */
    GpgME::GpgContext *mCtx; /**< TODO */
    KeyMgmt *keyMgmt; /**< TODO */
    KeyServerImportDialog *importDialog; /**< TODO */
    bool attachmentDockCreated;
};

#endif // __GPGWIN_H__
