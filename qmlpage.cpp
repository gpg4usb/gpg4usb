#include "qmlpage.h"
#include <QtDeclarative/QDeclarativeView>
#include <QHBoxLayout>
#include <QDebug>
#include <QGraphicsObject>
#include <QDeclarativeProperty>
#include <QDeclarativePropertyMap>
#include "kgpg/core/convert.h"

QMLPage::QMLPage(GpgME::GpgContext *ctx, KgpgCore::KgpgKey key, QWidget *parent) :
    QWidget(parent), mCtx(ctx)
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


    //or: http://xizhizhu.blogspot.de/2010/10/hybrid-application-using-qml-and-qt-c.html
    QDeclarativePropertyMap keymap;
    keymap.insert("id", key.id());
    keymap.insert("email", key.email());
    keymap.insert("name", key.name());
    keymap.insert("comment", key.comment());
    keymap.insert("size", key.size());
    keymap.insert("encryptionSize", key.encryptionSize());
    keymap.insert("algorithm",KgpgCore::Convert::toString(key.algorithm()));
    keymap.insert("encryptionAlgorithm", KgpgCore::Convert::toString(key.encryptionAlgorithm()));
    keymap.insert("creationDate", KgpgCore::Convert::toString(key.creationDate().date()));
    keymap.insert("expirationDate",KgpgCore::Convert::toString(key.expirationDate().date()));
    keymap.insert("fingerprint",key.fingerprintBeautified());
    keymap.insert("isSecret",ctx->isSecretKey(key.id()));
    keymap.insert("expired", (key.expirationDate().date() < QDate::currentDate() &! key.expirationDate().isNull()));
    context->setContextProperty("keymap", &keymap);

    qDebug() << "keydate vs current: " <<key.expirationDate().date() << " - "<< QDate::currentDate() << ":" << (key.expirationDate().date() < QDate::currentDate());


    // http://stackoverflow.com/questions/5947455/connecting-qml-signals-to-qt
    obj = qmlView->rootObject();
    connect( obj, SIGNAL(clicked()), this, SLOT(qmlClicked()));
    connect( obj, SIGNAL(exportPublicKeyClicked()), this, SLOT(slotExportPublicKey()));
    connect( obj, SIGNAL(exportPrivateKeyClicked()), this, SLOT(slotExportPrivateKey()));

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

void QMLPage::slotExportPublicKey() {
    QString id=obj->property("keyid").toString();
    mCtx->exportKeyToFile(QStringList(id));
}

void QMLPage::slotExportPrivateKey() {
    QString id=obj->property("keyid").toString();
    mCtx->exportPrivateKey(id);
}
