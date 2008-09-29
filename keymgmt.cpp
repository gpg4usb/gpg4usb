/*
 *      keymgmt.cpp
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

#include <QtGui>

#include "keymgmt.h"


KeyMgmt::KeyMgmt(GpgME::Context *ctx, QString iconpath)
{
    mCtx = ctx;
    mIconPath = iconpath;
    resize(640, 400);

    /* the list of Keys available*/
    m_keyList = new KeyList(mCtx, mIconPath);
    m_keyList->setColumnWidth(2,250);
    m_keyList->setColumnWidth(3,250);
    setCentralWidget(m_keyList);
    createActions();
    createMenus();
    createToolBars();
    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    setIconSize(QSize(32, 32));

    setWindowTitle(tr("Keymanagement"));
    m_keyList->addMenuAction(deleteSelectedKeysAct);
}

void KeyMgmt::createActions()
{
    closeAct = new QAction(tr("&Close Key Management"), this);
    closeAct->setShortcut(tr("Ctrl+Q"));
    closeAct->setIcon(QIcon(mIconPath + "exit.png"));
    closeAct->setStatusTip(tr("Close Key Management"));
    connect(closeAct, SIGNAL(triggered()), this, SLOT(close()));

    importKeyFromFileAct = new QAction(tr("Import Key From &File"), this);
    importKeyFromFileAct->setIcon(QIcon(mIconPath + "importkey_editor.png"));
    importKeyFromFileAct->setStatusTip(tr("Import New Key From File"));
    connect(importKeyFromFileAct, SIGNAL(triggered()), this, SLOT(importKeyFromFile()));

    importKeyFromClipboardAct = new QAction(tr("Import Key From &Clipboard"), this);
    importKeyFromClipboardAct->setIcon(QIcon(mIconPath + "importkey_editor.png"));
    importKeyFromClipboardAct->setStatusTip(tr("Import New Key From Clipboard"));
    connect(importKeyFromClipboardAct, SIGNAL(triggered()), this, SLOT(importKeyFromClipboard()));

    deleteSelectedKeysAct = new QAction(tr("Delete Selected Key(s)"), this);
    deleteSelectedKeysAct->setStatusTip(tr("Delete the Selected keys"));
    connect(deleteSelectedKeysAct, SIGNAL(triggered()), this, SLOT(deleteSelectedKeys()));

    deleteCheckedKeysAct = new QAction(tr("Delete Checked Key(s)"), this);
    deleteCheckedKeysAct->setStatusTip(tr("Delete the Checked keys"));
    deleteCheckedKeysAct->setIcon(QIcon(mIconPath + "button_cancel.png"));
    connect(deleteCheckedKeysAct, SIGNAL(triggered()), this, SLOT(deleteCheckedKeys()));
}

void KeyMgmt::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&Quit"));
    fileMenu->addAction(closeAct);
    keyMenu = menuBar()->addMenu(tr("&Key"));
    keyMenu->addAction(importKeyFromFileAct);
    keyMenu->addAction(importKeyFromClipboardAct);
    keyMenu->addSeparator();
    keyMenu->addAction(deleteCheckedKeysAct);
}

void KeyMgmt::createToolBars()
{
    QToolBar *keyToolBar = addToolBar(tr("Crypt"));
    keyToolBar->addAction(importKeyFromFileAct);
    keyToolBar->addAction(importKeyFromClipboardAct);
    keyToolBar->addSeparator();
    keyToolBar->addAction(deleteCheckedKeysAct);
}

void KeyMgmt::importKeyFromFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Key"), "", tr("Key Files") + " (*.asc *.txt);;All Files (*.*)");
    if (! fileName.isNull()) {
        QFile file;
        file.setFileName(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << tr("couldn't open file: ") + fileName;
        }
        QByteArray inBuffer = file.readAll();

        mCtx->importKey(inBuffer);
        m_keyList->refresh();
    }
}

void KeyMgmt::importKeyFromClipboard()
{
    QClipboard *cb = QApplication::clipboard();
    mCtx->importKey(cb->text(QClipboard::Clipboard).toAscii());
    m_keyList->refresh();
}

void KeyMgmt::deleteSelectedKeys()
{
    mCtx->deleteKeys(m_keyList->getSelected());
    m_keyList->refresh();
}

void KeyMgmt::deleteCheckedKeys()
{
    mCtx->deleteKeys(m_keyList->getChecked());
    m_keyList->refresh();
}
