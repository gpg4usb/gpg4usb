/*
 *      attachments.cpp
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

#include "attachments.h"

Attachments::Attachments(QString iconpath, QWidget *parent)
        : QWidget(parent)
{

    this->iconPath = iconpath;

    mAttachmentTable = new QTableWidget(this);
    mAttachmentTable->setColumnCount(2);

    QStringList labels;
    labels << "filename" << "content-type";
    mAttachmentTable->setHorizontalHeaderLabels(labels);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(mAttachmentTable);
    setLayout(layout);
    createActions();

}

void Attachments::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(saveFileAct);
    menu.exec(event->globalPos());
}

void Attachments::createActions()
{
    saveFileAct = new QAction(tr("Save File"), this);
    saveFileAct->setToolTip(tr("Save this file"));
    saveFileAct->setIcon(QIcon(iconPath + "filesave.png"));
    connect(saveFileAct, SIGNAL(triggered()), this, SLOT(saveFile()));

}

void Attachments::saveFile()
{

    qDebug() << "want to save file";

    foreach (QString tmp, *getSelected()) {
        qDebug() << tmp;
    }

}

QStringList *Attachments::getSelected()
{

    QStringList *ret = new QStringList();

    for (int i = 0; i < mAttachmentTable->rowCount(); i++) {
        if (mAttachmentTable->item(i, 0)->isSelected() == 1) {
            *ret << mAttachmentTable->item(i, 0)->text();
        }
    }
    return ret;

}

void Attachments::addMimePart(MimePart *mp)
{
        QString icoPath  = "/usr/lib/kde4/share/icons/oxygen/32x32/mimetypes/";
        QString icon = mp->getValue("Content-Type").replace("/", "-");
        icon = icoPath + icon + ".png";

        //QIcon *icon = new QIcon("/usr/lib/kde4/share/icons/oxygen/32x32/mimetypes/" + )
        mAttachmentTable->setRowCount(mAttachmentTable->rowCount()+1);
        QTableWidgetItem  *tmp = new QTableWidgetItem(QIcon(icon) , mp->getParam("Content-Type", "name"));
        mAttachmentTable->setItem(mAttachmentTable->rowCount()-1, 0, tmp);

       //  QTableWidgetItem  *tmp2 = new QTableWidgetItem(mp->getValue("Content-Type"));
       // mAttachmentTable->setItem(mAttachmentTable->rowCount()-1, 1, tmp2);

}

