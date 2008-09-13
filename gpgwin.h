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
#include <QApplication>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QWidget>
#include <QVBoxLayout>
#include <QGridLayout>
#include <iostream>
#include <QtGui>
#include <QString>
#include <QFileDialog>
#include <QStringList>
#include <QIcon>
#include <QMessageBox>
#include <QVBoxLayout>
#include "context.h"
#include "keylist.h"
#include "attachments.h"

class QAction;
class QMenu;
class QPlainTextEdit;
class QComboBox;
class QPushButton;
class QRadioButton;
class QButtonGroup;


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
    void print();
    void about();
    bool save();
    bool saveAs();
    void open();
    void importKeyDialog();
    //void browse();

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
    QString strippedName(const QString &fullFileName);
    void preventNoDataErr(QByteArray *in); 

    QPlainTextEdit *edit;
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *cryptMenu;
    QMenu *helpMenu;
    QMenu *importKeyMenu;
    QToolBar *cryptToolBar;
    QToolBar *editToolBar;
    QPushButton *browseButton;
    QWidget *keywindow;

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
    QAction *copyAct;
    QAction *cutAct;
    QAction *pasteAct;
    QAction *selectallAct;
    QAction *aboutAct;

    QString curFile;
    KeyList *m_keyList;
    Attachments *m_attachments;
    GpgME::Context *myCtx;
    QString iconPath;
};



