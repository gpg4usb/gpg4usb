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

    edit = new TextEdit();
    setCentralWidget(edit);

    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);


    /* the list of Keys available*/
    mKeyList = new KeyList(mCtx, iconPath);

    /* List of binary Attachments */
    mAttachments = new Attachments(iconPath);
    
    /* test attachmentdir for files alll 15s */
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkAttachmentFolder()));
    timer->start(5000);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createDockWindows();
    edit->setCurrentFile("");

    mKeyList->addMenuAction(appendSelectedKeysAct);
    restoreSettings();

    // open filename if provided as first command line parameter
    QStringList args = qApp->arguments();
    if (args.size() > 1) {
        if (!args[1].startsWith("-")) {
            if (QFile::exists(args[1]))
                edit->loadFile(args[1]);
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

void GpgWin::saveSettings()
{
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
}

void GpgWin::createActions()
{
    /** Main Menu
      */
    openAct = new QAction(tr("&Open..."), this);
    openAct->setIcon(QIcon(iconPath + "fileopen.png"));
    openAct->setShortcut(QKeySequence::Open);
    openAct->setToolTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), edit, SLOT(open()));

    saveAct = new QAction(tr("&Save"), this);
    saveAct->setIcon(QIcon(iconPath + "filesave.png"));
    saveAct->setShortcut(QKeySequence::Save);
    saveAct->setToolTip(tr("Save the current File"));
    connect(saveAct, SIGNAL(triggered()), edit, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As"), this);
    saveAsAct->setIcon(QIcon(iconPath + "filesaveas.png"));
    saveAsAct->setShortcut(QKeySequence::SaveAs);
    saveAsAct->setToolTip(tr("Save the current File as..."));
    connect(saveAsAct, SIGNAL(triggered()), edit, SLOT(saveAs()));

    printAct = new QAction(tr("&Print"), this);
    printAct->setIcon(QIcon(iconPath + "fileprint.png"));
    printAct->setShortcut(QKeySequence::Print);
    printAct->setToolTip(tr("Print Document"));
    connect(printAct, SIGNAL(triggered()), edit, SLOT(print()));

    quitAct = new QAction(tr("&Quit"), this);
//    quitAct->setShortcut(QKeySequence::Quit);
    quitAct->setIcon(QIcon(iconPath + "exit.png"));
    quitAct->setToolTip(tr("Quit Program"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    /** Edit Menu
     */
    undoAct = new QAction(tr("&Undo"), this);
    undoAct->setShortcut(QKeySequence::Undo);
    undoAct->setToolTip(tr("Undo Last Edit Action"));
    connect(undoAct, SIGNAL(triggered()), edit, SLOT(undo()));

    redoAct = new QAction(tr("&Redo"), this);
    redoAct->setShortcut(QKeySequence::Redo);
    redoAct->setToolTip(tr("Redo Last Edit Action"));
    connect(redoAct, SIGNAL(triggered()), edit, SLOT(redo()));

    pasteAct = new QAction(tr("&Paste"), this);
    pasteAct->setIcon(QIcon(iconPath + "button_paste.png"));
    pasteAct->setShortcut(QKeySequence::Paste);
    pasteAct->setToolTip(tr("Paste Text From Clipboard"));
    connect(pasteAct, SIGNAL(triggered()), edit, SLOT(paste()));

    cutAct = new QAction(tr("Cu&t"), this);
    cutAct->setIcon(QIcon(iconPath + "button_cut.png"));
    cutAct->setShortcut(QKeySequence::Cut);
    cutAct->setToolTip(tr("Cut the current selection's contents to the "
                          "clipboard"));
    connect(cutAct, SIGNAL(triggered()), edit, SLOT(cut()));

    copyAct = new QAction(tr("&Copy"), this);
    copyAct->setIcon(QIcon(iconPath + "button_copy.png"));
    copyAct->setShortcut(QKeySequence::Copy);
    copyAct->setToolTip(tr("Copy the current selection's contents to the "
                           "clipboard"));
    connect(copyAct, SIGNAL(triggered()), edit, SLOT(copy()));

    quoteAct = new QAction(tr("&Quote"), this);
    quoteAct->setIcon(QIcon(iconPath + "quote.png"));
    quoteAct->setToolTip(tr("Insert \">\" in front of every line"));
    connect(quoteAct, SIGNAL(triggered()), edit, SLOT(quote()));

    selectallAct = new QAction(tr("Select &All"), this);
    selectallAct->setIcon(QIcon(iconPath + "edit.png"));
    selectallAct->setShortcut(QKeySequence::SelectAll);
    selectallAct->setToolTip(tr("Select the whole text"));
    connect(selectallAct, SIGNAL(triggered()), edit, SLOT(selectAll()));

    openSettingsAct = new QAction(tr("Se&ttings"), this);
    openSettingsAct->setToolTip(tr("Open settings dialog"));
//    openSettingsAct->setShortcut(QKeySequence::Preferences);
    connect(openSettingsAct, SIGNAL(triggered()), this, SLOT(openSettingsDialog()));

    /** Crypt Menu
     */
    encryptAct = new QAction(tr("&Encrypt"), this);
    encryptAct->setIcon(QIcon(iconPath + "encrypted.png"));
    encryptAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
    encryptAct->setToolTip(tr("Encrypt Message"));
    connect(encryptAct, SIGNAL(triggered()), this, SLOT(encrypt()));

    decryptAct = new QAction(tr("&Decrypt"), this);
    decryptAct->setIcon(QIcon(iconPath + "decrypted.png"));
    decryptAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
    decryptAct->setToolTip(tr("Decrypt Message"));
    connect(decryptAct, SIGNAL(triggered()), this, SLOT(decrypt()));

    fileEncryptionAct = new QAction(tr("&File Encryption"), this);
    fileEncryptionAct->setIcon(QIcon(iconPath + "fileencrytion.png"));
    fileEncryptionAct->setToolTip(tr("Encrypt/Decrypt File"));
    connect(fileEncryptionAct, SIGNAL(triggered()), this, SLOT(fileEncryption()));

    signAct = new QAction(tr("&Sign"), this);
    //signAct->setIcon(QIcon(iconPath + "encrypted.png"));
    //signAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
    signAct->setToolTip(tr("Sign Message"));
    connect(signAct, SIGNAL(triggered()), this, SLOT(sign()));

    verifyAct = new QAction(tr("&Verify"), this);
    //signAct->setIcon(QIcon(iconPath + "encrypted.png"));
    //signAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
    verifyAct->setToolTip(tr("Verify Message"));
    connect(verifyAct, SIGNAL(triggered()), this, SLOT(verify()));


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

    openTutorialAct = new QAction(tr("Online &Tutorial"), this);
    openTutorialAct->setIcon(QIcon(iconPath + "help.png"));
    openTutorialAct->setToolTip(tr("Open Online Tutorial"));
    connect(openTutorialAct, SIGNAL(triggered()), this, SLOT(openTutorial()));

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
    editMenu->addAction(redoAct);
    editMenu->addAction(undoAct);
    editMenu->addSeparator();
    editMenu->addAction(copyAct);
    editMenu->addAction(cutAct);
    editMenu->addAction(pasteAct);
    editMenu->addAction(selectallAct);
    editMenu->addAction(quoteAct);
    editMenu->addSeparator();
    editMenu->addAction(openSettingsAct);

    cryptMenu = menuBar()->addMenu(tr("&Crypt"));
    cryptMenu->addAction(encryptAct);
    cryptMenu->addAction(decryptAct);
    /*cryptMenu->addSeparator();
    cryptMenu->addAction(signAct);
    cryptMenu->addAction(verifyAct);*/
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
    helpMenu->addAction(openTutorialAct);
    helpMenu->addAction(aboutAct);
}

void GpgWin::createToolBars()
{
    cryptToolBar = addToolBar(tr("Crypt"));
    cryptToolBar->setObjectName("cryptToolBar");
    cryptToolBar->addAction(encryptAct);
    cryptToolBar->addAction(decryptAct);
    cryptToolBar->addAction(fileEncryptionAct);
    viewMenu->addAction(cryptToolBar->toggleViewAction());

    keyToolBar = addToolBar(tr("Key"));
    keyToolBar->setObjectName("keyToolBar");
    keyToolBar->addAction(importKeyDialogAct);
    keyToolBar->addAction(openKeyManagementAct);
    viewMenu->addAction(keyToolBar->toggleViewAction());

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->setObjectName("editToolBar");
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
    editToolBar->addAction(selectallAct);
    editToolBar->addAction(quoteAct);
    viewMenu->addAction(editToolBar->toggleViewAction());
}

void GpgWin::createStatusBar()
{
    QWidget *statusBarBox = new QWidget();
    QHBoxLayout *statusBarBoxLayout = new QHBoxLayout();

    QPixmap *pixmap;

    // icon which should be shown if there are files in attachments-folder
    pixmap = new QPixmap(iconPath + "statusbar_icon.png");
    statusBarIcon = new QLabel(statusBar());
    statusBarIcon->setPixmap(*pixmap);
    statusBar()->insertPermanentWidget(0,statusBarIcon,0);
    statusBarIcon->hide();  
        
    statusBar()->showMessage(tr("Ready"),2000);
    statusBarBox->setLayout(statusBarBoxLayout);

}

void GpgWin::createDockWindows()
{
    /** KeyList-Dockwindow
     */
    dock = new QDockWidget(tr("Encrypt for:"), this);
    dock->setObjectName("EncryptDock");
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    dock->setWidget(mKeyList);
    viewMenu->addAction(dock->toggleViewAction());

    /** Attachments-Dockwindow
      */
    aDock = new QDockWidget(tr("Attached files:"), this);
    aDock->setObjectName("AttachmentDock");
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
    if (edit->maybeSave()) {
        saveSettings();
        event->accept();
    } else {
        event->ignore();
    }

    // clear password from memory
    mCtx->clearPasswordCache();

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
                            "Alessandro (pt_br), Alex (fr), Kirill (ru), Viriato (es), Serse (it) <br><br>"
                            "If you have any questions and/or<br>"
                            "suggestions, contact us at<br>"
                            "gpg4usb at cpunk.de</a><br><br>"
                            "or feel free to meet us in our xmpp-channel:<br>"
                            "gpg4usb at conference.jabber.ccc.de</center>"));
}

void GpgWin::openTutorial() {
    QDesktopServices::openUrl(QUrl("http://gpg4usb.cpunk.de/docu.html"));
}

void GpgWin::encrypt()
{
    QStringList *uidList = mKeyList->getChecked();

    QByteArray *tmp = new QByteArray();
//    if (mCtx->encrypt(uidList, edit.curTextPage.toPlainText().toUtf8(), tmp)) {
    if (mCtx->encrypt(uidList, edit->curTextPage()->toPlainText().toUtf8(), tmp)) {
        QString *tmp2 = new QString(*tmp);
        edit->curTextPage()->setPlainText(*tmp2);
    }
}

void GpgWin::decrypt()
{
    QByteArray *decrypted = new QByteArray();
    QByteArray text = edit->curTextPage()->toPlainText().toAscii(); // TODO: toUtf8() here?
    preventNoDataErr(&text);
    mCtx->decrypt(text, decrypted);
    if (!decrypted->isEmpty()) {

        /**
         *   1) is it mime (content-type:)
         *   2) parse header
         *   2) choose action depending on content-type
         */

        if(Mime::isMime(decrypted)) {
            Header header = Mime::getHeader(decrypted);

            // is it multipart, is multipart-parsing enabled
            if(header.getValue("Content-Type") == "multipart/mixed"
               && settings.value("mime/parseMime").toBool()) {

                    parseMime(decrypted);

            } else if(header.getValue("Content-Type") == "text/plain"
               && settings.value("mime/parseQP").toBool()){

                    if (header.getValue("Content-Transfer-Encoding") == "quoted-printable") {
                        QByteArray *decoded = new QByteArray();
                        Mime::quotedPrintableDecode(*decrypted, *decoded);
                        //TODO: remove header
                        decrypted = decoded;

                    }
            }
        }

        edit->curTextPage()->setPlainText(QString::fromUtf8(*decrypted));
        //edit->setPlainText(*decrypted);
    }
}

/**
  * if this is mime, split text and attachments...
  * message contains only text afterwards
  */
void GpgWin::parseMime(QByteArray *message)
{

    /*if (! Mime::isMultipart(message)) {
        qDebug() << "no multipart";
        return;
    }*/
    //qDebug() << "multipart";

    QString pText;
    bool showmadock = false;

    Mime *mime = new Mime(message);
    foreach(MimePart tmp, mime->parts()) {
        if (tmp.header.getValue("Content-Type") == "text/plain"
                && tmp.header.getValue("Content-Transfer-Encoding") != "base64") {

            QByteArray body;
            if (tmp.header.getValue("Content-Transfer-Encoding") == "quoted-printable") {

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

    *message = pText.toUtf8();
    if (showmadock) {
		aDock->show();
	}
}

void GpgWin::checkAttachmentFolder() {
    // TODO: always check?
    if(!settings.value("mime/parseMime").toBool()) {
        return;
    }
    
    QString attachmentDir = qApp->applicationDirPath() + "/attachments/";
    // filenum minus . and ..
    int filenum = QDir(attachmentDir).count() - 2 ;
    if(filenum > 0) {
        QString statusText;
        if(filenum == 1) {
            statusText = tr("There is one unencrypted file in attachment folder");
        } else {
            statusText = tr("There are ") + QString::number(filenum) +  tr(" unencrypted files in attachment folder");
        }
        statusBarIcon->setStatusTip(statusText);
        statusBarIcon->show();
    } else {
        statusBarIcon->hide();
    }

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
    mCtx->importKey(edit->curTextPage()->toPlainText().toAscii());
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

void GpgWin::sign() {
    // test-stuff that does not belong here ;-)
    //mCtx->verify(QByteArray());
    //mCtx->sign(QByteArray(), new QByteArray());
    // end of test

    QStringList *uidList = mKeyList->getChecked();

    QByteArray *tmp = new QByteArray();
    if (mCtx->sign(uidList, edit->curTextPage()->toPlainText().toUtf8(), tmp)) {
        QString *tmp2 = new QString(*tmp);
        edit->curTextPage()->setPlainText(*tmp2);
    }
}

void GpgWin::verify() {

    mCtx->verify(edit->curTextPage()->toPlainText().toUtf8());

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
    edit->curTextPage()->appendPlainText(*keyArray);
}

void GpgWin::fileEncryption()
{
	QStringList *keyList;
	keyList = mKeyList->getChecked();
    new FileEncryptionDialog(mCtx, iconPath, *keyList, this);
}

void GpgWin::openSettingsDialog()
{
    new SettingsDialog(this);
    // Iconsize
    QSize iconSize = settings.value("toolbar/iconsize", QSize(32, 32)).toSize();
    this->setIconSize(iconSize);

    // Iconstyle
    Qt::ToolButtonStyle buttonStyle = static_cast<Qt::ToolButtonStyle>(settings.value("toolbar/iconstyle", Qt::ToolButtonTextUnderIcon).toUInt());
    this->setToolButtonStyle(buttonStyle);
}
