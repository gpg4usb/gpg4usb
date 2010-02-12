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

GpgWin::GpgWin()
{
    mCtx = new GpgME::Context();
    keyMgmt = NULL;  // initialized on first key-management-window open

    /* get path were app was started */
    QString appPath = qApp->applicationDirPath();
    iconPath = appPath + "/icons/";

    edit = new QPlainTextEdit();
    setCentralWidget(edit);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);


    /* the list of Keys available*/
    mKeyList = new KeyList(mCtx, iconPath);

    /* List of binary Attachments */
    /*mAttachments = new Attachments();
    mAttachments->setIconPath(iconPath);
    mAttachments->setContext(mCtx);
    mAttachments->setKeyList(mKeyList);*/

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createDockWindows();
    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    setIconSize(QSize(32, 32));
    setCurrentFile("");

    mKeyList->addMenuAction(appendSelectedKeysAct);
   
    // Restore window size & location
    // TODO: is this a good idea for a portable app? screen size & resolution may vary
    QSettings settings;
    //restoreGeometry(settings.value("window/geometry").toByteArray());
    QPoint pos = settings.value("window/pos", QPoint(100, 100)).toPoint();
    QSize size = settings.value("window/size", QSize(800, 450)).toSize();

    resize(size);
    move(pos);

    // state sets pos & size of dock-widgets
    restoreState(settings.value("window/windowState").toByteArray());

}

