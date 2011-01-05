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
#include "editorpage.h"

class QWidget;
class QString;
class QTabWidget;

class TextEdit : public QWidget
{
    Q_OBJECT
public:
    TextEdit();
    void loadFile(const QString &fileName);
    bool maybeSaveAnyTab();
    bool maybeSaveCurrentTab();
    QPlainTextEdit* curTextPage();

public slots:
    void quote();
    void save();
    bool saveAs();
    void open();
    void print();
    void newTab();
    void showModified();
    void closeTab();
    void switchTabUp();
    void switchTabDown();

private:
    QString strippedName(const QString &fullFileName);
    int countPage;
    QTabWidget *tabWidget;
    bool maybeSaveFile();
    EditorPage *curPage();
    void setCursorPosition();

private slots:
    void removeTab(int index);
    void cut();
    void copy();
    void paste();
    void undo();
    void redo();
    void selectAll();

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent* event);
    bool saveFile(const QString &fileName);
    bool closeFile();
};
#endif // TEXTEDIT
