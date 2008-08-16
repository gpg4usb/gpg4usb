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
#include <QVBoxLayout>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <iostream>

#include "attachments.h"

Attachments::Attachments(QWidget *parent)
        : QWidget(parent)
{
    m_attachmentList = new QListWidget();
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_attachmentList);
    setLayout(layout);
    createActions();

}

void Attachments::setIconPath(QString path)
{
    this->iconPath = path;
}

void Attachments::setContext(GpgME::Context *ctx)
{
    m_ctx = ctx;
}

void Attachments::setKeyList(KeyList *keylist)
{
    m_keyList = keylist;
}


void Attachments::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(addFileAct);
    menu.addAction(encryptAct);
    menu.addAction(decryptAct);
    menu.exec(event->globalPos());
}

void Attachments::createActions()
{
    addFileAct = new QAction(tr("Add File"), this);
    addFileAct->setStatusTip(tr("Add a file"));
    addFileAct->setIcon(QIcon(iconPath + "fileopen.png"));
    connect(addFileAct, SIGNAL(triggered()), this, SLOT(addFile()));

    encryptAct = new QAction(tr("Encrypt"), this);
    encryptAct->setStatusTip(tr("Encrypt marked File(s)"));
    encryptAct->setIcon(QIcon(iconPath + "encrypted.png"));
    connect(encryptAct, SIGNAL(triggered()), this, SLOT(encryptFile()));

    decryptAct = new QAction(tr("Decrypt"), this);
    decryptAct->setStatusTip(tr("Decrypt marked File(s)"));
    decryptAct->setIcon(QIcon(iconPath + "decrypted.png"));
    connect(decryptAct, SIGNAL(triggered()), this, SLOT(decryptFile()));
}

void Attachments::addFile()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFiles);

    QStringList fileNames;
    if (dialog.exec())
       fileNames = dialog.selectedFiles();

    //foreach(QString tmp, fileNames) qDebug() << tmp;
    m_attachmentList->addItems(fileNames);
}

void Attachments::encryptFile()
{
    qDebug() << "enc";

    foreach(QString filename, *(getSelected())) {

        QByteArray *outBuffer = new QByteArray();
        QList<QString> *uidList = m_keyList->getChecked();

        QFile file;
        file.setFileName(filename);

        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << tr("couldn't open file: ") + filename;
        }
        qDebug() << "filesize: " << file.size();
        QByteArray inBuffer = file.readAll();
        qDebug() << "buffsize: " << inBuffer.size();

        if (m_ctx->encrypt(uidList, inBuffer, outBuffer)) {
          //qDebug() << "inb: " << inBuffer.toHex();
          qDebug() << "outb: " << outBuffer->data();
        }
    }
}

void Attachments::decryptFile()
{
    qDebug() << "dec";
    foreach(QString inFilename, *(getSelected())){
      qDebug() << inFilename;

      QFile infile;
      infile.setFileName(inFilename);
      if (!infile.open(QIODevice::ReadOnly)) {
        qDebug() << tr("couldn't open file: ") + inFilename;
      }

      QByteArray inBuffer = infile.readAll();
      QByteArray *outBuffer = new QByteArray();
      m_ctx->decrypt(inBuffer, outBuffer);

      QString outFilename = QFileDialog::getSaveFileName(this);
      if (outFilename.isEmpty()) {
        qDebug() << "need Filename";
        return;
      }

      QFile outfile(outFilename);
      if (!outfile.open(QFile::WriteOnly)) {
        QMessageBox::warning(this, tr("File"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(outFilename)
                             .arg(outfile.errorString()));
        return;
      }

      QDataStream out(&outfile);
      out << outBuffer;

      //qDebug() << "outb: " << outBuffer->toHex();

    }

}

QStringList *Attachments::getSelected()
{
    QStringList *ret = new QStringList();
    for (int i = 0; i < m_attachmentList->count(); i++) {
        if (m_attachmentList->item(i)->isSelected() == 1) {
            *ret << m_attachmentList->item(i)->text();
        }
    }
    return ret;
}
