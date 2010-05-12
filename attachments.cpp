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

/* TODO:
 * - check content encoding (base64 / quoted-printable) and apply appropriate opperation (maybe already in mime.cpp)
 * - check memory usage, use less copy operations / more references
 * - try table-model-view for mimeparts
 * - possibility to clear attachment-view , e.g. with decryption or encrypting a new message
 */

#include "attachments.h"

Attachments::Attachments(QString iconpath, QWidget *parent)
        : QWidget(parent)
{

    this->iconPath = iconpath;

    mAttachmentTable = new QTableWidget(this);
    mAttachmentTable->setColumnCount(2);
    mAttachmentTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    mAttachmentTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mAttachmentTable->setFocusPolicy(Qt::NoFocus);
    mAttachmentTable->setAlternatingRowColors(true);
    mAttachmentTable->verticalHeader()->hide();
    mAttachmentTable->setShowGrid(false);
    mAttachmentTable->setColumnWidth(0, 300);

    attachmentBodys = new QList<QByteArray>();

    QStringList labels;
    labels << "filename" << "content-type";
    mAttachmentTable->setHorizontalHeaderLabels(labels);
    mAttachmentTable->horizontalHeader()->setStretchLastSection(true);

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

    foreach (int tmp, getSelectedPos()) {
        qDebug() << "int is: " << tmp;
        saveByteArrayToFile(attachmentBodys->at(tmp));
    }

}

void  Attachments::saveByteArrayToFile(QByteArray outBuffer)
{

    QString path="";
    QString outfileName = QFileDialog::getSaveFileName(this, tr("Save File"), path);

    QFile outfile(outfileName);
    if (!outfile.open(QFile::WriteOnly)) {
        QMessageBox::warning(this, tr("File"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(outfileName)
                             .arg(outfile.errorString()));
        return;
    }

    QDataStream out(&outfile);
    out.writeRawData(outBuffer.data(), outBuffer.length());
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

QList<int> Attachments::getSelectedPos () {

    QList<int> ret;

    for (int i = 0; i < mAttachmentTable->rowCount(); i++) {
        if (mAttachmentTable->item(i, 0)->isSelected() == 1) {
            ret << i;
        }
    }
    return ret;

}

void Attachments::addMimePart(MimePart *mp)
{

       QString icon = mp->getValue("Content-Type").replace("/", "-");
       icon = iconPath + "/mimetypes/" + icon + ".png";

       mAttachmentTable->setRowCount(mAttachmentTable->rowCount()+1);
       QTableWidgetItem  *tmp = new QTableWidgetItem(QIcon(icon) , mp->getParam("Content-Type", "name"));
       mAttachmentTable->setItem(mAttachmentTable->rowCount()-1, 0, tmp);

       QTableWidgetItem  *tmp2 = new QTableWidgetItem(mp->getValue("Content-Type"));
       mAttachmentTable->setItem(mAttachmentTable->rowCount()-1, 1, tmp2);

       //TODO: check, if content-encoding is base64 (get from header)
       attachmentBodys->append(QByteArray::fromBase64(mp->body));

}

