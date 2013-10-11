#include "qmlpage.h"
#include <QtDeclarative/QDeclarativeView>
#include <QHBoxLayout>
#include <QDebug>
#include <QDeclarativeContext>


QMLPage::QMLPage(KgpgCore::KgpgKey key, QWidget *parent) :
    QWidget(parent)
{

    // http://harmattan-dev.nokia.com/docs/library/html/qt4/qml-integration.html
    // http://qt-project.org/doc/qt-4.8/qtbinding.html
    // http://jryannel.wordpress.com/
    // http://stackoverflow.com/questions/5594769/normal-desktop-user-interface-controls-with-qml

    QDeclarativeView *qmlView = new QDeclarativeView;
    qmlView->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    qmlView->setSource(QUrl("qrc:/qml/keydetails.qml"));

    QDeclarativeContext *context = qmlView->rootContext();
    context->setContextProperty("id", key.id());
    context->setContextProperty("email", key.email());
    context->setContextProperty("name", key.name());

    qDebug() << "qml:::::" << QUrl::fromLocalFile("keydetails.qml");

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addWidget(qmlView);
}
