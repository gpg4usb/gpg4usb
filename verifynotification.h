/*
 *      verifynotification.h
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

#ifndef VERIFYNOTIFICATION_H
#define VERIFYNOTIFICATION_H

#include "keyserverimportdialog.h"
#include "context.h"
#include <gpgme.h>
#include <QWidget>

class QLabel;
class QHBoxLayout;
class QMenu;
class QPushButton;

/**
 * @details Enumeration for the status of Verifylabel
 *
 */
typedef enum
{
    VERIFY_ERROR_OK = 0,
    VERIFY_ERROR_WARN = 1,
    VERIFY_ERROR_CRITICAL = 2,
}  verify_label_status;

/**
 * @brief Class for handling the verifylabel shown at buttom of a textedit-page
 *
 */
class VerifyNotification : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief
     *
     * @param ctx The GPGme-Context
     * @param parent The parent widget
     */
    explicit VerifyNotification(GpgME::Context *ctx,QWidget *parent = 0 );
    /**
     * @details Set the text and background-color of verify notification.
     *
     * @param text The text to be set.
     * @param verifyLabelStatus The status of label to set the specified color.
     */
    void setVerifyLabel(QString text, verify_label_status verifyLabelStatus);

    /**
     * @details Show the import from keyserver-action in detailsmenu.
     * @param visible show the action, if visible is true, otherwise hide it.
     */
    void showImportAction(bool visible);

    /**
     * @details Set the text of verify-detail dialog.
     *
     * @param text The text to be set.
     */
    void setVerifyDetailText(QString text);

    /****************************************************************************************
     * Name:                keysNotInList
     * Description:         List holding the keys in signature, which are not in the keylist
    */
    QStringList *keysNotInList;

signals:

public slots:
    // import missing key from keyserver
    /**
     * @brief
     *
     */
    void importFromKeyserver();
    // show verify details
    /**
     * @brief
     *
     */
    void showVerifyDetails();

private:
    QMenu *detailMenu; // Menu for te Button in verfiyNotification /**< TODO */
    QAction *importFromKeyserverAct; /**< TODO */
    QAction *showVerifyDetailsAct; /**< TODO */
    QString *verifyDetailText; // Text showed in VerifiyNotification /**< TODO */
    QPushButton *detailsButton; // Button shown in verifynotification /**< TODO */
    QLabel *verifyLabel; // Label holding the text shown in verifyNotification /**< TODO */
    GpgME::Context *mCtx; /**< TODO */
    QHBoxLayout *notificationWidgetLayout; /**< TODO */
};
#endif // VERIFYNOTIFICATION_H
