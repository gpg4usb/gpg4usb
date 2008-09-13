/*
 *      attachments.h
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

#ifndef __ATTACHMENTS_H__
#define __ATTACHMENTS_H__

#include "context.h"
#include "keylist.h"

class QVBoxLayout;
class QDebug;
class QFileDialog;
class QMessageBox;
class iostream;
class QListWidget;
class QWidget;
class QAction;
class QMenu;
class QContextMenuEvent;
class QMenu;

class Attachments : public QWidget
{
    Q_OBJECT

public slots:
    void addFile();
    void encryptFile();
    void decryptFile();

public:
    Attachments(QWidget *parent = 0);
    void setIconPath(QString iconPath);
    void setContext(GpgME::Context *ctx);
    void setKeyList(KeyList *keylist);

private:
    void createActions();
    QStringList *getSelected();
    QListWidget *m_attachmentList;
    QAction *addFileAct;
    QAction *encryptAct;
    QAction *decryptAct;
    QString iconPath;
    GpgME::Context *m_ctx;
    KeyList *m_keyList;

protected:
    void contextMenuEvent(QContextMenuEvent *event);

};

#endif // __ATTACHMENTS_H__
