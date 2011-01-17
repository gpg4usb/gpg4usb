/*
 *      textedit.cpp
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

#include "QDebug"
#include "QUrl"
#include "textedit.h"
#include "quitdialog.h"
class QFileDialog;
class QMessageBox;

TextEdit::TextEdit(QString iconPath)
{
    mIconPath = iconPath;
    countPage         = 0;
    tabWidget = new QTabWidget(this);
    tabWidget->setMovable(true);
    tabWidget->setTabsClosable(true);
    tabWidget->setDocumentMode(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(tabWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    setLayout(layout);

    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(removeTab(int)));
    newTab();
    setAcceptDrops(false);
}

void TextEdit::newTab()
{
    QString header = "new " +
                     QString::number(++countPage);

    EditorPage *page = new EditorPage();
    tabWidget->addTab(page, header);
    tabWidget->setCurrentIndex(tabWidget->count() - 1);
    page->getTextPage()->setFocus();
    connect(page->getTextPage(), SIGNAL(modificationChanged(bool)), this, SLOT(showModified()));
//    setCursorPosition();
 }


void TextEdit::open()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open file"),
                                                          QDir::currentPath());
    foreach (QString fileName,fileNames){
        if (!fileName.isEmpty())
        {
            QFile file(fileName);

            if (file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                EditorPage *page = new EditorPage(fileName);

                QTextStream in(&file);
                QApplication::setOverrideCursor(Qt::WaitCursor);
                page->getTextPage()->setPlainText(in.readAll());
                page->setFilePath(fileName);
                QTextDocument *document = page->getTextPage()->document();
                document->setModified(false);

                tabWidget->addTab(page, strippedName(fileName));
                tabWidget->setCurrentIndex(tabWidget->count() - 1);
                QApplication::restoreOverrideCursor();
                page->getTextPage()->setFocus();
                connect(page->getTextPage(), SIGNAL(modificationChanged(bool)), this, SLOT(showModified()));
                //       setCursorPosition();
                //enableAction(true)
            }
            else
            {
                QMessageBox::warning(this, tr("Application"),
                                     tr("Cannot read file %1:\n%2.")
                                     .arg(fileName)
                                     .arg(file.errorString()));
            }
        }
    }
}


void TextEdit::save()
{
    QString fileName = curPage()->getFilePath();
	qDebug() <<fileName;

    if (fileName.isEmpty())
    {
        saveAs();
    }
    else
    {
        saveFile(fileName);
    }
}


bool TextEdit::saveFile(const QString &fileName)
{
    if (fileName.isEmpty())
    {
        return false;
    }


    QFile file(fileName);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream outputStream(&file);
        EditorPage *page = curPage();

        outputStream << page->getTextPage()->toPlainText();

        QTextDocument *document = page->getTextPage()->document();
        document->setModified(false);

        int curIndex = tabWidget->currentIndex();
        tabWidget->setTabText(curIndex, strippedName(fileName));
		page->setFilePath(fileName);
  //      statusBar()->showMessage(tr("File saved"), 2000);

        return true;
    }
    else
    {
        QMessageBox::warning(this, tr("File"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));

        return false;
    }
}


bool TextEdit::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save file"),
                                                    QDir::currentPath());

    return saveFile(fileName);
}


bool TextEdit::closeFile()
{
    if (tabWidget->count() != 0) {

/*
        // maybesave is also called in removeTab, so not necesarry here
        if (maybeSaveCurrentTab()) {

            // get current index and set it then back?
            int tabIndex = tabWidget->currentIndex();
            tabWidget->setCurrentIndex(tabIndex);

            // removetab is going to call close
            curPage()->close();
            tabWidget->removeTab(tabIndex);

            if (tabWidget->count() == 0) {
 //               enableAction(false);
            }
            return true;
        }
        return false;
        */

        int tabIndex = tabWidget->currentIndex();
        tabWidget->removeTab(tabIndex);

    }
    return false;
}

/**
 * close current tab
 */
void TextEdit::closeTab()
{
    removeTab(tabWidget->currentIndex());
    if (tabWidget->count() != 0) {
        curPage()->getTextPage()->setFocus();
    }
}

