
#include "findwidget.h"

FindWidget::FindWidget(QWidget *parent, QTextEdit *edit) :
    QWidget(parent)
{
    mTextpage = edit;
    findEdit = new QLineEdit(this);

    QHBoxLayout *notificationWidgetLayout = new QHBoxLayout(this);
    notificationWidgetLayout->setContentsMargins(10,0,0,0);
    notificationWidgetLayout->addWidget(new QLabel(tr("Find:")));
    notificationWidgetLayout->addWidget(findEdit,2);
    this->setLayout(notificationWidgetLayout);
    connect(findEdit,SIGNAL(textChanged(QString)),this,SLOT(find()));
    connect(findEdit,SIGNAL(returnPressed()),this,SLOT(findNext()));

    // The timer is necessary for setting the focus
    QTimer::singleShot(0, findEdit, SLOT(setFocus()));
    cursorFormat = cursor.charFormat();
    cursorFormat.setBackground(QBrush(Qt::yellow));
    cursor.setCharFormat(cursorFormat);

}

void FindWidget::findNext()
{
    // set background of previous selection back to white
    cursorFormat.setBackground(QBrush(Qt::white));
    cursor.setCharFormat(cursorFormat);

    cursor = mTextpage->document()->find(findEdit->text(), cursor, QTextDocument::FindCaseSensitively);

    // if end of document is reached, restart search from beginning
    if (cursor.position() == -1) {
        cursor.setPosition(0);
        cursor = mTextpage->document()->find(findEdit->text(), cursor, QTextDocument::FindCaseSensitively);
    }

    // set background of current selection to yellow
    cursorFormat.setBackground(QBrush(Qt::yellow));
    cursor.setCharFormat(cursorFormat);
}

void FindWidget::find()
{
    // set background of previous selection back to white
    cursorFormat.setBackground(QBrush(Qt::white));
    cursor.setCharFormat(cursorFormat);

    if (cursor.anchor() == -1) {
        cursor = mTextpage->document()->find(findEdit->text(), cursor, QTextDocument::FindCaseSensitively);
    } else {
        cursor = mTextpage->document()->find(findEdit->text(), cursor.anchor(), QTextDocument::FindCaseSensitively);
    }

    // set background of current selection to yellow
    cursorFormat.setBackground(QBrush(Qt::yellow));
    cursor.setCharFormat(cursorFormat);
}

void FindWidget::keyPressEvent( QKeyEvent* e )
{
    switch ( e->key() )
    {
        case Qt::Key_Escape:
            mTextpage->setFocus();
            this->close();
    }
}

//QPalette bgPalette( findEdit->palette() );
/*    if ( start == -1 ) {
    haveHit =true;
    // set background of lineedit to red
    bgPalette.setColor( QPalette::Base, "#ececba");
    QTextCharFormat format = cursor.charFormat();
    format.setBackground(QBrush(Qt::white));
    cursor.setCharFormat(format);
    qDebug() << "cursor: " <<cursor.position();
} else {
    haveHit =true;
    // set background of lineedit to white
    bgPalette.setColor( QPalette::Base, QColor(Qt::white) );
    cursor.setPosition(start);
    cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, findEdit->text().length());

    qDebug() << "cursor: " <<cursor.position();

    QTextCharFormat format = cursor.charFormat();
    format.setBackground(QBrush(Qt::yellow));
    cursor.setCharFormat(format);
}
findEdit->setPalette(bgPalette);
*/
