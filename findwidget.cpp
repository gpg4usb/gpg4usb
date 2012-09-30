
#include "findwidget.h"

FindWidget::FindWidget(QWidget *parent, QTextEdit *edit) :
    QWidget(parent)
{
    mTextpage = edit;
    findEdit = new QLineEdit(this);
    QPushButton *closeButton= new QPushButton(this->style()->standardIcon(QStyle::SP_TitleBarCloseButton),"",this);
    QPushButton *nextButton= new QPushButton(QIcon(":button_next.png"), "");

    QHBoxLayout *notificationWidgetLayout = new QHBoxLayout(this);
    notificationWidgetLayout->setContentsMargins(10,0,0,0);
    notificationWidgetLayout->addWidget(new QLabel(tr("Find:")));
    notificationWidgetLayout->addWidget(findEdit,2);
    notificationWidgetLayout->addWidget(nextButton);
    notificationWidgetLayout->addWidget(closeButton);

    this->setLayout(notificationWidgetLayout);
    connect(findEdit,SIGNAL(textEdited(QString)),this,SLOT(slotFind()));
    connect(findEdit,SIGNAL(returnPressed()),this,SLOT(slotFindNext()));
    connect(nextButton,SIGNAL(clicked()),this,SLOT(slotFindNext()));
    connect(closeButton,SIGNAL(clicked()),this,SLOT(slotClose()));

    // The timer is necessary for setting the focus
    QTimer::singleShot(0, findEdit, SLOT(setFocus()));
}

void FindWidget::slotFindNext()
{
    cursor = mTextpage->document()->find(findEdit->text(), cursor, QTextDocument::FindCaseSensitively);
    // if end of document is reached, restart search from beginning
    if (cursor.position() == -1) {
        cursor.setPosition(0);
        cursor = mTextpage->document()->find(findEdit->text(), cursor, QTextDocument::FindCaseSensitively);
    }

    // if match is found set background of QLineEdit to white, otherwise to red
    QPalette bgPalette( findEdit->palette() );
    if ((cursor.position() == -1) && (!findEdit->text().isEmpty())) {
        bgPalette.setColor( QPalette::Base, "#ececba");
    } else {
        bgPalette.setColor( QPalette::Base, Qt::white);
    }
    findEdit->setPalette(bgPalette);

    mTextpage->setTextCursor(cursor);
}

void FindWidget::slotFind()
{
    if (cursor.anchor() == -1) {
        cursor = mTextpage->document()->find(findEdit->text(), cursor, QTextDocument::FindCaseSensitively);
    } else {
        cursor = mTextpage->document()->find(findEdit->text(), cursor.anchor(), QTextDocument::FindCaseSensitively);
    }

    // if match is found set background of QLineEdit to white, otherwise to red
    QPalette bgPalette( findEdit->palette() );
    if ((cursor.position() == -1) && (!findEdit->text().isEmpty())) {
        bgPalette.setColor( QPalette::Base, "#ececba");
    } else {
        bgPalette.setColor( QPalette::Base, Qt::white);
    }
    findEdit->setPalette(bgPalette);

    mTextpage->setTextCursor(cursor);
}

void FindWidget::keyPressEvent( QKeyEvent* e )
{
    switch ( e->key() )
    {
    case Qt::Key_Escape:
        this->slotClose();
        break;
    case Qt::Key_F3:
        this->slotFindNext();
        break;
    }
}

void FindWidget::slotClose() {
    if ( cursor.position() == -1) {
        cursor = mTextpage->textCursor();
        cursor.setPosition(0);
        mTextpage->setTextCursor(cursor);
    }
    mTextpage->setFocus();
    close();
}
