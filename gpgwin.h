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
    void importKeyFromFile();
    void importKeyFromEdit();
    void importKeyFromClipboard();
    void importKeyDialog();
    //void deleteSelectedKeys();
    void appendSelectedKeys();
    void openKeyManagement();
    void print();
    void about();
    bool save();
    bool saveAs();
    void open();
    void fileEncryption();
    void openSettingsDialog();
//    void dropEvent(QDropEvent *event);

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createDockWindows();
    bool saveFile(const QString &fileName);
    void loadFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    bool maybeSave();
    void restoreSettings();
    void preventNoDataErr(QByteArray *in);
    void parseMime(QByteArray *message);
    QString strippedName(const QString &fullFileName);

    QPlainTextEdit *edit;
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

    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *printAct;
    QAction *quitAct;
    QAction *encryptAct;
    QAction *decryptAct;
    QAction *importKeyDialogAct;
    QAction *importKeyFromFileAct;
    QAction *importKeyFromEditAct;
    QAction *importKeyFromClipboardAct;
    QAction *appendSelectedKeysAct;
    QAction *openKeyManagementAct;
    QAction *copyAct;
    QAction *cutAct;
    QAction *pasteAct;
    QAction *selectallAct;
    QAction *aboutAct;
    QAction *fileEncryptionAct;
    QAction *openSettingsAct;
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

    QString curFile;
    KeyList *mKeyList;
    Attachments *mAttachments;
    GpgME::Context *mCtx;
    QString iconPath;
    KeyMgmt *keyMgmt;
};

#endif // __GPGWIN_H__
