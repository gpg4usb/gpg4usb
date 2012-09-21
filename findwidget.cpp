
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
}

void FindWidget::findNext()
{
    cursor = mTextpage->document()->find(findEdit->text(), cursor, QTextDocument::FindCaseSensitively);
    // if end of document is reached, restart search from beginning
    if (cursor.position() == -1) {
        cursor.setPosition(0);
        cursor = mTextpage->document()->find(findEdit->text(), cursor, QTextDocument::FindCaseSensitively);
    }
    mTextpage->setTextCursor(cursor);
}

void FindWidget::find()
{
    if (cursor.anchor() == -1) {
        cursor = mTextpage->document()->find(findEdit->text(), cursor, QTextDocument::FindCaseSensitively);
    } else {
        cursor = mTextpage->document()->find(findEdit->text(), cursor.anchor(), QTextDocument::FindCaseSensitively);
    }
    mTextpage->setTextCursor(cursor);
}

void FindWidget::keyPressEvent( QKeyEvent* e )
{
    switch ( e->key() )
    {
        case Qt::Key_Escape:
            mTextpage->setFocus();
            this->close();
        case Qt::Key_F3:
            this->findNext();

    }
}
