/*
 *      main.cpp
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

#include <QApplication>
#include "gpgwin.h"

int main(int argc, char *argv[])
{

#ifndef _WIN32
    // do not use GPG_AGENTS like seahorse, because they may save
    // a password an pc's not owned by user
    unsetenv("GPG_AGENT_INFO");
#endif

    QApplication app(argc, argv);

    //internationalize
    QString locale = QLocale::system().name();
    QTranslator translator;
    translator.load(QString("ts/gpg4usb_") + locale);
    app.installTranslator(&translator);

    GpgWin *window = new GpgWin();
    window->resize(800, 450);

    window->show();

    return app.exec();
}



