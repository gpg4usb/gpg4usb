#include "qmlpage.h"
#include <QtDeclarative/QDeclarativeView>
#include <QHBoxLayout>
#include <QDebug>
#include <QGraphicsObject>
#include <QDeclarativeProperty>


QMLPage::QMLPage(KgpgCore::KgpgKey key, QWidget *parent) :
    QWidget(parent)
{

    // http://harmattan-dev.nokia.com/docs/library/html/qt4/qml-integration.html
    // http://qt-project.org/doc/qt-4.8/qtbinding.html
    // http://jryannel.wordpress.com/
    // http://stackoverflow.com/questions/5594769/normal-desktop-user-interface-controls-with-qml


    QDeclarativeView *qmlView = new QDeclarativeView;
    qmlView->setResizeMode(QDeclarativeView::SizeRootObjectToView);


    context = qmlView->rootContext();
    context->setContextProperty("id", key.id());
    context->setContextProperty("email", key.email());
    context->setContextProperty("name", key.name());

    qmlView->setSource(QUrl("qrc:/qml/keydetails.qml"));

    /*
    or: http://xizhizhu.blogspot.de/2010/10/hybrid-application-using-qml-and-qt-c.html
    DeclarativePropertyMap map;
    map.insert("key1", "value1");
    map.insert("key2", "value2");
    context->setContextProperty("map", &map);

    */


    // http://stackoverflow.com/questions/5947455/connecting-qml-signals-to-qt
    obj = qmlView->rootObject();
    connect( obj, SIGNAL(clicked()), this, SLOT(qmlClicked()));

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addWidget(qmlView);

}

void QMLPage::qmlClicked() {

    // http://stackoverflow.com/questions/9062189/how-to-modify-a-qml-text-from-c
    qDebug() << "c++, click recieved from qml";
    qDebug() << "text1" << obj->property("tf1Text").toString();

    //QObject *text2 = obj->findChild<QObject*>("tf2");
    qDebug() << "text2 "<< obj->property("tf2Text");
}

