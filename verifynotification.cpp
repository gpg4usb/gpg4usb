#include "verifynotification.h"

VerifyNotification::VerifyNotification(QWidget *parent) :
    QWidget(parent)
{
    QLabel *verifyLabel = new QLabel("Verified");

    QHBoxLayout *notificationWidgetLayout = new QHBoxLayout();
    notificationWidgetLayout->setContentsMargins(0,0,0,0);
    notificationWidgetLayout->addWidget(verifyLabel);

//    notificationWidget = new QWidget(this);

    this->setStyleSheet("background-color: #CBFDCB;");
    this->setLayout(notificationWidgetLayout);

    QAction *openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(QKeySequence::Open);
    openAct->setToolTip(tr("Open an existing file"));

    QMenu *verifyMenu = new QMenu();
    verifyMenu->addAction(openAct);

    QPushButton *verifyButton = new QPushButton("Details");
    verifyButton->setMenu(verifyMenu);
    notificationWidgetLayout->addStretch(1);
    notificationWidgetLayout->addWidget(verifyButton);


}