void TextEdit::removeTab(int index)
{
    if (tabWidget->count() == 0) {
        return;
    }

    int lastIndex = tabWidget->currentIndex();
    tabWidget->setCurrentIndex(index);

    if (maybeSaveCurrentTab()) {
        //curPage()->close();
        tabWidget->removeTab(index);

        if(index >= lastIndex) {
            tabWidget->setCurrentIndex(lastIndex);
        } else {
            tabWidget->setCurrentIndex(lastIndex-1);
        }
    }

    if (tabWidget->count() == 0) {
      //  enableAction(false);
    }
}

void TextEdit::cut()
{
    curTextPage()->cut();
}


void TextEdit::copy()
{
    curTextPage()->copy();
}


void TextEdit::paste()
{
    curTextPage()->paste();
}


void TextEdit::undo()
{
    curTextPage()->undo();
}


void TextEdit::redo()
{
    curTextPage()->redo();
}

void TextEdit::selectAll()
{
    curTextPage()->selectAll();
}

/*void TextEditor::closeEvent(QCloseEvent *event)
{
    int  curIndex = tabWidget->count();
    bool answ     = true;


    while (curIndex >= 1 && answ == true)
    {
        answ = closeFile();

        curIndex--;
    }


    if (answ == true)
    {
        writeSettings();
        event->accept();
    }
    else
    {
        event->ignore();
    }
}
*/

 /**
  * Check if current may need to be saved.
  * Call this function before closing the currently active tab-
  *
  * If it returns false, the close event should be aborted.
  */
bool TextEdit::maybeSaveCurrentTab() {
    EditorPage *page = curPage();
    QTextDocument *document = page->getTextPage()->document();

    if (document->isModified())
    {
        QString filePath = page->getFilePath();
        QMessageBox::StandardButton result;
        result = QMessageBox::warning(this, tr("Unsaved document"),
                                   tr("The document has been modified:")+"\n"+filePath+"\n\n"+tr("Do you want to save your changes?"),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (result == QMessageBox::Save) {
            if (filePath == "") {
                return saveAs();
            } else {
                return saveFile(filePath);
            }
        } else if (result == QMessageBox::Discard) {
            return true;
        } else {
            return false;
        }
    }
    return true;
}

bool TextEdit::saveTab(int i) {
    EditorPage *page = qobject_cast<EditorPage *> (tabWidget->widget(i));
 
        QString filePath = page->getFilePath();
            if (filePath == "") {
                return saveAs();
            } else {
                return saveFile(filePath);
            }
 

}
/**
 *  Checks if there are unsaved documents in any tab,
 *  which may need to be saved. Call this function before
 *  closing the programme or all tabs.
 *
 *   If it returns false, the close event should be aborted.
 */
bool TextEdit::maybeSaveAnyTab()
{

    QHash<int, QString> unsavedDocs;  // this list could be used to implement gedit like "unsaved changed"-dialog

    for(int i=0; i < tabWidget->count(); i++) {
        EditorPage *ep = qobject_cast<EditorPage *> (tabWidget->widget(i));
        if(ep->getTextPage()->document()->isModified()) {
            QString docname = tabWidget->tabText(i);
            // remove * before name of modified doc
            docname.remove(0,2);
            unsavedDocs.insert(i, docname);
        }
    }

    /*
     * only 1 unsaved document -> set modified tab as current
     * and show normal unsaved doc dialog
     */
    if(unsavedDocs.size() == 1) {
        int modifiedTab = unsavedDocs.keys().at(0);
        tabWidget->setCurrentIndex(modifiedTab);

        return maybeSaveCurrentTab();

    /*
     * more than one unsaved documents
     */
    } else if(unsavedDocs.size() > 1) {

        QString docList;
        QHashIterator<int, QString> i (unsavedDocs);
        while (i.hasNext()) {
            i.next();
            qDebug() << "unsaved: " << i.key() << ": " << i.value();
            docList.append(i.value()).append("\n");
        }

        QuitDialog *dialog;
        dialog=new QuitDialog(this, unsavedDocs, mIconPath);


        int result = dialog->exec();
        // return true, if app can be closed
        if (result == QDialog::Rejected){
            if (dialog->isDiscarded()){
                return true;
            } else {
                return false;
            }
        } else {
            QList <int> tabIdsToSave = dialog->getTabIdsToSave();
            foreach (int tabId, tabIdsToSave){
                qDebug() << "handling for save" << tabId;

            }

            return true;
        }

        /*
     * no unsaved documents
     */
    } else {
        return true;
    }

    // code should never reach this statement
    return false;
}


QPlainTextEdit* TextEdit::curTextPage()
{
    EditorPage *curTextPage = qobject_cast<EditorPage *>(tabWidget->currentWidget());

    return curTextPage->getTextPage();
}


EditorPage* TextEdit::curPage()
{
    EditorPage *curPage = qobject_cast<EditorPage *>(tabWidget->currentWidget());

    return curPage;
}

void TextEdit::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/plain"))
        qDebug() << "enter textedit drag action";
        event->acceptProposedAction();
}

