/*
 *      gpgwin.cpp
 *
 *      Copyright 2008 gpg4usb-team <gpg4usb@cpunk.de>
 *
 *      This file is part of gpg4usb.
 *
 *      Gpg4usb is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *
 *      Gpg4usb is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with gpg4usb.  If not, see <http://www.gnu.org/licenses/>
 */

#include "mainwindow.h"

MainWindow::MainWindow()
{
    mCtx = new GpgME::GpgContext();

    /* get path were app was started */
    QString appPath = qApp->applicationDirPath();
    iconPath = appPath + "/icons/";

    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    edit = new TextEdit(iconPath);
    setCentralWidget(edit);

    /* the list of Keys available*/
    mKeyList = new KeyList(mCtx, iconPath);

    /* List of binary Attachments */
    attachmentDockCreated = false;

    keyMgmt = new KeyMgmt(mCtx, iconPath);
    keyMgmt->hide();

    /* test attachmentdir for files alll 15s */
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkAttachmentFolder()));
    timer->start(5000);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createDockWindows();

    mKeyList->addMenuAction(appendSelectedKeysAct);
    mKeyList->addMenuAction(copyMailAddressToClipboardAct);
    mKeyList->addMenuAction(showKeyDetailsAct);

    restoreSettings();

    // open filename if provided as first command line parameter
    QStringList args = qApp->arguments();
    if (args.size() > 1) {
        if (!args[1].startsWith("-")) {
            if (QFile::exists(args[1]))
                edit->loadFile(args[1]);
        }
    }
    edit->curTextPage()->setFocus();
    this->setWindowTitle(qApp->applicationName());
    this->show();

    // Show wizard, if the don't show wizard message box wasn't checked
    // and keylist doesn't contain a private key
    QSettings settings;
    if (settings.value("wizard/showWizard",true).toBool() && !mKeyList->containsPrivateKeys()) {
        startWizard();
    }
}

void MainWindow::restoreSettings()
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

    // set list of keyserver if not defined
    QStringList *keyServerDefaultList;
    keyServerDefaultList = new QStringList("http://gpg-keyserver.de");
    keyServerDefaultList->append("http://pgp.mit.edu");
    keyServerDefaultList->append("http://pool.sks-keyservers.net");
    keyServerDefaultList->append("http://subkeys.pgp.net");

    QStringList keyServerList = settings.value("keyserver/keyServerList", *keyServerDefaultList).toStringList();
    settings.setValue("keyserver/keyServerList", keyServerList);

    // set default keyserver, if it's not set
    QString defaultKeyServer = settings.value("keyserver/defaultKeyServer", QString("http://pgp.mit.edu")).toString();
    settings.setValue("keyserver/defaultKeyServer", defaultKeyServer);

    // Iconstyle
    Qt::ToolButtonStyle buttonStyle = static_cast<Qt::ToolButtonStyle>(settings.value("toolbar/iconstyle", Qt::ToolButtonTextUnderIcon).toUInt());
    this->setToolButtonStyle(buttonStyle);

    // Checked Keys
    if (settings.value("keys/keySave").toBool()) {
        QStringList keyIds = settings.value("keys/keyList").toStringList();
        mKeyList->setChecked(&keyIds);
    }
}

void MainWindow::saveSettings()
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

