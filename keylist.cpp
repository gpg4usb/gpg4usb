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
#include <QTableWidgetItem>
#include <QLabel>
#include <QMessageBox>
#include <QtGui>
#include "keylist.h"

KeyList::KeyList(QWidget *parent)
        : QWidget(parent)
{
    m_keyList = new QTableWidget(this);
    m_keyList->setColumnCount(5);
    m_keyList->verticalHeader()->hide();
    m_keyList->setShowGrid(false);
    m_keyList->setColumnWidth(0, 24);
    m_keyList->setColumnWidth(1, 20);
    m_keyList->sortByColumn(2,Qt::AscendingOrder);
    m_keyList->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_keyList->setColumnHidden(4, true);
    // tableitems not editable
    m_keyList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // no focus (rectangle around tableitems)
    // may be it should focus on whole row
    m_keyList->setFocusPolicy(Qt::NoFocus);

    m_deleteButton = new QPushButton(tr("Delete Checked Keys"));

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

    QStringList labels;
    labels << "" << "" << "Name" << "EMail" << "id";
    m_keyList->setHorizontalHeaderLabels(labels);

    GpgKeyList keys = m_ctx->listKeys();
    m_keyList->setRowCount(keys.size());
    QTableWidgetItem *tmp;
    int row=0;
    GpgKeyList::iterator it = keys.begin();
    while (it != keys.end()) {

        tmp = new QTableWidgetItem();
        tmp->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        tmp->setCheckState(Qt::Unchecked);
        m_keyList->setItem(row, 0, tmp);

        if(it->privkey) {
            tmp = new QTableWidgetItem(QIcon(iconPath + "kgpg_key2.png"),"");
            m_keyList->setItem(row, 1, tmp);
        }
        tmp = new QTableWidgetItem(it->name);
        tmp->setToolTip(it->name);
        m_keyList->setItem(row, 2, tmp);
        tmp = new QTableWidgetItem(it->email);
        tmp->setToolTip(it->email);
        m_keyList->setItem(row, 3, tmp);
        tmp = new QTableWidgetItem(it->id);
        m_keyList->setItem(row, 4, tmp);
        it++;
        ++row;
    }
    m_keyList->setSortingEnabled(true);
}


QList<QString> *KeyList::getChecked()
{
    QList<QString> *ret = new QList<QString>();
    for (int i = 0; i < m_keyList->rowCount(); i++) {
        if (m_keyList->item(i,0)->checkState() == Qt::Checked) {
            *ret << m_keyList->item(i,4)->text();
        }
    }
    return ret;
}

QList<QString> *KeyList::getSelected()
{
    QList<QString> *ret = new QList<QString>();

    for (int i = 0; i < m_keyList->rowCount(); i++) {
        if (m_keyList->item(i,0)->isSelected() == 1) {
            *ret << m_keyList->item(i,4)->text();
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
    deleteKeyAct->setStatusTip(tr("Delete the checked keys"));
    connect(deleteKeyAct, SIGNAL(triggered()), this, SLOT(deleteKey()));
}
