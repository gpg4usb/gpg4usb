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

    app.setApplicationVersion("0.2.5");
    app.setApplicationName("gpg4usb");

    // QSettings uses org-name for automatically setting path...
    app.setOrganizationName("conf");
    // specify default path & format for QSettings
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, qApp->applicationDirPath());
    QSettings::setDefaultFormat(QSettings::IniFormat);

    QSettings settings;
    QString lang = settings.value("int/lang", QLocale::system().name()).toString();
    if (lang.isEmpty()) {
        lang = QLocale::system().name();
    }

    //internationalize
    QTranslator translator;
    translator.load("ts/gpg4usb_" +  lang,
                    qApp->applicationDirPath());
    app.installTranslator(&translator);

    // make shortcuts system and language independent
    QTranslator translator2;
#ifdef _WIN32
    translator2.load("ts/qt_windows_" + lang, qApp->applicationDirPath());
#else
    translator2.load("ts/qt_linux_" + lang, qApp->applicationDirPath());
#endif
    app.installTranslator(&translator2);

    GpgWin *window = new GpgWin();
    window->show();

    return app.exec();
}