void MainWindow::createActions()
{
    /* Main Menu
      */
    newTabAct = new QAction(tr("&New"), this);
    newTabAct->setIcon(QIcon(iconPath + "misc_doc.png"));
    QList<QKeySequence> newTabActShortcutList;
    newTabActShortcutList.append(QKeySequence (Qt::CTRL + Qt::Key_N));
    newTabActShortcutList.append(QKeySequence (Qt::CTRL + Qt::Key_T));
    newTabAct->setShortcuts(newTabActShortcutList);
    newTabAct->setToolTip(tr("Open a new file"));
    connect(newTabAct, SIGNAL(triggered()), edit, SLOT(newTab()));

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

    saveAsAct = new QAction(tr("Save &As")+"...", this);
    saveAsAct->setIcon(QIcon(iconPath + "filesaveas.png"));
    saveAsAct->setShortcut(QKeySequence::SaveAs);
    saveAsAct->setToolTip(tr("Save the current File as..."));
    connect(saveAsAct, SIGNAL(triggered()), edit, SLOT(saveAs()));

    printAct = new QAction(tr("&Print"), this);
    printAct->setIcon(QIcon(iconPath + "fileprint.png"));
    printAct->setShortcut(QKeySequence::Print);
    printAct->setToolTip(tr("Print Document"));
    connect(printAct, SIGNAL(triggered()), edit, SLOT(print()));

    closeTabAct = new QAction(tr("&Close"), this);
    closeTabAct->setShortcut(QKeySequence::Close);
    closeTabAct->setToolTip(tr("Close file"));
    connect(closeTabAct, SIGNAL(triggered()), edit, SLOT(closeTab()));

    quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcut(QKeySequence::Quit);
    quitAct->setIcon(QIcon(iconPath + "exit.png"));
    quitAct->setToolTip(tr("Quit Program"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    /* Edit Menu
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
    quoteAct->setToolTip(tr("Quote whole text"));
    connect(quoteAct, SIGNAL(triggered()), edit, SLOT(quote()));

    selectallAct = new QAction(tr("Select &All"), this);
    selectallAct->setIcon(QIcon(iconPath + "edit.png"));
    selectallAct->setShortcut(QKeySequence::SelectAll);
    selectallAct->setToolTip(tr("Select the whole text"));
    connect(selectallAct, SIGNAL(triggered()), edit, SLOT(selectAll()));

    cleanDoubleLinebreaksAct = new QAction(tr("Remove double &Linebreaks"), this);
    //cleanDoubleLineBreaksAct->setIcon(QIcon(iconPath + "edit.png"));
    //cleanDoubleLineBreaksAct->setShortcut(QKeySequence::SelectAll);
    cleanDoubleLinebreaksAct->setToolTip(tr("Remove double linebreaks, e.g. in pasted text from webmailer"));
    connect(cleanDoubleLinebreaksAct, SIGNAL(triggered()), this, SLOT(cleanDoubleLinebreaks()));

    openSettingsAct = new QAction(tr("Se&ttings"), this);
    openSettingsAct->setToolTip(tr("Open settings dialog"));
    openSettingsAct->setShortcut(QKeySequence::Preferences);
    connect(openSettingsAct, SIGNAL(triggered()), this, SLOT(openSettingsDialog()));

    /* Crypt Menu
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

    /*
     * File encryption submenu
     */
    fileEncryptAct = new QAction(tr("&Encrypt"), this);
    //fileEncryptAct->setIcon(QIcon(iconPath + "fileencrytion.png"));
    fileEncryptAct->setToolTip(tr("Encrypt File"));
    connect(fileEncryptAct, SIGNAL(triggered()), this, SLOT(fileEncrypt()));

    fileDecryptAct = new QAction(tr("&Decrypt"), this);
    //fileDecryptAct->setIcon(QIcon(iconPath + "fileencrytion.png"));
    fileDecryptAct->setToolTip(tr("Decrypt File"));
    connect(fileDecryptAct, SIGNAL(triggered()), this, SLOT(fileDecrypt()));

    signAct = new QAction(tr("&Sign"), this);
    signAct->setIcon(QIcon(iconPath + "signature.png"));
    signAct->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_I));
    signAct->setToolTip(tr("Sign Message"));
    connect(signAct, SIGNAL(triggered()), this, SLOT(sign()));

    verifyAct = new QAction(tr("&Verify"), this);
    verifyAct->setIcon(QIcon(iconPath + "verify.png"));
    verifyAct->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_V));
    verifyAct->setToolTip(tr("Verify Message"));
    connect(verifyAct, SIGNAL(triggered()), this, SLOT(verify()));

    /* Key Menu
     */

    importKeyFromEditAct = new QAction(tr("&Editor"), this);
    importKeyFromEditAct->setIcon(QIcon(iconPath + "txt.png"));
    importKeyFromEditAct->setToolTip(tr("Import New Key From Editor"));
    connect(importKeyFromEditAct, SIGNAL(triggered()), this, SLOT(importKeyFromEdit()));

    openKeyManagementAct = new QAction(tr("&Key Management"), this);
    openKeyManagementAct->setIcon(QIcon(iconPath + "keymgmt.png"));
    openKeyManagementAct->setToolTip(tr("Open Keymanagement"));
    connect(openKeyManagementAct, SIGNAL(triggered()), this, SLOT(openKeyManagement()));

    /* About Menu
     */
    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setIcon(QIcon(iconPath + "help.png"));
    aboutAct->setToolTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    openTutorialAct = new QAction(tr("Online &Tutorial"), this);
    openTutorialAct->setToolTip(tr("Open Online Tutorial"));
    connect(openTutorialAct, SIGNAL(triggered()), this, SLOT(openTutorial()));

    openTranslateAct = new QAction(tr("Translate gpg4usb"), this);
    openTranslateAct->setToolTip(tr("Translate gpg4usb yourself"));
    connect(openTranslateAct, SIGNAL(triggered()), this, SLOT(openTranslate()));

    startWizardAct= new QAction(tr("Open &Wizard"), this);
    startWizardAct->setToolTip(tr("Open the wizard"));
    connect(startWizardAct, SIGNAL(triggered()), this, SLOT(startWizard()));

    /* Popup-Menu-Action for KeyList
     */
    appendSelectedKeysAct = new QAction(tr("Append Selected Key(s) To Text"), this);
    appendSelectedKeysAct->setToolTip(tr("Append The Selected Keys To Text in Editor"));
    connect(appendSelectedKeysAct, SIGNAL(triggered()), this, SLOT(appendSelectedKeys()));

    copyMailAddressToClipboardAct = new QAction(tr("Copy EMail-address"), this);
    copyMailAddressToClipboardAct->setToolTip(tr("Copy selected EMailaddress to clipboard"));
    connect(copyMailAddressToClipboardAct, SIGNAL(triggered()), this, SLOT(copyMailAddressToClipboard()));

    // TODO: find central place for shared actions, to avoid code-duplication with keymgmt.cpp
    showKeyDetailsAct = new QAction(tr("Show Keydetails"), this);
    showKeyDetailsAct->setToolTip(tr("Show Details for this Key"));
    connect(showKeyDetailsAct, SIGNAL(triggered()), this, SLOT(showKeyDetails()));

    /* Key-Shortcuts for Tab-Switchung-Action
     */
    switchTabUpAct = new QAction(this);
    switchTabUpAct->setShortcut(QKeySequence::NextChild);
    connect(switchTabUpAct, SIGNAL(triggered()), edit, SLOT(switchTabUp()));
    this->addAction(switchTabUpAct);

    switchTabDownAct = new QAction(this);
    switchTabDownAct->setShortcut(QKeySequence::PreviousChild);
    connect(switchTabDownAct, SIGNAL(triggered()), edit, SLOT(switchTabDown()));
    this->addAction(switchTabDownAct);
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newTabAct);
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(closeTabAct);
    fileMenu->addAction(quitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();
    editMenu->addAction(copyAct);
    editMenu->addAction(cutAct);
    editMenu->addAction(pasteAct);
    editMenu->addAction(selectallAct);
    editMenu->addAction(quoteAct);
    editMenu->addAction(cleanDoubleLinebreaksAct);
    editMenu->addSeparator();
    editMenu->addAction(openSettingsAct);

    cryptMenu = menuBar()->addMenu(tr("&Crypt"));
    cryptMenu->addAction(encryptAct);
    cryptMenu->addAction(decryptAct);
    cryptMenu->addSeparator();
    cryptMenu->addAction(signAct);
    cryptMenu->addAction(verifyAct);
    cryptMenu->addSeparator();
    cryptMenu->addAction(fileEncryptionAct);

    keyMenu = menuBar()->addMenu(tr("&Keys"));
    importKeyMenu = keyMenu->addMenu(tr("&Import Key From..."));
    importKeyMenu->setIcon(QIcon(iconPath + "key_import.png"));
    importKeyMenu->addAction(keyMgmt->importKeyFromFileAct);
    importKeyMenu->addAction(importKeyFromEditAct);
    importKeyMenu->addAction(keyMgmt->importKeyFromClipboardAct);
    importKeyMenu->addAction(keyMgmt->importKeyFromKeyServerAct);
    importKeyMenu->addAction(keyMgmt->importKeyFromKeyServerAct);
    keyMenu->addAction(openKeyManagementAct);

    viewMenu = menuBar()->addMenu(tr("&View"));

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(openTutorialAct);
    helpMenu->addAction(openTranslateAct);
    helpMenu->addAction(startWizardAct);
    helpMenu->addAction(aboutAct);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->setObjectName("fileToolBar");
    fileToolBar->addAction(newTabAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);
    fileToolBar->hide();
    viewMenu->addAction(fileToolBar->toggleViewAction());

    cryptToolBar = addToolBar(tr("Crypt"));
    cryptToolBar->setObjectName("cryptToolBar");
    cryptToolBar->addAction(encryptAct);
    cryptToolBar->addAction(decryptAct);
    cryptToolBar->addAction(signAct);
    cryptToolBar->addAction(verifyAct);
    //cryptToolBar->addAction(fileEncryptionAct);
    viewMenu->addAction(cryptToolBar->toggleViewAction());

    keyToolBar = addToolBar(tr("Key"));
    keyToolBar->setObjectName("keyToolBar");
    keyToolBar->addAction(openKeyManagementAct);
    viewMenu->addAction(keyToolBar->toggleViewAction());

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->setObjectName("editToolBar");
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
    editToolBar->addAction(selectallAct);
    viewMenu->addAction(editToolBar->toggleViewAction());

    specialEditToolBar = addToolBar(tr("Special edit"));
    specialEditToolBar->setObjectName("specialEditToolBar");
    specialEditToolBar->addAction(quoteAct);
    specialEditToolBar->addAction(cleanDoubleLinebreaksAct);
    viewMenu->addAction(specialEditToolBar->toggleViewAction());

    // Add dropdown menu for key import to keytoolbar
    QToolButton* importButton = new QToolButton();
    importButton->setMenu(importKeyMenu);
    importButton->setPopupMode(QToolButton::InstantPopup);
    importButton->setIcon(QIcon(iconPath + "key_import.png"));
    importButton->setToolTip("Import key");
    importButton->setText("Import key from..");
    importButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    keyToolBar->addWidget(importButton);

    // Add dropdown menu for file encryption/decryption to crypttoolbar
    QToolButton* fileEncButton = new QToolButton();
    QMenu* fileEncMenu = new QMenu();
    fileEncMenu->addAction(fileEncryptAct);
    fileEncMenu->addAction(fileDecryptAct);
    fileEncButton->setMenu(fileEncMenu);
    fileEncButton->setPopupMode(QToolButton::InstantPopup);
    fileEncButton->setIcon(QIcon(iconPath + "fileencrytion.png"));
    fileEncButton->setToolTip("Encrypt or decrypt File");
    fileEncButton->setText("File...");
    fileEncButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    cryptToolBar->addWidget(fileEncButton);

}