void TextEdit::dropEvent(QDropEvent* event)
{
    curTextPage()->setPlainText(event->mimeData()->text());

    qDebug() << "enter textedit drop action";
    qDebug() << event->mimeData()->text();
    foreach (QUrl tmp, event->mimeData()->urls())
    {
        qDebug() << "hallo" << tmp;
    }

    //event->acceptProposedAction();
}

void TextEdit::quote()
{
    QTextCursor cursor(curTextPage()->document());

    // beginEditBlock and endEditBlock() let operation look like single undo/redo operation
    cursor.beginEditBlock();
    cursor.setPosition(0);
    cursor.insertText("> ");
    while (!cursor.isNull() && !cursor.atEnd()) {
        cursor.movePosition(QTextCursor::EndOfLine);
        cursor.movePosition(QTextCursor::NextCharacter);
        if(!cursor.atEnd())
            cursor.insertText("> ");
    }
    cursor.endEditBlock();

}

void TextEdit::loadFile(const QString &fileName)
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
    curTextPage()->setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();
    curPage()->setFilePath(fileName);
    tabWidget->setTabText(tabWidget->currentIndex(), strippedName(fileName));

   // statusBar()->showMessage(tr("File loaded"), 2000);
}

QString TextEdit::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void TextEdit::print()
{
#ifndef QT_NO_PRINTER
    QTextDocument *document = curTextPage()->document();
    QPrinter printer;

    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (dlg->exec() != QDialog::Accepted)
        return;

    document->print(&printer);

    //statusBar()->showMessage(tr("Ready"), 2000);
#endif
}

/** put a * in front of every modified document tab
  */
void TextEdit::showModified() {
    int index=tabWidget->currentIndex();
    QString title= tabWidget->tabText(index);
    if(curTextPage()->document()->isModified())
        tabWidget->setTabText(index, title.prepend("* "));
    else
        tabWidget->setTabText(index, title.remove(0,2));
}

void TextEdit::switchTabUp() {
        qDebug() << "hallo";
    if (tabWidget->count() > 1)
    {
        if (tabWidget->count() == tabWidget->currentIndex()+1){
            tabWidget->setCurrentIndex(0);
        }
        else
        {
            tabWidget->setCurrentIndex(tabWidget->currentIndex()+1);
        }
    }
}

void TextEdit::switchTabDown() {
    qDebug() << "hallo";
    if (tabWidget->count() > 1)
    {
        if (tabWidget->currentIndex()==1) {
            tabWidget->setCurrentIndex(tabWidget->count()-1);
        }
        else
        {
            tabWidget->setCurrentIndex(tabWidget->currentIndex()-1);
        }
    }
    this->removeTab(0);;
}

int TextEdit::getUnsavedDocumentsNumber() {
    int number=0;
    for(int i=0; i < tabWidget->count(); i++) {
        EditorPage *ep = qobject_cast<EditorPage *> (tabWidget->widget(i));
        if(ep->getTextPage()->document()->isModified()) {
            number++;
         }
    }
    return number;
}

QHash<int, QString> TextEdit::unsavedDocuments() {
    QHash<int, QString> unsavedDocs;  // this list could be used to implement gedit like "unsaved changed"-dialog

    for(int i=0; i < tabWidget->count(); i++) {
        EditorPage *ep = qobject_cast<EditorPage *> (tabWidget->widget(i));
        if(ep->getTextPage()->document()->isModified()) {
            QString docname = tabWidget->tabText(i);
            // remove * before name of modified doc
            docname.remove(0,2);
            unsavedDocs.insert(i, docname);
        }
    }
    return unsavedDocs;
}
