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

    m_keyList->setAlternatingRowColors(true);

    QStringList labels;
    labels << "" << "" << "Name" << "EMail" << "id";
    m_keyList->setHorizontalHeaderLabels(labels);
    m_keyList->horizontalHeader()->setStretchLastSection(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_keyList);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(3);
    setLayout(layout);

    popupMenu = new QMenu(this);

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

void KeyList::refresh()
{
    // while filling the table, sort enabled causes errors
    m_keyList->setSortingEnabled(false);
    m_keyList->clearContents();

    GpgKeyList keys = m_ctx->listKeys();
    m_keyList->setRowCount(keys.size());

    int row=0;
    GpgKeyList::iterator it = keys.begin();
    while (it != keys.end()) {

        QTableWidgetItem *tmp0 = new QTableWidgetItem();
        tmp0->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        tmp0->setCheckState(Qt::Unchecked);
        m_keyList->setItem(row, 0, tmp0);

        if(it->privkey) {
            QTableWidgetItem *tmp1 = new QTableWidgetItem(QIcon(iconPath + "kgpg_key2.png"),"");
            m_keyList->setItem(row, 1, tmp1);
        }
        QTableWidgetItem *tmp2 = new QTableWidgetItem(it->name);
        tmp2->setToolTip(it->name);
        m_keyList->setItem(row, 2, tmp2);
        QTableWidgetItem *tmp3 = new QTableWidgetItem(it->email);
        tmp3->setToolTip(it->email);
        m_keyList->setItem(row, 3, tmp3);
        QTableWidgetItem *tmp4 = new QTableWidgetItem(it->id);
        m_keyList->setItem(row, 4, tmp4);
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

void KeyList::setColumnWidth(int row, int size)
{
    m_keyList->setColumnWidth(row, size);
}

void KeyList::contextMenuEvent(QContextMenuEvent *event)
{
    popupMenu->exec(event->globalPos());
}

void KeyList::addMenuAction(QAction *act)
{
    popupMenu->addAction(act);
}