void MainWindow::createStatusBar()
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

void MainWindow::createDockWindows()
{
    /* KeyList-Dockwindow
     */
    keylistDock = new QDockWidget(tr("Encrypt for:"), this);
    keylistDock->setObjectName("EncryptDock");
    keylistDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, keylistDock);
    keylistDock->setWidget(mKeyList);
    viewMenu->addAction(keylistDock->toggleViewAction());

    /* Attachments-Dockwindow
      */
    if(settings.value("mime/parseMime").toBool()) {
        createAttachmentDock();
    }
}

void MainWindow::createAttachmentDock() {
    if (attachmentDockCreated) {
        return;
    }
    mAttachments = new Attachments(iconPath);
    attachmentDock = new QDockWidget(tr("Attached files:"), this);
    attachmentDock->setObjectName("AttachmentDock");
    attachmentDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, attachmentDock);
    attachmentDock->setWidget(mAttachments);
    // hide till attachment is decrypted
    viewMenu->addAction(attachmentDock->toggleViewAction());
    attachmentDock->hide();
    attachmentDockCreated = true;
}

void MainWindow::closeAttachmentDock() {
    if (!attachmentDockCreated) {
        return;
    }
    attachmentDock->close();
    attachmentDock->deleteLater();
    attachmentDockCreated = false;
}