void GpgWin::createActions()
{
    /** Main Menu
      */
    openAct = new QAction(tr("&Open..."), this);
    openAct->setIcon(QIcon(iconPath + "fileopen.png"));
    openAct->setShortcut(tr("Ctrl+O"));
    openAct->setToolTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(tr("&Save"), this);
    saveAct->setIcon(QIcon(iconPath + "filesave.png"));
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setToolTip(tr("Save the current File"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As"), this);
    saveAsAct->setIcon(QIcon(iconPath + "filesaveas.png"));
    saveAsAct->setToolTip(tr("Save the current File as..."));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    printAct = new QAction(tr("&Print"), this);
    printAct->setIcon(QIcon(iconPath + "fileprint.png"));
    printAct->setShortcut(tr("Ctrl+P"));
    printAct->setToolTip(tr("Print Document"));
    connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

    quitAct = new QAction(tr("&Quit"), this);
    quitAct->setShortcut(tr("Ctrl+Q"));
    quitAct->setIcon(QIcon(iconPath + "exit.png"));
    quitAct->setToolTip(tr("Quit Program"));
    connect(quitAct, SIGNAL(triggered()), this, SLOT(close()));

    /** Edit Menu
     */
    pasteAct = new QAction(tr("&Paste"), this);
    pasteAct->setIcon(QIcon(iconPath + "button_paste.png"));
    pasteAct->setShortcut(tr("Ctrl+V"));
    pasteAct->setToolTip(tr("Paste Text From Clipboard"));
    connect(pasteAct, SIGNAL(triggered()), edit, SLOT(paste()));

    cutAct = new QAction(tr("Cu&t"), this);
    cutAct->setIcon(QIcon(iconPath + "button_cut.png"));
    cutAct->setShortcut(tr("Ctrl+X"));
    cutAct->setToolTip(tr("Cut the current selection's contents to the "
                          "clipboard"));
    connect(cutAct, SIGNAL(triggered()), edit, SLOT(cut()));

    copyAct = new QAction(tr("&Copy"), this);
    copyAct->setIcon(QIcon(iconPath + "button_copy.png"));
    copyAct->setShortcut(tr("Ctrl+C"));
    copyAct->setToolTip(tr("Copy the current selection's contents to the "
                           "clipboard"));
    connect(copyAct, SIGNAL(triggered()), edit, SLOT(copy()));

    selectallAct = new QAction(tr("Select &All"), this);
    selectallAct->setIcon(QIcon(iconPath + "edit.png"));
    selectallAct->setShortcut(tr("Ctrl+A"));
    selectallAct->setToolTip(tr("Select the whole text"));
    connect(selectallAct, SIGNAL(triggered()), edit, SLOT(selectAll()));

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

    /** View Menu
     */
	viewKeyToolbarAct = new QAction(tr("Keytoolbar"), this);
    viewKeyToolbarAct->setIcon(QIcon(iconPath + "checkbox_checked.png"));
	viewKeyToolbarAct->setToolTip(tr("Show/Hide Key-Toolbar"));
    connect(viewKeyToolbarAct, SIGNAL(triggered()), this, SLOT(viewKeyToolBar()));

	viewCryptToolbarAct = new QAction(tr("Crypttoolbar"), this);
    viewCryptToolbarAct->setIcon(QIcon(iconPath + "checkbox_checked.png"));
	viewCryptToolbarAct->setToolTip(tr("Show/Hide Crypt-Toolbar"));
    connect(viewCryptToolbarAct, SIGNAL(triggered()), this, SLOT(viewCryptToolBar()));

	viewEditToolbarAct = new QAction(tr("Edittoolbar"), this);
    viewEditToolbarAct->setIcon(QIcon(iconPath + "checkbox_checked.png"));
	viewEditToolbarAct->setToolTip(tr("Show/Hide Edit-Toolbar"));
    connect(viewEditToolbarAct, SIGNAL(triggered()), this, SLOT(viewEditToolBar()));

    /** About Menu
     */
    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setIcon(QIcon(iconPath + "help.png"));
    aboutAct->setToolTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    /** Popup-Menu-Action for KeyList
     */
    /*deleteSelectedKeysAct = new QAction(tr("Delete Selected Key(s)"), this);
    deleteSelectedKeysAct->setToolTip(tr("Delete The Selected Keys"));
    connect(deleteSelectedKeysAct, SIGNAL(triggered()), this, SLOT(deleteSelectedKeys()));*/

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

	viewMenu = menuBar()->addMenu(tr("View"));
	viewMenu->addAction(viewCryptToolbarAct);
	viewMenu->addAction(viewKeyToolbarAct);
	viewMenu->addAction(viewEditToolbarAct);
	
    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
}

void GpgWin::createToolBars()
{
    cryptToolBar = addToolBar(tr("Crypt"));
    cryptToolBar->addAction(encryptAct);
    cryptToolBar->addAction(decryptAct);
    cryptToolBar->addAction(fileEncryptionAct);

    keyToolBar = addToolBar(tr("Key"));
    keyToolBar->addAction(importKeyDialogAct);
    keyToolBar->addAction(openKeyManagementAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
    editToolBar->addAction(selectallAct);
}

void GpgWin::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void GpgWin::createDockWindows()
{
    QDockWidget *dock = new QDockWidget(tr("Encrypt for:"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    dock->setWidget(mKeyList);

    /*dock = new QDockWidget(tr("Attached files:"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea );
    addDockWidget(Qt::BottomDockWidgetArea, dock);
    dock->setWidget(mAttachments);*/

}

void GpgWin::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
    
     QSettings settings;

     
     //settings.setValue("geometry", saveGeometry());
     settings.setValue("window/windowState", saveState());
     settings.setValue("window/pos", pos());
     settings.setValue("window/size", size());
     //settings.setValue("windows/size", isFullscreen());
     
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
        shownName = tr("untitled.txt");
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
                            "Kirill (ru)<br><br>"
                            "If you have any questions and/or<br>"
                            "suggestions, contact us at<br>"
                            "gpg4usb at cpunk.de</a><br><br>"
                            "or feel free to meet us in our xmpp-channel:<br>"
                            "gpg4usb at conference.jabber.ccc.de</center>"));
}

void GpgWin::encrypt()
{
    QList<QString> *uidList = mKeyList->getChecked();
    
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
        edit->setPlainText(QString::fromUtf8(*tmp));
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

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Key"), "", tr("Key Files") + " (*.asc *.txt);;All Files (*)");
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
        keyMgmt->resize(800, 400);
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
 * Delete a selected (not checked!) Key(s) from keylist
 */
/*void GpgWin::deleteSelectedKeys()
{
    mCtx->deleteKeys(mKeyList->getSelected());
}*/

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

    new FileEncryptionDialog(mCtx, iconPath);

}

void GpgWin::viewKeyToolBar()
{
	if (keyToolBar->isHidden()) {
		keyToolBar->show();
		viewKeyToolbarAct->setIcon(QIcon(iconPath + "checkbox_checked.png"));
	} else {
		keyToolBar->hide();
		viewKeyToolbarAct->setIcon(QIcon(iconPath + "checkbox_unchecked.png"));
	}
}

void GpgWin::viewCryptToolBar()
{
	if (cryptToolBar->isHidden()) {
		cryptToolBar->show();
		viewCryptToolbarAct->setIcon(QIcon(iconPath + "checkbox_checked.png"));
	} else {
		cryptToolBar->hide();
		viewCryptToolbarAct->setIcon(QIcon(iconPath + "checkbox_unchecked.png"));
	}
}

void GpgWin::viewEditToolBar()
{
	if (editToolBar->isHidden()) {
		editToolBar->show();
		viewEditToolbarAct->setIcon(QIcon(iconPath + "checkbox_checked.png"));
	} else {
		editToolBar->hide();
		viewEditToolbarAct->setIcon(QIcon(iconPath + "checkbox_unchecked.png"));
	}
}
