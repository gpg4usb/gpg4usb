/*
 *      keydetailswidget.cpp
 *
 *      Copyright 2013 gpg4usb-team <gpg4usb@cpunk.de>
 *
 *      This file is part of gpg4usb.
 *
 *      Gpg4usb is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *
 *      Gpg4usb is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with gpg4usb.  If not, see <http://www.gnu.org/licenses/>
 */

#include "keydetailswidget.h"
#include <QtDeclarative/QDeclarativeView>
#include <QHBoxLayout>
#include <QDebug>
#include <QGraphicsObject>
#include <QDeclarativeProperty>
#include <QDeclarativePropertyMap>
#include "kgpg/core/convert.h"

KeyDetailsWidget::KeyDetailsWidget(GpgME::GpgContext *ctx, KgpgCore::KgpgKey key, QWidget *parent) :
    QWidget(parent), mCtx(ctx)
{

    // http://harmattan-dev.nokia.com/docs/library/html/qt4/qml-integration.html
    // http://qt-project.org/doc/qt-4.8/qtbinding.html
    // http://jryannel.wordpress.com/
    // http://stackoverflow.com/questions/5594769/normal-desktop-user-interface-controls-with-qml

    QDeclarativeView *qmlView = new QDeclarativeView;
    qmlView->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    context = qmlView->rootContext();

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
    keymap.insert("revoked", !key.valid());
    context->setContextProperty("keymap", &keymap);

    qmlView->setSource(QUrl("qrc:/qml/keydetails.qml"));

    obj = qmlView->rootObject();
    //connect( obj, SIGNAL(clicked()), this, SLOT(qmlClicked()));
    connect( obj, SIGNAL(exportPublicKeyClicked()), this, SLOT(slotExportPublicKey()));
    connect( obj, SIGNAL(exportPrivateKeyClicked()), this, SLOT(slotExportPrivateKey()));

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addWidget(qmlView);

}

void KeyDetailsWidget::qmlClicked() {

    // http://stackoverflow.com/questions/9062189/how-to-modify-a-qml-text-from-c
    qDebug() << "c++, click recieved from qml";
    qDebug() << "text1" << obj->property("tf1Text").toString();

    //QObject *text2 = obj->findChild<QObject*>("tf2");
    qDebug() << "text2 "<< obj->property("tf2Text");
}

void KeyDetailsWidget::slotExportPublicKey() {
    QString id=obj->property("keyid").toString();
    mCtx->exportKeyToFile(QStringList(id));
}

void KeyDetailsWidget::slotExportPrivateKey() {
    QString id=obj->property("keyid").toString();
    mCtx->exportPrivateKey(id);
}
