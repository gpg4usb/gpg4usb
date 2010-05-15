/*
 *      gpgwin.cpp
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

#include "gpgwin.h"
#include "fileencryptiondialog.h"
#include "settingsdialog.h"

GpgWin::GpgWin()
{
    mCtx = new GpgME::Context();
    keyMgmt = NULL;  // initialized on first key-management-window open

    /* get path were app was started */
    QString appPath = qApp->applicationDirPath();
    iconPath = appPath + "/icons/";

    edit = new QPlainTextEdit();
    setCentralWidget(edit);
//    setAcceptDrops(true);

    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);


    /* the list of Keys available*/
    mKeyList = new KeyList(mCtx, iconPath);

    /* List of binary Attachments */
    mAttachments = new Attachments(iconPath);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createDockWindows();
    setCurrentFile("");

    mKeyList->addMenuAction(appendSelectedKeysAct);
    restoreSettings();

    // open filename if provided as first command line parameter
    QStringList args = qApp->arguments();
    if (args.size() > 1) {
        if (!args[1].startsWith("-")) {
            if (QFile::exists(args[1]))
                loadFile(args[1]);
        }
    }

}

void GpgWin::restoreSettings()
{
    // state sets pos & size of dock-widgets
    this->restoreState(settings.value("window/windowState").toByteArray());

    // Restore window size & location
    if (settings.value("window/windowSave").toBool()) {
        QPoint pos = settings.value("window/pos", QPoint(100, 100)).toPoint();
        QSize size = settings.value("window/size", QSize(800, 450)).toSize();
        this->resize(size);
        this->move(pos);
    } else {
        this->resize(QSize(800, 450));
        this->move(QPoint(100, 100));
    }

    // Iconsize
    QSize iconSize = settings.value("toolbar/iconsize", QSize(32, 32)).toSize();
    this->setIconSize(iconSize);

    // Iconstyle
    Qt::ToolButtonStyle buttonStyle = static_cast<Qt::ToolButtonStyle>(settings.value("toolbar/iconstyle", Qt::ToolButtonTextUnderIcon).toUInt());
    this->setToolButtonStyle(buttonStyle);

    // Checked Keys
    if (settings.value("keys/keySave").toBool()) {
        QStringList keyIds = settings.value("keys/keyList").toStringList();
        mKeyList->setChecked(&keyIds);
    }
}

