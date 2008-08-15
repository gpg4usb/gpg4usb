/*
 *      keylist.cpp
 *
 *      Copyright 2008 gpg4usb-team <gpg4usb@cpunk.de>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */
#include <QWidget>
#include <QVBoxLayout>
#include <QListWidgetItem>
#include <QLabel>
#include <QMessageBox>
#include <QtGui>
#include "keylist.h"

KeyList::KeyList(QWidget *parent)
        : QWidget(parent)
{
    m_keyList = new QListWidget();
    m_idList = new QList<QString>();
    m_deleteButton = new QPushButton(tr("Delete Selected Keys"));

    connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(deleteKeys()));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_keyList);
    layout->addWidget(m_deleteButton);
    setLayout(layout);
    createActions();

}

void KeyList::setContext(GpgME::Context *ctx)
{
    m_ctx = ctx;
    refresh();
}

void KeyList::setIconPath(QString path)
{
    this->iconPath = path;
}

void KeyList::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(deleteKeyAct);
    menu.exec(event->globalPos());
}

void KeyList::refresh()
{
    m_keyList->clear();
    m_idList->clear();

    GpgKeyList keys = m_ctx->listKeys();
    GpgKeyList::iterator it = keys.begin();
    while (it != keys.end()) {
        QListWidgetItem *tmp = new QListWidgetItem(
            it->name + " <" + it->email + ">"
        );
        tmp->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        tmp->setCheckState(Qt::Unchecked);
        if (it->privkey == 1)  tmp->setIcon(QIcon(iconPath + "kgpg_key2.png"));
        m_keyList->addItem(tmp);

        *m_idList << QString(it->id);
        it++;
    }
}


QList<QString> *KeyList::getChecked()
{
    QList<QString> *ret = new QList<QString>();
    for (int i = 0; i < m_keyList->count(); i++) {
        if (m_keyList->item(i)->checkState() == Qt::Checked) {
            *ret << m_idList->at(i);
        }
    }
    return ret;
}

QList<QString> *KeyList::getSelected()
{
    QList<QString> *ret = new QList<QString>();
    for (int i = 0; i < m_keyList->count(); i++) {
        if (m_keyList->item(i)->isSelected() == 1) {
            *ret << m_idList->at(i);
        }
    }
    return ret;
}

void KeyList::deleteKeys()
{

    m_ctx->deleteKeys(getChecked());
    refresh();

}

void KeyList::deleteKey()
{

    m_ctx->deleteKeys(getSelected());
    refresh();

}

void KeyList::createActions()
{
    deleteKeyAct = new QAction(tr("Delete Key"), this);
    deleteKeyAct->setStatusTip(tr("Delete the selected keys"));
    connect(deleteKeyAct, SIGNAL(triggered()), this, SLOT(deleteKey()));
}
