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

/**
 * @brief TextEdit class
 */
class TextEdit : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief
     *
     * @param iconPath
     */
    TextEdit(QString iconPath);

    /**
     * @brief Load the content of file into the current textpage
     *
     * @param fileName QString containing the filename to load
     * @return nothing
     */
    void loadFile(const QString &fileName);


    /**
     * @details Checks if there are unsaved documents in any tab,
     *  which may need to be saved. Call this function before
     *  closing the programme or all tabs.
     * @return \li false, if the close event should be aborted.
     *         \li true, otherwise
     */
    bool maybeSaveAnyTab();

    /**
     * @details textpage of the currently activated tab
     */
    QPlainTextEdit* curTextPage();

    /**
     * @details  List of currently unsaved tabs
     * @returns QHash<int, QString> Hash of tabindexes and title of unsaved tabs
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
    /**
     * @brief
     *
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
    /**
     * @brief
     *
     */
    void save();

    /****************************************************************************************
     * Name:                SaveAs
     * Description:         Opens a savefiledialog and calls saveFile with the choosen filename
     * Parameters:          none
     * Return Values:       just returns the return value of the saveFile method
     * Change on members:   none
    */
    /**
     * @brief
     *
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
    /**
     * @brief
     *
     */
    void open();

    /****************************************************************************************
     * Name:                print
     * Description:         opens print dialog for the current tab
     * Parameters:          none
     * Return Values:       none
     * Change on members:   none
    */
    /**
     * @brief
     *
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
    /**
     * @details Adds a new tab with the title "untitled"+countpage+".txt"
     *                      Sets the focus to the new tab. Increase Tab-Count by one
     */
    void newTab();

    /**
     * @details put a * in front of current tabs title, if current textedit is modified
     */
    void showModified();

    /**
     * @details close the current tab and decrease TabWidget->count by \a 1
     *
     */
    void closeTab();
    /****************************************************************************************
     * Name:                switchTabUp
     * Description:         switch to next tab
     * Parameters:          none
     * Return Values:       none
     * Change on members:   increase tabWidget->count() per 1
    */
    /**
     * @brief
     *
     */
    void switchTabUp();
    /****************************************************************************************
     * Name:                switchTabDown
     * Description:         switch to next tab
     * Parameters:          none
     * Return Values:       none
     * Change on members:   decrease tabWidget->count() per 1
    */
    /**
     * @brief
     *
     */
    void switchTabDown();
    /****************************************************************************************
     * Name:                curPage
     * Description:         return pointer to the currently activated tabpage
     * Parameters:          none
     * Return Values:       pointer to the currently activated tabpage
     * Change on members:   none
    */
    /**
     * @brief
     *
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
    /**
     * @brief
     *
     * @param fullFileName
     */
    QString strippedName(const QString &fullFileName);
    /**
     * @brief
     *
     */
    bool maybeSaveFile();
    /**
     * @brief
     *
     * @param askToSave
     */
    bool maybeSaveCurrentTab(bool askToSave);

    QString mIconPath; /* TODO */
    /****************************************************************************************
     * Name:                countPage
     * Description:         int cotaining the number of added tabs
    */
    int countPage; /* TODO */
    QTabWidget *tabWidget; /* TODO */

private slots:
    /****************************************************************************************
     * Name:                removeTab
     * Description:         Removes the tab on index
     * Parameters:          int index, shows the index of the tab to remove
     * Return Values:       none
     * Change on members:   none
    */
    /**
     * @brief
     *
     * @param index
     */
    void removeTab(int index);

    /****************************************************************************************
     * Name:                cut
     * Description:         cut selected text in current textpage
     * Parameters:          none
     * Return Values:       none
     * Change on members:   none
    */
    /**
     * @brief
     *
     */
    void cut();

    /****************************************************************************************
     * Name:                copy
     * Description:         copy selected text of current textpage to clipboard
     * Parameters:          none
     * Return Values:       none
     * Change on members:   none
    */
    /**
     * @brief
     *
     */
    void copy();

    /****************************************************************************************
     * Name:                paste
     * Description:         paste text from clipboard to current textpage
     * Parameters:          none
     * Return Values:       none
     * Change on members:   none
    */
    /**
     * @brief
     *
     */
    void paste();

    /****************************************************************************************
     * Name:                undo
     * Description:         undo last change in current textpage
     * Parameters:          none
     * Return Values:       none
     * Change on members:   none
    */

    /**
     * @brief
     *
     */
    void undo();
    /****************************************************************************************
     * Name:                redo
     * Description:         redo last change in current textpage
     * Parameters:          none
     * Return Values:       none
     * Change on members:   none
    */
    /**
     * @brief
     *
     */
    void redo();

    /****************************************************************************************
     * Name:                selectAll
     * Description:         select all in current textpage
     * Parameters:          none
     * Return Values:       none
     * Change on members:   none
    */
    /**
     * @brief
     *
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
    /**
     * @brief
     *
     * @param fileName
     */
    bool saveFile(const QString &fileName);
};
#endif // TEXTEDIT