void GpgWin::createActions()
{
    /** Main Menu
      */
    openAct = new QAction(tr("&Open..."), this);
    openAct->setIcon(QIcon(iconPath + "fileopen.png"));
    openAct->setShortcut("Ctrl+O");
    openAct->setToolTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(tr("&Save"), this);
    saveAct->setIcon(QIcon(iconPath + "filesave.png"));
    saveAct->setShortcut("Ctrl+S");
    saveAct->setToolTip(tr("Save the current File"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As"), this);
    saveAsAct->setIcon(QIcon(iconPath + "filesaveas.png"));
    saveAsAct->setToolTip(tr("Save the current File as..."));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    printAct = new QAction(tr("&Print"), this);
    printAct->setIcon(QIcon(iconPath + "fileprint.png"));
    printAct->setShortcut("Ctrl+P");
    printAct->setToolTip(tr("Print Document"));
    connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

    quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcut("Ctrl+Q");
    quitAct->setIcon(QIcon(iconPath + "exit.png"));
    quitAct->setToolTip(tr("Quit Program"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    /** Edit Menu
     */
    pasteAct = new QAction(tr("&Paste"), this);
    pasteAct->setIcon(QIcon(iconPath + "button_paste.png"));
    pasteAct->setShortcut("Ctrl+V");
    pasteAct->setToolTip(tr("Paste Text From Clipboard"));
    connect(pasteAct, SIGNAL(triggered()), edit, SLOT(paste()));

    cutAct = new QAction(tr("Cu&t"), this);
    cutAct->setIcon(QIcon(iconPath + "button_cut.png"));
    cutAct->setShortcut("Ctrl+X");
    cutAct->setToolTip(tr("Cut the current selection's contents to the "
                          "clipboard"));
    connect(cutAct, SIGNAL(triggered()), edit, SLOT(cut()));

    copyAct = new QAction(tr("&Copy"), this);
    copyAct->setIcon(QIcon(iconPath + "button_copy.png"));
    copyAct->setShortcut("Ctrl+C");
    copyAct->setToolTip(tr("Copy the current selection's contents to the "
                           "clipboard"));
    connect(copyAct, SIGNAL(triggered()), edit, SLOT(copy()));

    selectallAct = new QAction(tr("Select &All"), this);
    selectallAct->setIcon(QIcon(iconPath + "edit.png"));
    selectallAct->setShortcut("Ctrl+A");
    selectallAct->setToolTip(tr("Select the whole text"));
    connect(selectallAct, SIGNAL(triggered()), edit, SLOT(selectAll()));

    openSettingsAct = new QAction(tr("Se&ttings"), this);
    openSettingsAct->setToolTip(tr("Open settings dialog"));
    connect(openSettingsAct, SIGNAL(triggered()), this, SLOT(openSettingsDialog()));

    /** Crypt Menu
     */
    encryptAct = new QAction(tr("&Encrypt"), this);
    encryptAct->setIcon(QIcon(iconPath + "encrypted.png"));
    encryptAct->setShortcut(tr("Ctrl+E"));
    encryptAct->setToolTip(tr("Encrypt Message"));
    connect(encryptAct, SIGNAL(triggered()), this, SLOT(encrypt()));

    decryptAct = new QAction(tr("&Decrypt"), this);
    decryptAct->setIcon(QIcon(iconPath + "decrypted.png"));
    decryptAct->setShortcut(tr("Ctrl+D"));
    decryptAct->setToolTip(tr("Decrypt Message"));
    connect(decryptAct, SIGNAL(triggered()), this, SLOT(decrypt()));

    fileEncryptionAct = new QAction(tr("&File Encryption"), this);
    fileEncryptionAct->setIcon(QIcon(iconPath + "fileencrytion.png"));
    fileEncryptionAct->setToolTip(tr("Encrypt/Decrypt File"));
    connect(fileEncryptionAct, SIGNAL(triggered()), this, SLOT(fileEncryption()));

    /** Key Menu
     */
    importKeyFromFileAct = new QAction(tr("&File"), this);
    importKeyFromFileAct->setIcon(QIcon(iconPath + "misc_doc.png"));
    importKeyFromFileAct->setToolTip(tr("Import New Key From File"));
    connect(importKeyFromFileAct, SIGNAL(triggered()), this, SLOT(importKeyFromFile()));

    importKeyFromEditAct = new QAction(tr("&Editor"), this);
    importKeyFromEditAct->setIcon(QIcon(iconPath + "txt.png"));
    importKeyFromEditAct->setToolTip(tr("Import New Key From Editor"));
    connect(importKeyFromEditAct, SIGNAL(triggered()), this, SLOT(importKeyFromEdit()));

    importKeyFromClipboardAct = new QAction(tr("&Clipboard"), this);
    importKeyFromClipboardAct->setIcon(QIcon(iconPath + "button_paste.png"));
    importKeyFromClipboardAct->setToolTip(tr("Import New Key From Clipboard"));
    connect(importKeyFromClipboardAct, SIGNAL(triggered()), this, SLOT(importKeyFromClipboard()));

    openKeyManagementAct = new QAction(tr("Key Management"), this);
    openKeyManagementAct->setIcon(QIcon(iconPath + "keymgmt.png"));
    openKeyManagementAct->setToolTip(tr("Open Keymanagement"));
    connect(openKeyManagementAct, SIGNAL(triggered()), this, SLOT(openKeyManagement()));

    importKeyDialogAct = new QAction(tr("Import Key"), this);
    importKeyDialogAct->setIcon(QIcon(iconPath + "key_import.png"));
    importKeyDialogAct->setToolTip(tr("Open Import New Key Dialog"));
    connect(importKeyDialogAct, SIGNAL(triggered()), this, SLOT(importKeyDialog()));

    /** About Menu
     */
    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setIcon(QIcon(iconPath + "help.png"));
    aboutAct->setToolTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    /** Popup-Menu-Action for KeyList
     */
    appendSelectedKeysAct = new QAction(tr("Append Selected Key(s) To Text"), this);
    appendSelectedKeysAct->setToolTip(tr("Append The Selected Keys To Text in Editor"));
    connect(appendSelectedKeysAct, SIGNAL(triggered()), this, SLOT(appendSelectedKeys()));
}

void GpgWin::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(copyAct);
    editMenu->addAction(cutAct);
    editMenu->addAction(pasteAct);
    editMenu->addAction(selectallAct);
    editMenu->addAction(openSettingsAct);

    cryptMenu = menuBar()->addMenu(tr("&Crypt"));
    cryptMenu->addAction(encryptAct);
    cryptMenu->addAction(decryptAct);
    cryptMenu->addSeparator();
    cryptMenu->addAction(fileEncryptionAct);

    keyMenu = menuBar()->addMenu(tr("&Keys"));
    importKeyMenu = keyMenu->addMenu(tr("&Import Key From..."));
    importKeyMenu->setIcon(QIcon(iconPath + "key_import.png"));
    importKeyMenu->addAction(importKeyFromFileAct);
    importKeyMenu->addAction(importKeyFromEditAct);
    importKeyMenu->addAction(importKeyFromClipboardAct);
    keyMenu->addAction(openKeyManagementAct);

    viewMenu = menuBar()->addMenu(tr("&View"));

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
}

void GpgWin::createToolBars()
{
    cryptToolBar = addToolBar(tr("Crypt"));
    cryptToolBar->addAction(encryptAct);
    cryptToolBar->addAction(decryptAct);
    cryptToolBar->addAction(fileEncryptionAct);
    viewMenu->addAction(cryptToolBar->toggleViewAction());

    keyToolBar = addToolBar(tr("Key"));
    keyToolBar->addAction(importKeyDialogAct);
    keyToolBar->addAction(openKeyManagementAct);
    viewMenu->addAction(keyToolBar->toggleViewAction());

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
    editToolBar->addAction(selectallAct);
    viewMenu->addAction(editToolBar->toggleViewAction());
}

void GpgWin::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void GpgWin::createDockWindows()
{
    /** KeyList-Dockwindow
     */
    dock = new QDockWidget(tr("Encrypt for:"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    dock->setWidget(mKeyList);
    viewMenu->addAction(dock->toggleViewAction());

    /** Attachments-Dockwindow
      */
    aDock = new QDockWidget(tr("Attached files:"), this);
    aDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, aDock);
    aDock->setWidget(mAttachments);
    // hide till attachmendt is decrypted
    viewMenu->addAction(aDock->toggleViewAction());
    aDock->hide();


}

void GpgWin::closeEvent(QCloseEvent *event)
{
    /** ask to save changes, if text modified
     */
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }

    /** Save the settings
     */
    // window position and size
    settings.setValue("window/windowState", saveState());
    settings.setValue("window/pos", pos());
    settings.setValue("window/size", size());

    // keyid-list of private checked keys
    if (settings.value("keys/keySave").toBool()) {
        QStringList *keyIds = mKeyList->getPrivateChecked();
        if (!keyIds->isEmpty()) {
            settings.setValue("keys/keyList", *keyIds);
        } else {
            settings.setValue("keys/keyList", "");
        }
    } else  {
        settings.remove("keys/keyList");
    }

    /********************
     * Quit programm
     * ******************/
    QMainWindow::closeEvent(event);
}

void GpgWin::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

bool GpgWin::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool GpgWin::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

void GpgWin::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    edit->setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}

void GpgWin::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    edit->document()->setModified(false);
    setWindowModified(false);

    QString shownName;
    if (curFile.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = strippedName(curFile);

    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(qApp->applicationName()));
}

QString GpgWin::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

bool GpgWin::maybeSave()
{
    if (edit->document()->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Application"),
                                   tr("The document has been modified.\nDo you want to save your changes?"),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

bool GpgWin::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("File"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << edit->toPlainText();
    QApplication::restoreOverrideCursor();
    statusBar()->showMessage(tr("Saved '%1'").arg(fileName), 2000);
    return true;
}

void GpgWin::print()
{
#ifndef QT_NO_PRINTER
    QTextDocument *document = edit->document();
    QPrinter printer;

    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (dlg->exec() != QDialog::Accepted)
        return;

    document->print(&printer);

    statusBar()->showMessage(tr("Ready"), 2000);
#endif
}

void GpgWin::about()
{
    QMessageBox::about(this, tr("About ") + qApp->applicationName(),
                       "<center><h2>" + qApp->applicationName() + " "
                       + qApp->applicationVersion() + "</h2></center>"
                       + tr("<center>This Application allows you to do simple<br>"
                            "encryption/decryption of your text-message or file.<br>"
                            "It's licensed under the GPL v2.0<br><br>"
                            "<b>Developer:</b><br>"
                            "Bene, Heimer, Juergen, Nils, Ubbo<br><br>"
                            "<b>Translation:</b><br>"
                            "Alessandro (pt_br), Alex (fr), Kirill (ru), Viriato (es)<br><br>"
                            "If you have any questions and/or<br>"
                            "suggestions, contact us at<br>"
                            "gpg4usb at cpunk.de</a><br><br>"
                            "or feel free to meet us in our xmpp-channel:<br>"
                            "gpg4usb at conference.jabber.ccc.de</center>"));
}

void GpgWin::encrypt()
{
    QStringList *uidList = mKeyList->getChecked();

    QByteArray *tmp = new QByteArray();
    if (mCtx->encrypt(uidList, edit->toPlainText().toUtf8(), tmp)) {
        QString *tmp2 = new QString(*tmp);
        edit->setPlainText(*tmp2);
    }
}

void GpgWin::decrypt()
{
    QByteArray *tmp = new QByteArray();
    QByteArray text = edit->toPlainText().toAscii();
    preventNoDataErr(&text);
    mCtx->decrypt(text, tmp);
    if (!tmp->isEmpty()) {
        // is it mime?
        if (settings.value("mime/parseMime").toBool()) {
            parseMime(tmp);
        }
        edit->setPlainText(QString::fromUtf8(*tmp));
    }
}

/**
  * if this is mime, split text and attachments...
  * message contains only text afterwards
  */
void GpgWin::parseMime(QByteArray *message)
{

    if (! Mime::isMultipart(message)) {
        qDebug() << "no multipart";
        return;
    }
    qDebug() << "multipart";

    QString pText;
    bool showmadock = false;

    Mime *mime = new Mime(message);
    foreach(MimePart tmp, mime->parts()) {
        if (tmp.getValue("Content-Type") == "text/plain"
                && tmp.getValue("Content-Transfer-Encoding") != "base64") {

            QByteArray body;
            if (tmp.getValue("Content-Transfer-Encoding") == "quoted-printable") {

                Mime::quotedPrintableDecode(tmp.body, body);
            } else {
                body = tmp.body;
            }

            pText.append(QString(body));

        } else {
            (mAttachments->addMimePart(&tmp));
            showmadock = true;
        }
    }

    *message = pText.toAscii();
    if (showmadock) aDock->show();
}

/**
 * if there is no '\n' before the PGP-Begin-Block, but for example a whitespace,
 * GPGME doesn't recognise the Message as encrypted. This function adds '\n'
 * before the PGP-Begin-Block, if missing.
 */
void GpgWin::preventNoDataErr(QByteArray *in)
{
    int block_start = in->indexOf("-----BEGIN PGP MESSAGE-----");
    if (block_start > 0 && in->at(block_start - 1) != '\n') {
        in->insert(block_start, '\n');
    }
}

void GpgWin::importKeyFromEdit()
{
    mCtx->importKey(edit->toPlainText().toAscii());
}

void GpgWin::importKeyFromClipboard()
{
    QClipboard *cb = QApplication::clipboard();
    mCtx->importKey(cb->text(QClipboard::Clipboard).toAscii());
}

void GpgWin::importKeyFromFile()
{
    QFile file;
    QByteArray inBuffer;

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Key"), "", tr("Key Files") + " (*.asc *.txt);;" + tr("All Files") + " (*)");
    if (! fileName.isNull()) {
        file.setFileName(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << tr("couldn't open file: ") + fileName;
        }
        QByteArray inBuffer = file.readAll();
        mCtx->importKey(inBuffer);
    }
}


void GpgWin::openKeyManagement()
{
    if (!keyMgmt) {
        keyMgmt = new KeyMgmt(mCtx, iconPath);
//        keyMgmt->resize(800, 400);
    }
    keyMgmt->show();
    keyMgmt->raise();
    keyMgmt->activateWindow();
}

void GpgWin::importKeyDialog()
{

    QDialog *dialog = new QDialog();

    dialog->setWindowTitle(tr("Import Key"));
    dialog->setModal(true);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), dialog, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), dialog, SLOT(reject()));

    QGroupBox *groupBox = new QGroupBox(tr("Import Key From..."));
    QRadioButton *radio1 = new QRadioButton(tr("&File"));
    QRadioButton *radio2 = new QRadioButton(tr("&Editor"));
    QRadioButton *radio3 = new QRadioButton(tr("&Clipboard"));
    radio1->setChecked(true);

    QVBoxLayout *vbox1 = new QVBoxLayout();
    vbox1->addWidget(radio1);
    vbox1->addWidget(radio2);
    vbox1->addWidget(radio3);

    groupBox->setLayout(vbox1);

    QVBoxLayout *vbox2 = new QVBoxLayout();
    vbox2->addWidget(groupBox);
    vbox2->addWidget(buttonBox);

    dialog->setLayout(vbox2);

    if (dialog->exec() == QDialog::Accepted) {
        if (radio1->isChecked()) importKeyFromFile();
        if (radio2->isChecked()) importKeyFromEdit();
        if (radio3->isChecked()) importKeyFromClipboard();
    }

}

/**
 * Append the selected (not checked!) Key(s) To Textedit
 */
void GpgWin::appendSelectedKeys()
{
    QByteArray *keyArray = new QByteArray();

    mCtx->exportKeys(mKeyList->getSelected(), keyArray);
    edit->appendPlainText(*keyArray);
}

void GpgWin::fileEncryption()
{
    new FileEncryptionDialog(mCtx, iconPath, this);

}
void GpgWin::openSettingsDialog()
{
    new SettingsDialog(this);
//  restoreSettings();
    // Iconsize
    QSize iconSize = settings.value("toolbar/iconsize", QSize(32, 32)).toSize();
    this->setIconSize(iconSize);

    // Iconstyle
    Qt::ToolButtonStyle buttonStyle = static_cast<Qt::ToolButtonStyle>(settings.value("toolbar/iconstyle", Qt::ToolButtonTextUnderIcon).toUInt());
    this->setToolButtonStyle(buttonStyle);
}
