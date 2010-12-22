#include "QDebug"
class QString;
#include "textedit.h"
TextEdit::TextEdit(QWidget *parent)
{
    setAcceptDrops(true);
}

void TextEdit::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("text/plain"))
        qDebug() << "enter drag action";

        //event->acceptProposedAction();
}

void TextEdit::dropEvent(QDropEvent* event)
{
    this->setPlainText(event->mimeData()->text());

    qDebug() << "enter drop action";
    qDebug() << event->mimeData()->text();
    event->acceptProposedAction();
}

void TextEdit::comment()
{
    QString test=this->toPlainText();
    qDebug() << "-------------";
    qDebug() << test;
    test.replace("\n","\n>",Qt::CaseSensitive);
    test.insert(0,QString(">"));
    qDebug() << "-------------";
    qDebug() << test;
}
