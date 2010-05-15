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

#include "mime.h"
#include "attachmenttablemodel.h"

#include "QWidget"
#include "QTableView"
#include "QHeaderView"
#include "QVBoxLayout"
#include "QMenu"
#include "QMessageBox"
#include "QContextMenuEvent"
#include "QFileDialog"

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
    void saveByteArrayToFile(QByteArray outBuffer, QString filename);
    QAction *saveFileAct;
    QString iconPath;
    AttachmentTableModel *table;
    QTableView *tableView;

protected:
    void contextMenuEvent(QContextMenuEvent *event);

};

#endif // __ATTACHMENTS_H__
