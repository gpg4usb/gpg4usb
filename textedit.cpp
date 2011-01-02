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

class QFileDialog;
class QMessageBox;

TextEdit::TextEdit()
{
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
void TextEdit::newTab()
{
    QString header = "new " +
                     QString::number(++countPage);

    EditorPage *page = new EditorPage();
    tabWidget->addTab(page, header);
    tabWidget->setCurrentIndex(tabWidget->count() - 1);

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
    if (tabWidget->count() != 0)
    {

        if (maybeSave())
        {
            int tabIndex = tabWidget->currentIndex();
            tabWidget->setCurrentIndex(tabIndex);

            curPage()->close();

            tabWidget->removeTab(tabIndex);


            if (tabWidget->count() == 0)
            {
 //               enableAction(false);
            }

            return true;
        }

        return false;
    }
    return false;
}


void TextEdit::closeTab()
{
    removeTab(tabWidget->currentIndex());
}

void TextEdit::removeTab(int index)
{
    if (tabWidget->count() != 0)
    {
        if (maybeSave())
        {
            tabWidget->setCurrentIndex(index);

            curPage()->close();

            tabWidget->removeTab(index);
        }
    }


    if (tabWidget->count() == 0)
    {
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

bool TextEdit::maybeSave()
{
    EditorPage *page = curPage();

    if (page == 0)
    {
        return false;
    }

    QTextDocument *document = page->getTextPage()->document();

    if (document->isModified())
    {
        QString filePath = page->getFilePath();
        QMessageBox::StandardButton result;

        result = QMessageBox::warning(this, tr("Application"),
                                   tr("The document has been modified:")+"\n"+filePath+"\n\n"+tr("Do you want to save your changes?"),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

        if (result == QMessageBox::Save)
        {
            if (filePath == "")
            {
                return saveAs();
            }
            else
            {
                return saveFile(filePath);
            }
        }
        else if (result == QMessageBox::Discard)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    return true;
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
