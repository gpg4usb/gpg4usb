/*
 *      fileencryptiondialog.cpp
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
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QRadioButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDebug>

#include "fileencryptiondialog.h"

FileEncryptionDialog::FileEncryptionDialog(GpgME::Context *ctx, QString iconPath, QWidget *parent)
        : QDialog(parent)

{

    mCtx = ctx;
    setWindowTitle(tr("Encrypt / Decrypt File"));
    resize(500, 200);
    setModal(true);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(executeAction()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QGroupBox *groupBox1 = new QGroupBox(tr("File"));
    QGroupBox *groupBox3 = new QGroupBox(tr("Action"));

    /* Setup input & Outputfileselection*/
    inputFileEdit = new QLineEdit();
    QPushButton *fb1 = new QPushButton("...");
    connect(fb1, SIGNAL(clicked()), this, SLOT(selectInputFile()));
    QLabel *fl1 = new QLabel(tr("Input"));
    fl1->setBuddy(inputFileEdit);

    outputFileEdit = new QLineEdit();
    QPushButton *fb2 = new QPushButton(tr("..."));
    connect(fb2, SIGNAL(clicked()), this, SLOT(selectOutputFile()));
    QLabel *fl2 = new QLabel(tr("Output"));
    fl2->setBuddy(outputFileEdit);

    QGridLayout *gLayout = new QGridLayout();
    gLayout->addWidget(fl1, 0, 0);
    gLayout->addWidget(inputFileEdit, 0, 1);
    gLayout->addWidget(fb1, 0, 2);
    gLayout->addWidget(fl2, 1, 0);
    gLayout->addWidget(outputFileEdit, 1, 1);
    gLayout->addWidget(fb2, 1, 2);

    /*Setup KeyList*/
    mKeyList = new KeyList(mCtx, iconPath);
    mKeyList->hide();
    mKeyList->setColumnWidth(2, 150);
    mKeyList->setColumnWidth(3, 150);

    /* Setup Action */
    radioEnc = new QRadioButton(tr("&Encrypt"));
    connect(radioEnc, SIGNAL(clicked()), this, SLOT(showKeyList()));
    radioDec = new QRadioButton(tr("&Decrypt"));
    connect(radioDec, SIGNAL(clicked()), this, SLOT(hideKeyList()));
    radioDec->setChecked(true);

    QHBoxLayout *hbox1 = new QHBoxLayout();
    hbox1->addWidget(radioDec);
    hbox1->addWidget(radioEnc);

    groupBox1->setLayout(gLayout);
    groupBox3->setLayout(hbox1);

    QVBoxLayout *vbox2 = new QVBoxLayout();
    vbox2->addWidget(groupBox3);
    vbox2->addWidget(groupBox1);
    vbox2->addWidget(mKeyList);
    vbox2->addWidget(buttonBox);
    vbox2->addStretch(0);
    setLayout(vbox2);
    exec();
}

void FileEncryptionDialog::selectInputFile()
{
    QString path = "";
    if (inputFileEdit->text().size() > 0) {
        path = QFileInfo(inputFileEdit->text()).absolutePath();
    }

//    QString infileName = QFileDialog::getOpenFileName(this, tr("Open File"), path, tr("Files") + tr("All Files (*)"));
    QString infileName = QFileDialog::getOpenFileName(this, tr("Open File"), path);
    inputFileEdit->setText(infileName);

    // try to find a matching output-filename, if not yet done
    if (infileName > 0 && outputFileEdit->text().size() == 0) {
        if (radioEnc->isChecked()) {
            outputFileEdit->setText(infileName + ".asc");
        } else {
            if (infileName.endsWith(".asc", Qt::CaseInsensitive)) {
                QString ofn = infileName;
                ofn.chop(4);
                outputFileEdit->setText(ofn);
            } else {
                outputFileEdit->setText(infileName + ".out");
            }
        }
    }
}

void FileEncryptionDialog::selectOutputFile()
{
    QString path = "";
    if (outputFileEdit->text().size() > 0) {
        path = QFileInfo(outputFileEdit->text()).absolutePath();
    }

    QString outfileName = QFileDialog::getSaveFileName(this, tr("Save File"), path);
    outputFileEdit->setText(outfileName);
}

void FileEncryptionDialog::executeAction()
{

    QFile infile;
    infile.setFileName(inputFileEdit->text());
    if (!infile.open(QIODevice::ReadOnly)) {
        qDebug() << tr("couldn't open file: ") + inputFileEdit->text();
    }

    QByteArray inBuffer = infile.readAll();

    QByteArray *outBuffer = new QByteArray();

    if (radioEnc->isChecked()) {
        if (! mCtx->encrypt(mKeyList->getChecked(), inBuffer, outBuffer)) return;
    }

    if (radioDec->isChecked()) {
        if (! mCtx->decrypt(inBuffer, outBuffer)) return;
    }

    QFile outfile(outputFileEdit->text());
    if (!outfile.open(QFile::WriteOnly)) {
        QMessageBox::warning(this, tr("File"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(outputFileEdit->text())
                             .arg(outfile.errorString()));
        return;
    }

    QDataStream out(&outfile);
    out.writeRawData(outBuffer->data(), outBuffer->length());

    accept();
    QMessageBox::information(0, "Done", "Output saved to " + outputFileEdit->text());
}

void FileEncryptionDialog::showKeyList()
{
    mKeyList->show();
}

void FileEncryptionDialog::hideKeyList()
{
    mKeyList->hide();
}
