#include "qmlpage.h"
#include <QtDeclarative/QDeclarativeView>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDebug>
#include <QDeclarativeContext>


QMLPage::QMLPage(KgpgCore::KgpgKey key, QWidget *parent) :
    QWidget(parent)
{

    // http://harmattan-dev.nokia.com/docs/library/html/qt4/qml-integration.html
    // http://qt-project.org/doc/qt-4.8/qtbinding.html
    QDeclarativeView *qmlView = new QDeclarativeView;
    qmlView->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    qmlView->setSource(QUrl("qrc:/qml/keydetails.qml"));

    QDeclarativeContext *context = qmlView->rootContext();
    context->setContextProperty("id", key.id());
    context->setContextProperty("email", key.email());
    context->setContextProperty("name", key.name());

    qDebug() << "qml:::::" << QUrl::fromLocalFile("keydetails.qml");


    //QWidget *widget = myExistingWidget();
    //QVBoxLayout *layout = new QVBoxLayout(this);
    QGridLayout *layout = new QGridLayout(this);
    layout->setOriginCorner(Qt::TopLeftCorner);
    layout->addWidget(qmlView);
}
