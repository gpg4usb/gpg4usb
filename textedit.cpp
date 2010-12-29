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
    QString text=this->toPlainText();
    text.replace("\n","\n> ",Qt::CaseSensitive);
    text.insert(0,QString("> "));
    this->setPlainText(text);
}

bool TextEdit::isKey(QString key)
{
    qDebug() << key.contains("-----BEGIN PGP PUBLIC KEY BLOCK-----", Qt::CaseSensitive);
    return true;
}
