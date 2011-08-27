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

#include "editorpage.h"
#include "quitdialog.h"

class QDebug;
class QtGui;
class QFileDialog;
class QMessageBox;
class QFileInfo;
class QApplication;
class QFile;
class QPlainTextEdit;
class QFileDialog;
class QMessageBox;
class QWidget;
class QString;
class QTabWidget;

class TextEdit : public QWidget
{
    Q_OBJECT
public:
    TextEdit(QString iconPath);

    /****************************************************************************************
     * Name:                loadFile
     * Description:         Load the content of file into the current textpage
     * Parameters:          Qstring containg the filename
     * Return Values:       the pointer to the currently activated textpage
     * Change on members:   textpage is filles with filecontents
    */
    void loadFile(const QString &fileName);

    /****************************************************************************************
     * Name:                maybeSaveAnyTab
     * Description:         ask if tabs should be saved
     * Parameters:          none
     * Return Values        false, if the close event should be aborted.
     * Change on members:   none
    */
    bool maybeSaveAnyTab();
    /****************************************************************************************
     * Name:                curTextPage
     * Description:         The currently activated Page
     * Parameters:          none
     * Return Values:       the pointer to the currently activated textpage
     * Change on members:
    */
    QPlainTextEdit* curTextPage();
    /****************************************************************************************
     * Name:                unsavedDocuments
     * Description:         List of currently unsaved tabs
     * Parameters:          none
     * Return Values:       a hash of tabindexes and title of unsaved tabs
     * Change on members:
    */
    QHash<int, QString> unsavedDocuments();

public slots:
    /****************************************************************************************
     * Name:                quote
     * Description:         Insert a ">" at the begining of every line of current textedit
     * Parameters:          none
     * Return Values:       none
     * Change on members:
    */
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
     * Name:                showModified
     * Description:         show an "*" in tabtitle, if textedit is modified
     * Parameters:          none
     * Return Values:       none
     * Change on members:   add "*" to title of current tab, if current textedit is modified
    */
    void showModified();
    /****************************************************************************************
     * Name:                closeTab
     * Description:         closes the tab choosen
     * Parameters:          none
     * Return Values:       none
     * Change on members:   decreases countPage per 1
    */
    void closeTab();
    /****************************************************************************************
     * Name:                switchTabUp
     * Description:         switch to next tab
     * Parameters:          none
     * Return Values:       none
     * Change on members:   increase tabWidget->count() per 1
    */
    void switchTabUp();
    /****************************************************************************************
     * Name:                switchTabDown
     * Description:         switch to next tab
     * Parameters:          none
     * Return Values:       none
     * Change on members:   decrease tabWidget->count() per 1
    */
    void switchTabDown();
    /****************************************************************************************
     * Name:                curPage
     * Description:         return pointer to the currently activated tabpage
     * Parameters:          none
     * Return Values:       pointer to the currently activated tabpage
     * Change on members:   none
    */
    EditorPage *curPage();

private:
    /****************************************************************************************
     * Name:                strippedName
     * Description:         return just filename
     * Parameters:          a filename path
     * Return Values:       QStirng containig the filename
     * Change on members:   none
    */
    QString strippedName(const QString &fullFileName);
    bool maybeSaveFile();
    bool maybeSaveCurrentTab(bool askToSave);

    QString mIconPath;
    /****************************************************************************************
     * Name:                countPage
     * Description:         int cotaining the number of added tabs
    */
    int countPage;
    QTabWidget *tabWidget;

private slots:
    /****************************************************************************************
     * Name:                removeTab
     * Description:         Removes the tab on index
     * Parameters:          int index, shows the index of the tab to remove
     * Return Values:       none
     * Change on members:   none
    */
    void removeTab(int index);

    /****************************************************************************************
     * Name:                cut
     * Description:         cut selected text in current textpage
     * Parameters:          none
     * Return Values:       none
     * Change on members:   none
    */
    void cut();

    /****************************************************************************************
     * Name:                copy
     * Description:         copy selected text of current textpage to clipboard
     * Parameters:          none
     * Return Values:       none
     * Change on members:   none
    */
    void copy();

    /****************************************************************************************
     * Name:                paste
     * Description:         paste text from clipboard to current textpage
     * Parameters:          none
     * Return Values:       none
     * Change on members:   none
    */
    void paste();

    /****************************************************************************************
     * Name:                undo
     * Description:         undo last change in current textpage
     * Parameters:          none
     * Return Values:       none
     * Change on members:   none
    */

    void undo();
    /****************************************************************************************
     * Name:                redo
     * Description:         redo last change in current textpage
     * Parameters:          none
     * Return Values:       none
     * Change on members:   none
    */
    void redo();

    /****************************************************************************************
     * Name:                selectAll
     * Description:         select all in current textpage
     * Parameters:          none
     * Return Values:       none
     * Change on members:   none
    */
    void selectAll();

protected:
    // void dragEnterEvent(QDragEnterEvent *event);
    // void dropEvent(QDropEvent* event);
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
