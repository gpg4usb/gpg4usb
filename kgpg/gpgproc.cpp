/*
 * Copyright (C) 2007,2008,2009,2010,2011,2012 Rolf Eike Beer <kde@opensource.sf-tec.de>
 */

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "gpgproc.h"

//#include "kgpgsettings.h"

//#include <KDebug>
#include <QDebug>
//#include <KProcess>
#include "kprocess.h"
//#include <KStandardDirs>
#include <QDir>
#include <QTextCodec>

class GnupgBinary {
public:
	GnupgBinary();

	const QString &binary() const;
	void setBinary(const QString &executable);
	const QStringList &standardArguments() const;
	unsigned int version() const;
	bool supportsDebugLevel() const;
    //static GnupgBinary* instance();

private:
	QString m_binary;
	QStringList m_standardArguments;
	unsigned int m_version;
	bool m_useDebugLevel;
};

GnupgBinary::GnupgBinary()
	: m_useDebugLevel(false)
{
}

const QString &GnupgBinary::binary() const
{
	return m_binary;
}

/**
 * @brief check if GnuPG returns an error for this arguments
 * @param executable the GnuPG executable to call
 * @param arguments the arguments to pass to executable
 *
 * The arguments will be used together with "--version", so they should not
 * be any commands.
 */
static bool checkGnupgArguments(const QString &executable, const QStringList &arguments)
{
	KProcess gpg;

	// We ignore the output anyway, just make sure it doesn't clutter the output of
	// the parent process. Simplify the handling by putting all trash in one can.
	gpg.setOutputChannelMode(KProcess::MergedChannels);

	QStringList allArguments = arguments;
	allArguments << QLatin1String("--version");
	gpg.setProgram(executable, allArguments);

	return (gpg.execute() == 0);
}

static QString getGpgProcessHome(const QString &binary)
{
	GPGProc process(0, binary);
	process << QLatin1String( "--version" );
	process.start();
	process.waitForFinished(-1);

	if (process.exitCode() == 255) {
		return QString();
	}

	QString line;
	while (process.readln(line) != -1) {
		if (line.startsWith(QLatin1String("Home: "))) {
			line.remove(0, 6);
			return line;
		}
	}

	return QString();
}


void GnupgBinary::setBinary(const QString &executable)
{
    qDebug() << "checking version of GnuPG executable" << executable;
	// must be set first as gpgVersionString() uses GPGProc to parse the output
	m_binary = executable;
	const QString verstr = GPGProc::gpgVersionString(executable);
	m_version = GPGProc::gpgVersion(verstr);
    qDebug() << "version is" << verstr << m_version;

	m_useDebugLevel = (m_version > 0x20000);

    const QString gpgConfigFile = "";//KGpgSettings::gpgConfigPath();

	m_standardArguments.clear();
	m_standardArguments << QLatin1String( "--no-secmem-warning" )
			<< QLatin1String( "--no-tty" )
            << QLatin1String("--no-greeting")
            << QLatin1String("--homedir") << GPGProc::getGpgHome("");

	if (!gpgConfigFile.isEmpty())
		m_standardArguments << QLatin1String("--options")
				<< gpgConfigFile;

	QStringList debugLevelArguments(QLatin1String("--debug-level"));
	debugLevelArguments << QLatin1String("none");
	if (checkGnupgArguments(executable, debugLevelArguments))
		m_standardArguments << debugLevelArguments;
}

const QStringList& GnupgBinary::standardArguments() const
{
	return m_standardArguments;
}

unsigned int GnupgBinary::version() const
{
	return m_version;
}

bool GnupgBinary::supportsDebugLevel() const
{
	return m_useDebugLevel;
}

Q_GLOBAL_STATIC(GnupgBinary, lastBinary)

GPGProc::GPGProc(QObject *parent, const QString &binary)
       : KLineBufferedProcess(parent)
{
	resetProcess(binary);
}

GPGProc::~GPGProc()
{
}

void
GPGProc::resetProcess(const QString &binary)
{
    GnupgBinary *bin = lastBinary();
	QString executable;

    qDebug() << "bin:" << binary;

    if (binary.isEmpty()) {
        //executable = KGpgSettings::gpgBinaryPath();
        QString appPath = qApp->applicationDirPath();
        QString gpgBin;
        #ifdef Q_WS_WIN
            gpgBin = appPath + "/bin/gpg.exe";
        #endif
        #ifdef Q_WS_MAC
            gpgBin = appPath + "/gpg-mac";
        #endif
        #ifdef Q_WS_X11
            gpgBin = appPath + "/bin/gpg";
        #endif
        executable =  gpgBin;
    } else
		executable = binary;

    if (bin->binary() != executable)
        bin->setBinary(executable);
    qDebug() << "ex: " << executable;
    setProgram(executable, bin->standardArguments());

	setOutputChannelMode(OnlyStdoutChannel);

	disconnect(SIGNAL(finished(int,QProcess::ExitStatus)));
	disconnect(SIGNAL(lineReadyStandardOutput()));
}

