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
#include "mime.h"

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
    void saveFile();

public:
    Attachments(QString iconpath, QWidget *parent = 0);
    void addMimePart(MimePart *mp);

private:
    void createActions();
    void saveByteArrayToFile(QByteArray outBuffer);
    QStringList *getSelected();
    QList<int> getSelectedPos();
    QTableWidget *mAttachmentTable;
    //QList<QPointer<QByteArray>>
    QList<QByteArray> *attachmentBodys;
    QAction *saveFileAct;
    QString iconPath;

protected:
    void contextMenuEvent(QContextMenuEvent *event);

};

#endif // __ATTACHMENTS_H__
