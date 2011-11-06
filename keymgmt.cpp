/*
 *
 *      keymgmt.cpp
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

#include "keymgmt.h"

KeyMgmt::KeyMgmt(GpgME::GpgContext *ctx, QString iconpath)
{
    mCtx = ctx;
    mIconPath = iconpath;

    /* the list of Keys available*/
    mKeyList = new KeyList(mCtx, mIconPath);
    mKeyList->setColumnWidth(2, 250);
    mKeyList->setColumnWidth(3, 250);
    setCentralWidget(mKeyList);

    createActions();
    createMenus();
    createToolBars();

    /* Restore the iconstyle */
    QSettings settings;
    settings.sync();
    QSize iconSize = settings.value("toolbar/iconsize", QSize(32, 32)).toSize();
    Qt::ToolButtonStyle buttonStyle = static_cast<Qt::ToolButtonStyle>(settings.value("toolbar/iconstyle", Qt::ToolButtonTextUnderIcon).toUInt());
    this->setIconSize(iconSize);
    this->setToolButtonStyle(buttonStyle);

    // state sets pos & size of dock-widgets
    this->restoreState(settings.value("keymgmt/windowState").toByteArray());

    // Restore window size & location
    if (settings.value("window/windowSave").toBool()) {
        QPoint pos = settings.value("keymgmt/pos", QPoint(100, 100)).toPoint();
        QSize size = settings.value("keymgmt/size", QSize(800, 450)).toSize();
        this->resize(size);
        this->move(pos);
    } else {
        this->resize(QSize(800, 400));
    }

    setWindowTitle(tr("Keymanagement"));
    mKeyList->addMenuAction(deleteSelectedKeysAct);
    mKeyList->addMenuAction(showKeyDetailsAct);
}

void KeyMgmt::createActions()
{
    closeAct = new QAction(tr("&Close Key Management"), this);
    closeAct->setShortcut(tr("Ctrl+Q"));
    closeAct->setIcon(QIcon(mIconPath + "exit.png"));
    closeAct->setToolTip(tr("Close Key Management"));
    connect(closeAct, SIGNAL(triggered()), this, SLOT(close()));

    importKeyFromFileAct = new QAction(tr("&File"), this);
    importKeyFromFileAct->setIcon(QIcon(mIconPath + "import_key_from_file.png"));
    importKeyFromFileAct->setToolTip(tr("Import New Key From File"));
    connect(importKeyFromFileAct, SIGNAL(triggered()), this, SLOT(importKeyFromFile()));

    importKeyFromClipboardAct = new QAction(tr("&Clipboard"), this);
    importKeyFromClipboardAct->setIcon(QIcon(mIconPath + "import_key_from_clipboard.png"));
    importKeyFromClipboardAct->setToolTip(tr("Import New Key From Clipboard"));
    connect(importKeyFromClipboardAct, SIGNAL(triggered()), this, SLOT(importKeyFromClipboard()));

    importKeyFromKeyServerAct = new QAction(tr("&Keyserver"), this);
    importKeyFromKeyServerAct->setIcon(QIcon(mIconPath + "import_key_from_server.png"));
    importKeyFromKeyServerAct->setToolTip(tr("Import New Key From Keyserver"));
    connect(importKeyFromKeyServerAct, SIGNAL(triggered()), this, SLOT(importKeyFromKeyServer()));

    exportKeyToClipboardAct = new QAction(tr("Export To &Clipboard"), this);
    exportKeyToClipboardAct->setIcon(QIcon(mIconPath + "export_key_to_clipbaord.png"));
    exportKeyToClipboardAct->setToolTip(tr("Export Selected Key(s) To Clipboard"));
    connect(exportKeyToClipboardAct, SIGNAL(triggered()), this, SLOT(exportKeyToClipboard()));

    exportKeyToFileAct = new QAction(tr("Export To &File"), this);
    exportKeyToFileAct->setIcon(QIcon(mIconPath + "export_key_to_file.png"));
    exportKeyToFileAct->setToolTip(tr("Export Selected Key(s) To File"));
    connect(exportKeyToFileAct, SIGNAL(triggered()), this, SLOT(exportKeyToFile()));

    deleteSelectedKeysAct = new QAction(tr("Delete Selected Key(s)"), this);
    deleteSelectedKeysAct->setToolTip(tr("Delete the Selected keys"));
    connect(deleteSelectedKeysAct, SIGNAL(triggered()), this, SLOT(deleteSelectedKeys()));

    deleteCheckedKeysAct = new QAction(tr("Delete Checked Key(s)"), this);
    deleteCheckedKeysAct->setToolTip(tr("Delete the Checked keys"));
    deleteCheckedKeysAct->setIcon(QIcon(mIconPath + "button_cancel.png"));
    connect(deleteCheckedKeysAct, SIGNAL(triggered()), this, SLOT(deleteCheckedKeys()));

    generateKeyDialogAct = new QAction(tr("Generate Key"), this);
    generateKeyDialogAct->setToolTip(tr("Generate New Key"));
    generateKeyDialogAct->setIcon(QIcon(mIconPath + "key_generate.png"));
    connect(generateKeyDialogAct, SIGNAL(triggered()), this, SLOT(generateKeyDialog()));

    showKeyDetailsAct = new QAction(tr("Show Keydetails"), this);
    showKeyDetailsAct->setToolTip(tr("Show Details for this Key"));
    connect(showKeyDetailsAct, SIGNAL(triggered()), this, SLOT(showKeyDetails()));
}

