/*
 *      keymgmt.h
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

#ifndef __QUITDIALOG_H__
#define __QUITDIALOG_H__

#include "context.h"
#include <QtGui>

class QTableWidget;

class QuitDialog : public QDialog
{
    Q_OBJECT

public:
    QuitDialog(QWidget *parent,QHash<int, QString> unsavedDocs, QString iconPath);
    bool isDiscarded();
    QList <int> getTabIdsToSave();

private slots:
    void myDiscard();

private:
    QAction *closeAct;
    QLabel *nameLabel;
    bool discarded;
    QTableWidget *mFileList;
};

#endif // __QUITDIALOG_H__
