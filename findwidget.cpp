
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

    QTimer::singleShot(0, findEdit, SLOT(setFocus()));
    haveHit = false;
}

void FindWidget::findNext()
{
    cursor = mTextpage->document()->find(findEdit->text(), cursor, QTextDocument::FindCaseSensitively);

    qDebug() << "cursor: " << cursor.position();
    qDebug() << "anchor: " << cursor.anchor();
}

void FindWidget::find()
{
    if (cursor.anchor() == -1) {
        cursor = mTextpage->document()->find(findEdit->text(), cursor, QTextDocument::FindCaseSensitively);
    } else {
        cursor = mTextpage->document()->find(findEdit->text(), cursor.anchor(), QTextDocument::FindCaseSensitively);
    }
    qDebug() << "cursor: " << cursor.position();
    qDebug() << "anchor: " << cursor.anchor();
}

void FindWidget::keyPressEvent( QKeyEvent* e )
{
    switch ( e->key() )
    {
        case Qt::Key_Escape:
            this->close();
    }
}