void GPGProc::start()
{
	// make sure there is exactly one connection from us to that signal
	connect(this, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished()), Qt::UniqueConnection);
	connect(this, SIGNAL(lineReadyStandardOutput()), this, SLOT(received()), Qt::UniqueConnection);
	KProcess::start();
}

void GPGProc::received()
{
	emit readReady();
}

void GPGProc::finished()
{
	emit processExited();
}

int GPGProc::readln(QString &line, const bool colons)
{
	QByteArray a;
	if (!readLineStandardOutput(&a))
		return -1;

	line = recode(a, colons);

	return line.length();
}

int GPGProc::readln(QStringList &l)
{
    QString s;

    int len = readln(s);
    if (len < 0)
        return len;

    l = s.split(QLatin1Char( ':' ));

    for (int i = 0; i < l.count(); ++i)
    {
        int j = 0;
        while ((j = l[i].indexOf(QLatin1String( "\\x3a" ), j, Qt::CaseInsensitive)) >= 0)
        {
            l[i].replace(j, 4, QLatin1Char( ':' ));
            j++;
        }
    }

    return l.count();
}

QString
GPGProc::recode(QByteArray a, const bool colons)
{
	int pos = 0;

	while ((pos = a.indexOf("\\x", pos)) >= 0) {
		if (pos > a.length() - 4)
			break;

		const QByteArray pattern(a.mid(pos, 4));
		const QByteArray hexnum(pattern.right(2));
		bool ok;
		char n[2];
		n[0] = hexnum.toUShort(&ok, 16);
		n[1] = '\0';	// to use n as a 0-terminated string
		if (!ok)
			continue;

		// QLatin1Char( ':' ) must be skipped, it is used as column delimiter
		// since it is pure ascii it can be replaced in QString.
		if (!colons && (n[0] == ':' )) {
			pos += 3;
			continue;
		}

		// it is likely to find the same byte sequence more than once
		int npos = pos;
		do {
			a.replace(npos, 4, n);
		} while ((npos = a.indexOf(pattern, npos)) >= 0);
	}

	return QTextCodec::codecForName("utf8")->toUnicode(a);
}

int GPGProc::gpgVersion(const QString &vstr)
{
	if (vstr.isEmpty())
		return -1;

	QStringList values(vstr.split(QLatin1Char( '.' )));
	if (values.count() < 3)
		return -2;

	return (0x10000 * values[0].toInt() + 0x100 * values[1].toInt() + values[2].toInt());
}

QString GPGProc::gpgVersionString(const QString &binary)
{
	GPGProc process(0, binary);
	process << QLatin1String( "--version" );
	process.start();
	process.waitForFinished(-1);

        if (process.exitCode() == 255) {
                qDebug() << "exit255";
		return QString();
        }

	QString line;
        if (process.readln(line) != -1) {
            qDebug() << line;
            return line.simplified().section(QLatin1Char( ' ' ), -1);
        } else {
            qDebug() << "no readln";
		return QString();
        }
}

QString GPGProc::getGpgStartupError(const QString &binary)
{
	GPGProc process(0, binary);
	process << QLatin1String( "--version" );
	process.start();
	process.waitForFinished(-1);

	QString result;

	while (process.hasLineStandardError()) {
		QByteArray tmp;
		process.readLineStandardError(&tmp);
		tmp += '\n';
		result += QString::fromUtf8(tmp);
	}

	return result;
}

QString GPGProc::getGpgHome(const QString &binary)
{
	// First try: if environment is set GnuPG will use that directory
	// We can use this directly without starting a new process
/*	QByteArray env(qgetenv("GNUPGHOME"));
	QString gpgHome;
	if (!env.isEmpty()) {
		gpgHome = QLatin1String( env );
	} else if (!binary.isEmpty()) {
		// Second try: start GnuPG and ask what it is
		gpgHome = getGpgProcessHome(binary);
	}

	// Third try: guess what it is.
	if (gpgHome.isEmpty()) {
#ifdef Q_OS_WIN32	//krazy:exclude=cpp
		gpgHome = qgetenv("APPDATA") + QLatin1String( "/gnupg/" );
		gpgHome.replace(QLatin1Char( '\\' ), QLatin1Char( '/' ));
#else
		gpgHome = QDir::homePath() + QLatin1String( "/.gnupg/" );
#endif
	}

	gpgHome.replace(QLatin1String( "//" ), QLatin1String( "/" ));

	if (!gpgHome.endsWith(QLatin1Char( '/' )))
		gpgHome.append(QLatin1Char( '/' ));

	if (gpgHome.startsWith(QLatin1Char( '~' )))
		gpgHome.replace(0, 1, QDir::homePath());

    //KStandardDirs::makeDir(gpgHome, 0700);*/
    QString appPath = qApp->applicationDirPath();
    QString gpgHome = appPath + "/keydb";

	return gpgHome;
}

//#include "gpgproc.moc"
