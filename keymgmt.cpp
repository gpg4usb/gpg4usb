/*
 *      keymgmt.cpp
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

#include <QtGui>

#include "keymgmt.h"


KeyMgmt::KeyMgmt(GpgME::Context *ctx, QString iconpath)
{
    mCtx = ctx;
    mIconPath = iconpath;
    
    /* the list of Keys available*/
    mKeyList = new KeyList(mCtx, mIconPath);
    mKeyList->setColumnWidth(2,250);
    mKeyList->setColumnWidth(3,250);
    setCentralWidget(mKeyList);

    createActions();
    createMenus();
    createToolBars();
    setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    setIconSize(QSize(32, 32));

    setWindowTitle(tr("Keymanagement"));
    mKeyList->addMenuAction(deleteSelectedKeysAct);
}

void KeyMgmt::createActions()
{
    closeAct = new QAction(tr("&Close Key Management"), this);
    closeAct->setShortcut(tr("Ctrl+Q"));
    closeAct->setIcon(QIcon(mIconPath + "exit.png"));
    closeAct->setStatusTip(tr("Close Key Management"));
    connect(closeAct, SIGNAL(triggered()), this, SLOT(close()));

    importKeyFromFileAct = new QAction(tr("Import Key From &File"), this);
    importKeyFromFileAct->setIcon(QIcon(mIconPath + "import_key_from_file.png"));
    importKeyFromFileAct->setStatusTip(tr("Import New Key From File"));
    connect(importKeyFromFileAct, SIGNAL(triggered()), this, SLOT(importKeyFromFile()));

    importKeyFromClipboardAct = new QAction(tr("Import Key From &Clipboard"), this);
    importKeyFromClipboardAct->setIcon(QIcon(mIconPath + "import_key_from_clipboard.png"));
    importKeyFromClipboardAct->setStatusTip(tr("Import New Key From Clipboard"));
    connect(importKeyFromClipboardAct, SIGNAL(triggered()), this, SLOT(importKeyFromClipboard()));

    exportKeyToClipboardAct = new QAction(tr("Export Key To &Clipboard"), this);
    exportKeyToClipboardAct->setIcon(QIcon(mIconPath + "export_key_to_clipbaord.png"));
    exportKeyToClipboardAct->setStatusTip(tr("Export Selected Key(s) To Clipboard"));
    connect(exportKeyToClipboardAct, SIGNAL(triggered()), this, SLOT(exportKeyToClipboard()));

    exportKeyToFileAct = new QAction(tr("Export Key To &File"), this);
    exportKeyToFileAct->setIcon(QIcon(mIconPath + "export_key_to_file.png"));
    exportKeyToFileAct->setStatusTip(tr("Export Selected Key(s) To File"));
    connect(exportKeyToFileAct, SIGNAL(triggered()), this, SLOT(exportKeyToFile()));

    deleteSelectedKeysAct = new QAction(tr("Delete Selected Key(s)"), this);
    deleteSelectedKeysAct->setStatusTip(tr("Delete the Selected keys"));
    connect(deleteSelectedKeysAct, SIGNAL(triggered()), this, SLOT(deleteSelectedKeys()));

    deleteCheckedKeysAct = new QAction(tr("Delete Checked Key(s)"), this);
    deleteCheckedKeysAct->setStatusTip(tr("Delete the Checked keys"));
    deleteCheckedKeysAct->setIcon(QIcon(mIconPath + "button_cancel.png"));
    connect(deleteCheckedKeysAct, SIGNAL(triggered()), this, SLOT(deleteCheckedKeys()));
        
    generateKeyDialogAct = new QAction(tr("Generate Key"), this);
    generateKeyDialogAct->setStatusTip(tr("Generate New Key"));
    generateKeyDialogAct->setIcon(QIcon(mIconPath + "key_generate.png"));
    connect(generateKeyDialogAct, SIGNAL(triggered()), this, SLOT(generateKeyDialog()));
}

void KeyMgmt::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(closeAct);
    
    keyMenu = menuBar()->addMenu(tr("&Key"));
    keyMenu->addAction(importKeyFromFileAct);
    keyMenu->addAction(importKeyFromClipboardAct);
    keyMenu->addSeparator();
    keyMenu->addAction(exportKeyToFileAct);
    keyMenu->addAction(exportKeyToClipboardAct);
    keyMenu->addSeparator();
    keyMenu->addAction(deleteCheckedKeysAct);
    keyMenu->addAction(generateKeyDialogAct);

}

void KeyMgmt::createToolBars()
{
    QToolBar *keyToolBar = addToolBar(tr("Crypt"));
    keyToolBar->addAction(importKeyFromFileAct);
    keyToolBar->addAction(importKeyFromClipboardAct);
    keyToolBar->addSeparator();
    keyToolBar->addAction(deleteCheckedKeysAct);
    keyToolBar->addSeparator();
    keyToolBar->addAction(exportKeyToFileAct);
    keyToolBar->addAction(exportKeyToClipboardAct);
}

void KeyMgmt::importKeyFromFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Key"), "", tr("Key Files") + " (*.asc *.txt);;All Files (*.*)");
    if (! fileName.isNull()) {
        QFile file;
        file.setFileName(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << tr("Couldn't Open File: ") + fileName;
        }
        QByteArray inBuffer = file.readAll();

        mCtx->importKey(inBuffer);
    }
}

void KeyMgmt::importKeyFromClipboard()
{
    QClipboard *cb = QApplication::clipboard();
    mCtx->importKey(cb->text(QClipboard::Clipboard).toAscii());
}

void KeyMgmt::deleteSelectedKeys()
{
    mCtx->deleteKeys(mKeyList->getSelected());
}

