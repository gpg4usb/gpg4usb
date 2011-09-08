/*
 *      textpage.h
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

#ifndef EDITORPAGE_H
#define EDITORPAGE_H

#include <QPlainTextEdit>
#include <QtGui>

class QVBoxLayout;
class QHBoxLayout;
class QString;
class QLabel;

/**
 * @brief Class for handling a single tab of tabwidget
 *
 */
class EditorPage : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief
     *
     * @param filePath Path of the file handled in this tab
     * @param parent Pointer to the parent widget
     */
    EditorPage(const QString &filePath = "", QWidget *parent = 0);

    /**
     * @details Get the filepath of the currently activated tab.
     */
    const QString& getFilePath() const;

    /**
     * @details Set filepath of currently activated tab.
     *
     * @param filePath The path to be set
     */
    void setFilePath(const QString &filePath);

    /**
     * @details Return pointer tp the textedit of the currently activated tab.
     *
     */
    QPlainTextEdit *getTextPage();

    /**
     * @details Show additional widget at buttom of currently active tab
     *
     * @param widget The widget to be added
     * @param className The name to handle the added widget
     */
    void showNotificationWidget(QWidget *widget, const char *className);

    /**
     * @details Hide all widgets with the given className
     *
     * @param className The classname of the widgets to hide
     */
    void hideNoteByClass(const char *className);

private:
    QPlainTextEdit *textPage; /** The textedit of the tab */
    QVBoxLayout *mainLayout; /** The layout for the tab */
    QHBoxLayout *notificationWidgetLayout; /** layout for the notification-widget */
    QWidget *notificationWidget; /** The notification widget shown at the buttom of the tab */
    QMenu *verifyMenu; /** The menu in the notifiaction widget */
    QString fullFilePath; /** The path to the file handled in the tab */
    QLabel *verifyLabel; /** The label of the verify-notification widget */
    /**
     * @details bla
     *
     */
    void setSaveState();

};

#endif // TEXTPAGE_H
