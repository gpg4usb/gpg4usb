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
 * - possibility to clear attachment-view , e.g. with decryption or encrypting a new message
 * - save all: like in thunderbird, one folder, all files go there
 */

/*
 * - save, delete (clear) all
 * - each line save & clear button
 * - attached files to view-menu
 * - also an open button, whichs should save file to tmp-folder, and open with correct app (via QDesktopServices)
 */


#include "attachments.h"

Attachments::Attachments(QString iconpath, QWidget *parent)
        : QWidget(parent)
{

    this->iconPath = iconpath;

    table = new AttachmentTableModel(iconpath, this);

    tableView = new QTableView;
    tableView->setModel(table);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    // only one entry should be selected at time
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->setFocusPolicy(Qt::NoFocus);
    tableView->setAlternatingRowColors(true);
    tableView->verticalHeader()->hide();
    tableView->setShowGrid(false);
    tableView->setColumnWidth(0, 300);
    tableView->horizontalHeader()->setStretchLastSection(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(tableView);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);
    createActions();

}

void Attachments::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(saveFileAct);
    // enable open with only if allowed by user
    if(settings.value("mime/openAttachment").toBool())
        menu.addAction(openFileAct);

    menu.exec(event->globalPos());
}

void Attachments::createActions()
{
    saveFileAct = new QAction(tr("Save File"), this);
    saveFileAct->setToolTip(tr("Save this file"));
    saveFileAct->setIcon(QIcon(iconPath + "filesave.png"));
    connect(saveFileAct, SIGNAL(triggered()), this, SLOT(saveFile()));

    openFileAct = new QAction(tr("Open File"), this);
    openFileAct->setToolTip(tr("Open this file"));
    openFileAct->setIcon(QIcon(iconPath + "fileopen.png"));
    connect(openFileAct, SIGNAL(triggered()), this, SLOT(openFile()));

}

void Attachments::saveFile()
{

    QModelIndexList indexes = tableView->selectionModel()->selection().indexes();

    // only singe-selection possible now: TODO: foreach
    MimePart mp = table->getMimePart(indexes.at(0).row());
    QString filename = mp.header.getParam("Content-Type", "name");
    // TODO: find out why filename is quoted
    filename.chop(1);
    filename.remove(0, 1);
    // TODO: check if really base64
    saveByteArrayToFile(QByteArray::fromBase64(mp.body), filename);

}

void  Attachments::saveByteArrayToFile(QByteArray outBuffer, QString filename)
{

    //QString path="";
    QString path = filename;
    QString outfileName = QFileDialog::getSaveFileName(this, tr("Save File"), path);

    if (outfileName.isEmpty()) return;

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

/**
 * WIP: TODO:
 *   - create attachments dir if not existing
 *   - ask for cleanup of dir on exit
 *   - remove code-duplication with saveByteArrayToFile
 */
void Attachments::openFile() {

    // TODO: make attachmentdir constant or configurable
    QString attachmentDir = qApp->applicationDirPath() + "/attachments/";
    //QDir p = QDir(qApp->applicationDirPath() + "/attachments/");
    if(!QDir(attachmentDir).exists()) {
        QDir().mkpath(attachmentDir);
    }

    QModelIndexList indexes = tableView->selectionModel()->selection().indexes();
    MimePart mp = table->getMimePart(indexes.at(0).row());

//    qDebug() << "mime: " << mp.header.getValue("Content-Type");

    QString filename = mp.header.getParam("Content-Type", "name");
    // TODO: find out why filename is quoted
//    qDebug() << "file: " << filename;
    filename.chop(1);
    filename.remove(0, 1);
    filename.prepend(attachmentDir);

  //  qDebug() << "file: " << filename;
    QByteArray outBuffer = QByteArray::fromBase64(mp.body);


    QFile outfile(filename);
    if (!outfile.open(QFile::WriteOnly)) {
        QMessageBox::warning(this, tr("File"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(filename)
                             .arg(outfile.errorString()));
        return;
    }

    QDataStream out(&outfile);
    out.writeRawData(outBuffer.data(), outBuffer.length());

    QDesktopServices::openUrl(QUrl("file://"+filename, QUrl::TolerantMode));
}

void Attachments::addMimePart(MimePart *mp)
{
    table->add(*mp);
}