void KeyMgmt::deleteCheckedKeys()
{
    mCtx->deleteKeys(mKeyList->getChecked());
}

void KeyMgmt::exportKeyToFile()
{
    QByteArray *keyArray = new QByteArray();
	
    mCtx->exportKeys(mKeyList->getChecked(), keyArray);
    
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export Key To File"), "", tr("Key Files") + " (*.asc *.txt);;All Files (*.*)");
	QFile file(fileName);
	if (!file.open( QIODevice::WriteOnly |QIODevice::Text))
        return;
    QTextStream stream( &file );
	stream << *keyArray;
	file.close();
	delete keyArray;
}

void KeyMgmt::exportKeyToClipboard()
{
    QByteArray *keyArray = new QByteArray();
    QClipboard *cb = QApplication::clipboard();
	
    mCtx->exportKeys(mKeyList->getChecked(), keyArray);
    cb->setText(*keyArray);
	delete keyArray;
}

void KeyMgmt::generateKeyDialog() 
{
		QStringList errorMessages;
        genkeyDialog = new QDialog();
        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel);

        connect(buttonBox,SIGNAL(accepted()),this, SLOT(keyGenAccept()));
        connect(buttonBox,SIGNAL(rejected()), genkeyDialog, SLOT(reject()));

        genkeyDialog->setWindowTitle(tr("Generate Key"));
        genkeyDialog->setModal(true);

		nameLabel = new QLabel(tr("Name:*"));
		emailLabel = new QLabel(tr("E-Mailaddress::"));
		commentLabel = new QLabel(tr("Comment:"));
		keySizeLabel = new QLabel(tr("KeySize (in Bit):"));
		passwordLabel = new QLabel(tr("Password:"));
		repeatpwLabel = new QLabel(tr("Repeat Password:"));
		errorLabel = new QLabel(tr(""));

		nameEdit = new QLineEdit(genkeyDialog);
		emailEdit = new QLineEdit(genkeyDialog);
		commentEdit = new QLineEdit(genkeyDialog);
		keySizeSpinBox = new QSpinBox(genkeyDialog);
		keySizeSpinBox->setRange(512,8192);
		keySizeSpinBox->setValue(2048);
		keySizeSpinBox->setSingleStep(256);
		passwordEdit = new QLineEdit(genkeyDialog);
		repeatpwEdit = new QLineEdit(genkeyDialog);
		
		passwordEdit->setEchoMode(QLineEdit::Password);
		repeatpwEdit->setEchoMode(QLineEdit::Password);

 		QGridLayout *vbox1 = new QGridLayout;
 		vbox1->addWidget(nameLabel,0,0);
        vbox1->addWidget(nameEdit,0,1);
 		vbox1->addWidget(emailLabel,1,0);
        vbox1->addWidget(emailEdit,1,1);
 		vbox1->addWidget(commentLabel,2,0);
        vbox1->addWidget(commentEdit,2,1);
 		vbox1->addWidget(keySizeLabel,3,0);
        vbox1->addWidget(keySizeSpinBox,3,1);
   		vbox1->addWidget(passwordLabel,4,0);
   		vbox1->addWidget(passwordEdit,4,1);   		
   		vbox1->addWidget(repeatpwLabel,5,0);
   		vbox1->addWidget(repeatpwEdit,5,1);

		QWidget *nameList = new QWidget(genkeyDialog);
		nameList->setLayout(vbox1);

        QVBoxLayout *vbox2 = new QVBoxLayout();
		vbox2->addWidget(nameList);
        vbox2->addWidget(errorLabel);
        vbox2->addWidget(buttonBox);
		
        genkeyDialog->setLayout(vbox2);
		genkeyDialog->show();

        if(genkeyDialog->exec() == QDialog::Accepted ) {

		}
}


void KeyMgmt::keyGenAccept()
{
	QString errorString="";
    QString keyGenParams="";
    
    /**
     * check for errors in keygen dialog input
     */
	if ((nameEdit->text()).size() < 5 ) {
		errorString.append("  Name must contain at least five characters.  \n");
	}	
	if (passwordEdit->text() != repeatpwEdit->text()) {
		errorString.append("  Password and Repeat don't match.  ");
	}
	
	
	if (errorString.isEmpty()) { 

		/**
	 	* create the string for key generation
	 	*/
	  	keyGenParams ="<GnupgKeyParms format=\"internal\">\n"
    			"Key-Type: DSA\n"
	      		"Key-Length: "
	      		+keySizeSpinBox->cleanText()+"\n"
	      		"Subkey-Type: ELG-E\n"
	      		"Name-Real: "+ nameEdit->text()+"\n";
	    if (!(commentEdit->text().isEmpty())) {
	    	keyGenParams +="Name-Comment: "+commentEdit->text()+"\n";
	    }
	    if (!(emailEdit->text().isEmpty())) {
	    	keyGenParams +="Name-Email: "+emailEdit->text()+"\n";
	    }
	    keyGenParams += "Expire-Date: 0\n"
	    		"Passphrase: "+passwordEdit->text()+"\n"
	      		"</GnupgKeyParms>";

        mCtx->generateKey(&keyGenParams); 
 		genkeyDialog->accept();
	} else {

		/**
	 	* create error message
	 	*/
   		errorLabel->setAutoFillBackground(true);
   		QPalette error = errorLabel->palette();
   		error.setColor(QPalette::Background, "#ff8080");
   		errorLabel->setPalette(error);
		errorLabel->setText(errorString);

		genkeyDialog->show();
	}
}

/*int checkPassWordStrength(QString password)
{
	return 0;
}*/
