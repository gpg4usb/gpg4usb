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
    TextEdit(QString iconPath);
    void loadFile(const QString &fileName);
    bool maybeSaveAnyTab();
    QPlainTextEdit* curTextPage();
    QHash<int, QString> unsavedDocuments();

public slots:
    void quote();

    /****************************************************************************************
     * Name:                save
     * Description:         Saves the content of the current tab, if it has a filepath
     *                      otherwise it calls saveAs for the current tab
     * Parameters:          none
     * Return Values:       none
     * Change on members:
    */
    void save();

    /****************************************************************************************
     * Name:                SaveAs
     * Description:         Opens a savefiledialog and calls saveFile with the choosen filename
     * Parameters:          none
     * Return Values:       just returns the return value of the saveFile method
     * Change on members:   none
    */
    bool saveAs();

    /****************************************************************************************
     * Name:                open
     * Description:         shows an OpenFileDoalog and opens the file in a new tab
     *                      shows an error dialog, if the open fails
     *                      sets the focus to the tab of the opened file
     * Parameters:          none
     * Return Values:       none
     * Change on members:   none
    */
    void open();

    /****************************************************************************************
     * Name:                print
     * Description:         opens print dialog for the current tab
     * Parameters:          none
     * Return Values:       none
     * Change on members:   none
    */
    void print();

    /****************************************************************************************
     * Name:                newTab()
     * Description:         Adds a new tab with the title "untitled"+countpage+".txt"
     *                      Sets the focus to the new tab
     * Parameters:          none
     * Return Values:       none
     * Change on members:   increases countPage per 1
    */
    void newTab();

    /****************************************************************************************
     * Name:
     * Description:
     * Parameters:
     * Return Values:
     * Change on members:
    */
    void showModified();
    void closeTab();
    void switchTabUp();
    void switchTabDown();

private:
    QString strippedName(const QString &fullFileName);
    bool maybeSaveFile();
    EditorPage *curPage();
    void setCursorPosition();
    QString mIconPath;
    int countPage;
    QTabWidget *tabWidget;
    bool maybeSaveCurrentTab(bool askToSave);

private slots:
    /****************************************************************************************
     * Name:                removeTab
     * Description:         Removes the tab on index
     * Parameters:          int index, shows the index of the tab to remove
     * Return Values:       none
     * Change on members:   none
    */
    void removeTab(int index);
    void cut();
    void copy();
    void paste();
    void undo();
    void redo();
    void selectAll();

protected:
//    void dragEnterEvent(QDragEnterEvent *event);
//    void dropEvent(QDropEvent* event);
    /****************************************************************************************
     * Name:                saveFile
     * Description:         Saves the content of currentTab to the file filename
     * Parameters:          QString filename contains the full path of the file to save
     * Return Values:       true, if the file was saved succesfully
     *                      false, if parameter filename is empty or the saving failed
     * Change on members:   sets isModified of the current tab to false
    */
    bool saveFile(const QString &fileName);
};
#endif // TEXTEDIT
