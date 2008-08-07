/*
 *      keylist.h
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
#include <QListWidget>
#include <QPushButton>
#include "context.h"

 class QMenu;

class KeyList : public QWidget
{
  Q_OBJECT

  public slots:
     void deleteKeys();
     void deleteKey();
     
  public:
    KeyList(QWidget *parent = 0);
    void setContext(GpgME::Context *ctx);
    QList<QString>* getChecked();
    QList<QString>* getSelected();
    void refresh();

  private:
    GpgME::Context *m_ctx;
    QListWidget *m_keyList;
    QList<QString> *m_idList;
    QPushButton *m_deleteButton;
    QAction *deleteKeyAct;
    void createActions();

    
  protected:
    void contextMenuEvent(QContextMenuEvent *event);
};
