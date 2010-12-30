#include "QDebug"
#include "QUrl"
class QString;
#include "textedit.h"
TextEdit::TextEdit(QWidget *parent)
{
    setAcceptDrops(true);
}

void TextEdit::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/plain"))
        qDebug() << "enter textedit drag action";

        event->acceptProposedAction();
}

void TextEdit::dropEvent(QDropEvent* event)
{
    this->setPlainText(event->mimeData()->text());

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

    QTextCursor cursor(this->document());

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

bool TextEdit::isKey(QString key)
{
    qDebug() << key.contains("-----BEGIN PGP PUBLIC KEY BLOCK-----", Qt::CaseSensitive);
    return true;
}