void MainWindow::closeEvent(QCloseEvent *event)
{

    /*
     * ask to save changes, if there are
     * modified documents in any tab
     */
    if (edit->maybeSaveAnyTab()) {
        saveSettings();
        event->accept();
    } else {
        event->ignore();
    }

    // clear password from memory
    mCtx->clearPasswordCache();
}

void MainWindow::about()
{
    QPixmap *pixmap = new QPixmap(iconPath + "gpg4usb-logo.png");
    QString *title = new QString(tr("About ") + qApp->applicationName());
    QString *text = new QString("<center><h2>" + qApp->applicationName() + " "
                                + qApp->applicationVersion() + "</h2></center>"
                                + tr("<center>This application allows simple encryption <br/>"
                                     "and decryption of text messages or files.<br>"
                                     "It's licensed under the GPL v3<br><br>"
                                     "<b>Developer:</b><br>"
                                     "Bene, Heimer, Juergen, Nils, Ubbo<br><br>"
                                     "<b>Translation:</b><br>"
                                     "Alessandro (pt_br), Kirill (ru), Viriato (es), Serse (it) <br><br>"
                                     "If you have any questions or suggestions have a look<br/>"
                                     "at our <a href=\"http://gpg4usb.cpunk.de/contact.php\">"
                                     "contact page</a> or send a mail to our<br/> mailing list at"
                                     " <a href=\"mailto:gpg4usb@gzehn.de\">gpg4usb@gzehn.de</a>."));

    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(*title);
    QPushButton *closeButton = new QPushButton(tr("&Close"));
    connect(closeButton, SIGNAL(clicked()), dialog, SLOT(close()));

    QGridLayout *layout = new QGridLayout(dialog);
    QLabel *pixmapLabel = new QLabel();
    pixmapLabel->setPixmap(*pixmap);
    layout->addWidget(pixmapLabel, 0, 0, 1, -1, Qt::AlignCenter);
    QLabel *aboutLabel = new QLabel();
    aboutLabel->setText(*text);
    aboutLabel->setOpenExternalLinks(true);
    layout->addWidget(aboutLabel, 1, 0, 1, -1);
    layout->addItem(new QSpacerItem(20, 10, QSizePolicy::Minimum,
                                    QSizePolicy::Fixed), 2, 1, 1, 1);
    layout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding), 3, 0, 1, 1);
    layout->addWidget(closeButton, 3, 1, 1, 1);
    layout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding), 3, 2, 1, 1);

    dialog->exec();
}

