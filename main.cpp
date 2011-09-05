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

    QApplication app(argc, argv);

    // get application path
    QString appPath = qApp->applicationDirPath();

    app.setApplicationVersion("0.3.0");
    app.setApplicationName("gpg4usb");

    // set environment variables
    // TODO:
    //   - unsetenv on windows?
    //   - wputenv or wputenv_s on windows? http://msdn.microsoft.com/en-us/library/d6dtz42k(v=vs.80).aspx
    #ifndef _WIN32
        // do not use GPG_AGENTS like seahorse, because they may save
        // a password an pc's not owned by user
        unsetenv("GPG_AGENT_INFO");
    #endif

    // take care of gpg not creating directorys on harddisk
    putenv(QString("GNUPGHOME=" + appPath + "/keydb").toAscii().data());

    // QSettings uses org-name for automatically setting path...
    app.setOrganizationName("conf");

    // specify default path & format for QSettings
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, appPath);
    QSettings::setDefaultFormat(QSettings::IniFormat);

    QSettings settings;
    QString lang = settings.value("int/lang", QLocale::system().name()).toString();
    if (lang.isEmpty()) {
        lang = QLocale::system().name();
    }

    //internationalize
    QTranslator translator;
    translator.load("ts/gpg4usb_" +  lang,
                    appPath);
    app.installTranslator(&translator);

    // make shortcuts system and language independent
    QTranslator translator2;
#ifdef _WIN32
    translator2.load("ts/qt_windows_" + lang, appPath);
#else
    translator2.load("ts/qt_linux_" + lang, appPath);
#endif
    app.installTranslator(&translator2);

    // css
    QFile file(qApp->applicationDirPath() + "/css/default.css");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    qApp->setStyleSheet(styleSheet);

    GpgWin *window = new GpgWin();
    window->show();

    return app.exec();
}



