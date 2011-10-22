/*
 *      textpage.cpp
 *
 *      Copyright 2008 gpg4usb-team <gpg4usb@cpunk.de>
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

#include "editorpage.h"

EditorPage::EditorPage(const QString &filePath, QWidget *parent) : QWidget(parent),
                                                       fullFilePath(filePath)
{
    textPage   = new QPlainTextEdit();
    mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->addWidget(textPage);
    mainLayout->setContentsMargins(0,0,0,0);
    setLayout(mainLayout);
    setAttribute(Qt::WA_DeleteOnClose);
    textPage->setFocus();
}

const QString& EditorPage::getFilePath() const
{
    return fullFilePath;
}

QPlainTextEdit* EditorPage::getTextPage()
{
    return textPage;
}

void EditorPage::setFilePath(const QString &filePath)
{
    fullFilePath = filePath;
}

void EditorPage::showNotificationWidget(QWidget *widget, const char *className)
{
    widget->setProperty(className,true);
    mainLayout->addWidget(widget);
}

void EditorPage::closeNoteByClass(const char *className)
{
    QList<QWidget *> widgets = findChildren<QWidget *>();
    foreach(QWidget * widget, widgets)
    {
        if (widget->property(className) == true) {
                widget->close();
        }
    }
}
