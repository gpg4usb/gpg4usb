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
#include "verifydetailsdialog.h"
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
    VERIFY_ERROR_NEUTRAL =3,
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
    explicit VerifyNotification(QWidget *parent, GpgME::Context *ctx, KeyList *keyList, gpgme_signature_t sign);
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

    QStringList *keysNotInList; /** List with keys, which are in signature but not in keylist */

    /**
      * @details add text to the verifyDetailStringVector and associated status to the
      * verifyDetailStatusVector
      * @param text The text to be added
      * @param status The status to be set
      * @param prepend If prepend is true, prepend to the vectors,
      *  otherwise append to the vectors
      */
    void addVerifyDetailLabel(QString text, verify_label_status status, bool prepend);

public slots:
    /**
     * @details Import the keys contained in keysNotInList from keyserver
     *
     */
    void importFromKeyserver();

    /**
     * @details Show a dialog with signing details.
     */
    void showVerifyDetails();

private:
    QMenu *detailMenu; /** Menu for te Button in verfiyNotification */
    QAction *importFromKeyserverAct; /** Action for importing keys from keyserver which are notin keylist */
    QAction *showVerifyDetailsAct; /** Action for showing verify detail dialog */
    QPushButton *detailsButton; /** Button shown in verifynotification */
    QLabel *verifyLabel; /** Label holding the text shown in verifyNotification */
    GpgME::Context *mCtx; /** GpgME Context */
    KeyList *mKeyList;
    gpgme_signature_t mSignature;
    QHBoxLayout *notificationWidgetLayout; /** Layout for verify-notification */
    QVBoxLayout *verifyDetailListLayout;  /** Layout for verify-detail-dialog */
    QVector<QString> verifyDetailStringVector; /** Vector containing the text for labels in verifydetaildialog */
    QVector<verify_label_status> verifyDetailStatusVector; /** Vector containing the status for labels in verifydetaildialog */
};
#endif // VERIFYNOTIFICATION_H