void MainWindow::openTranslate() {
    QDesktopServices::openUrl(QUrl("http://gpg4usb.cpunk.de/docu_translate.html"));
}

void MainWindow::openTutorial() {
    //QDesktopServices::openUrl(QUrl("http://gpg4usb.cpunk.de/docu.html"));
    edit->newHelpTab("help", qApp->applicationDirPath() + "/help/docu.html");
}

void MainWindow::startWizard()
{
    Wizard *wizard = new Wizard(mCtx,this);
    wizard->show();
    wizard->setModal(true);
}

/*
  * if this is mime, split text and attachments...
  * message contains only text afterwards
  */
void MainWindow::parseMime(QByteArray *message)
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
        attachmentDock->show();
    }
}

void MainWindow::checkAttachmentFolder() {
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

void MainWindow::importKeyFromEdit()
{
    if (edit->tabCount()==0 || edit->curPage() == 0) {
        return;
    }

    mCtx->importKey(edit->curTextPage()->toPlainText().toAscii());
}

void MainWindow::openKeyManagement()
{
    keyMgmt->show();
    keyMgmt->raise();
    keyMgmt->activateWindow();
}

void MainWindow::encrypt()
{
    if (edit->tabCount()==0 || edit->curPage() == 0) {
        return;
    }

    QStringList *uidList = mKeyList->getChecked();

    QByteArray *tmp = new QByteArray();
    if (mCtx->encrypt(uidList, edit->curTextPage()->toPlainText().toUtf8(), tmp)) {
        QString *tmp2 = new QString(*tmp);
        edit->fillTextEditWithText(*tmp2);
    }
}

void MainWindow::sign()
{
    if (edit->tabCount()==0 || edit->curPage() == 0) {
        return;
    }

    QStringList *uidList = mKeyList->getPrivateChecked();

    QByteArray *tmp = new QByteArray();
    // TODO: toUtf8() here?
    if (mCtx->sign(uidList, edit->curTextPage()->toPlainText().toAscii(), tmp)) {
        QString *tmp2 = new QString(*tmp);
        edit->fillTextEditWithText(*tmp2);
    }
}

void MainWindow::decrypt()
{
    if (edit->tabCount()== 0 || edit->curPage() == 0) {
        return;
    }

    QByteArray *decrypted = new QByteArray();
    QByteArray text = edit->curTextPage()->toPlainText().toAscii(); // TODO: toUtf8() here?
    mCtx->preventNoDataErr(&text);

    // try decrypt, if fail do nothing, especially don't replace text
    if(!mCtx->decrypt(text, decrypted)) {
        return;
    }

    /*
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
    edit->fillTextEditWithText(QString::fromUtf8(*decrypted));
}

void MainWindow::verify()
{
    if (edit->tabCount()==0 || edit->curPage() == 0) {
        return;
    }

    // At first close verifynotification, if existing
    edit->curPage()->closeNoteByClass("verifyNotification");

    // create new verfiy notification
    VerifyNotification *vn = new VerifyNotification(this, mCtx, mKeyList, edit->curTextPage());

    // if signing information is found, show the notification, otherwise close it
    if (vn->refresh()) {
        edit->curPage()->showNotificationWidget(vn, "verifyNotification");
    } else {
        vn->close();
    }
}

/*
 * Append the selected (not checked!) Key(s) To Textedit
 */
void MainWindow::appendSelectedKeys()
{
    if (edit->tabCount()==0 || edit->curPage() == 0) {
        return;
    }

    QByteArray *keyArray = new QByteArray();
    mCtx->exportKeys(mKeyList->getSelected(), keyArray);
    edit->curTextPage()->append(*keyArray);
}

void MainWindow::copyMailAddressToClipboard()
{
    if (mKeyList->getSelected()->isEmpty()) {
        return;
    }

    gpgme_key_t key = mCtx->getKeyDetails(mKeyList->getSelected()->first());
    QClipboard *cb = QApplication::clipboard();
    QString mail = key->uids->email;
    cb->setText(mail);
}

void MainWindow::showKeyDetails()
{
    if (mKeyList->getSelected()->isEmpty()) {
        return;
    }

    gpgme_key_t key = mCtx->getKeyDetails(mKeyList->getSelected()->first());
    if (key) {
        new KeyDetailsDialog(mCtx, key, this);
    }
}

void MainWindow::fileEncryption()
{
        QStringList *keyList;
        keyList = mKeyList->getChecked();
        new FileEncryptionDialog(mCtx, iconPath, *keyList, this);
}

void MainWindow::fileEncrypt()
{
        QStringList *keyList;
        keyList = mKeyList->getChecked();
        new FileEncryptionDialog(mCtx, iconPath, *keyList, this, FileEncryptionDialog::Encrypt);
}

void MainWindow::fileDecrypt()
{
        QStringList *keyList;
        keyList = mKeyList->getChecked();
        new FileEncryptionDialog(mCtx, iconPath, *keyList, this, FileEncryptionDialog::Decrypt);
}

void MainWindow::openSettingsDialog()
{
    new SettingsDialog(this);
    // Iconsize
    QSize iconSize = settings.value("toolbar/iconsize", QSize(32, 32)).toSize();
    this->setIconSize(iconSize);

    // Iconstyle
    Qt::ToolButtonStyle buttonStyle = static_cast<Qt::ToolButtonStyle>(settings.value("toolbar/iconstyle", Qt::ToolButtonTextUnderIcon).toUInt());
    this->setToolButtonStyle(buttonStyle);

    if(settings.value("mime/parseMime").toBool()) {
        createAttachmentDock();
    } else if(attachmentDockCreated) {
        closeAttachmentDock();
    }

}

void MainWindow::cleanDoubleLinebreaks()
{
    if (edit->tabCount()==0 || edit->curPage() == 0) {
        return;
    }

    QString content = edit->curTextPage()->toPlainText();
    content.replace("\n\n", "\n");
    edit->fillTextEditWithText(content);
}
