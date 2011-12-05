#include "helppage.h"

HelpPage::HelpPage(const QString path, QWidget *parent) :
    QWidget(parent)
{

    QTextBrowser* browser   = new QTextBrowser();
    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->addWidget(browser);
    mainLayout->setContentsMargins(0,0,0,0);
    setLayout(mainLayout);
    //setAttribute(Qt::WA_DeleteOnClose);
    browser->setSource(path);
    browser->setFocus();

}