void KeyMgmt::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(closeAct);

    keyMenu = menuBar()->addMenu(tr("&Key"));
    importKeyMenu = keyMenu->addMenu(tr("&Import Key From..."));
    importKeyMenu->addAction(importKeyFromFileAct);
    importKeyMenu->addAction(importKeyFromClipboardAct);
    importKeyMenu->addAction(importKeyFromKeyServerAct);
    keyMenu->addAction(exportKeyToFileAct);
    keyMenu->addAction(exportKeyToClipboardAct);
    keyMenu->addSeparator();
    keyMenu->addAction(deleteCheckedKeysAct);
    keyMenu->addAction(generateKeyDialogAct);
}

void KeyMgmt::createToolBars()
{
    QToolBar *keyToolBar = addToolBar(tr("Key"));
    keyToolBar->setObjectName("keytoolbar");

    // add button with popup menu for import
    QToolButton* toolButton = new QToolButton(this);
    toolButton->setMenu(importKeyMenu);
    toolButton->setPopupMode(QToolButton::InstantPopup);
    toolButton->setIcon(QIcon(mIconPath + "key_import.png"));
    toolButton->setToolTip("Import key");
    toolButton->setText("Import key from");
    toolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    keyToolBar->addWidget(toolButton);

    keyToolBar->addSeparator();
    keyToolBar->addAction(deleteCheckedKeysAct);
    keyToolBar->addSeparator();
    keyToolBar->addAction(exportKeyToFileAct);
    keyToolBar->addAction(exportKeyToClipboardAct);

}

void KeyMgmt::importKeyFromFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Key"), "", tr("Key Files") + " (*.asc *.txt);;"+tr("Keyring files")+" (*.gpg);;All Files (*)");
    if (! fileName.isNull()) {
        QFile file;
        file.setFileName(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << tr("Couldn't Open File: ") + fileName;
        }
        QByteArray inBuffer = file.readAll();

        mCtx->importKey(inBuffer);
    }
}

void KeyMgmt::importKeyFromKeyServer()
{
    importDialog = new KeyServerImportDialog(mCtx, this);
    importDialog->show();
}

void KeyMgmt::importKeyFromClipboard()
{
    QClipboard *cb = QApplication::clipboard();
    mCtx->importKey(cb->text(QClipboard::Clipboard).toAscii());
}

void KeyMgmt::deleteSelectedKeys()
{
    deleteKeysWithWarning(mKeyList->getSelected());
}

void KeyMgmt::deleteCheckedKeys()
{
    deleteKeysWithWarning(mKeyList->getChecked());
}

void KeyMgmt::deleteKeysWithWarning(QStringList *uidList)
{
    /**
     * TODO: Different Messages for private/public key, check if
     * more than one selected... compare to seahorse "delete-dialog"
     */

    if (uidList->isEmpty()) {
        return;
    }
    QString keynames;
    foreach (QString uid, *uidList) {
        keynames.append(mCtx->getKeyDetails(uid)->uids->name);
        keynames.append("<i> &lt;");
        keynames.append(mCtx->getKeyDetails(uid)->uids->email);
        keynames.append("&gt; </i><br/>");
    }

    int ret = QMessageBox::warning(this, tr("Deleting Keys"),
                                    tr("<b>Are you sure that you want to delete the following keys?.</b><br/><br/>")+keynames+
                                    tr("<br/>The action can not be undone."),
                                    QMessageBox::No | QMessageBox::Yes);

    if (ret == QMessageBox::Yes) {
        mCtx->deleteKeys(uidList);
    }
}

void KeyMgmt::showKeyDetails()
{
    if (mKeyList->getSelected()->isEmpty()) {
        return;
    }

    // TODO: first...?
    gpgme_key_t key = mCtx->getKeyDetails(mKeyList->getSelected()->first());

    new KeyDetailsDialog(mCtx, key);
}

void KeyMgmt::exportKeyToFile()
{
    QByteArray *keyArray = new QByteArray();
    if (!mCtx->exportKeys(mKeyList->getChecked(), keyArray)) {
        return;
    }
    gpgme_key_t key = mCtx->getKeyDetails(mKeyList->getChecked()->first());
    QString fileString = QString(key->uids->name) + " " + QString(key->uids->email) + "(" + QString(key->subkeys->keyid)+ ")_pub.asc";

    QString fileName = QFileDialog::getSaveFileName(this, tr("Export Key To File"), fileString, tr("Key Files") + " (*.asc *.txt);;All Files (*)");
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QTextStream stream(&file);
    stream << *keyArray;
    file.close();
    delete keyArray;
}

void KeyMgmt::exportKeyToClipboard()
{
    QByteArray *keyArray = new QByteArray();
    QClipboard *cb = QApplication::clipboard();
    if (!mCtx->exportKeys(mKeyList->getChecked(), keyArray)) {
        return;
    }
    cb->setText(*keyArray);
    delete keyArray;
}

void KeyMgmt::generateKeyDialog()
{
    KeyGenDialog *keyGenDialog = new KeyGenDialog(mCtx,this);
    keyGenDialog->show();
}

void KeyMgmt::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    //settings.setValue("geometry", saveGeometry());
    settings.setValue("keymgmt/windowState", saveState());
    settings.setValue("keymgmt/pos", pos());
    settings.setValue("keymgmt/size", size());

    QMainWindow::closeEvent(event);
}
