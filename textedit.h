/*
 *      textedit.h
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

#ifndef __TEXTEDIT_H__
#define __TEXTEDIT_H__

#include <QPlainTextEdit>

#include <QtGui>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QApplication>
#include <QFile>

class QWidget;
class QString;

class TextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    TextEdit(QWidget *parent=0);
    void setCurrentFile(const QString &fileName);
    void loadFile(const QString &fileName);
    bool maybeSave();

public slots:
    void quote();
    bool save();
    bool saveAs();
    void open();
    void print();

private:
    bool isKey(QString key);
    bool saveFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    QString curFile;

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent* event);

};
#endif // TEXTEDIT
