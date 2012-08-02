/* Copyright 2008,2009,2010,2012 Rolf Eike Beer <kde@opensource.sf-tec.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "KGpgUatNode.h"

#include "gpgproc.h"
#include "KGpgKeyNode.h"

//#include <KLocale>
//#include <KUrl>

#include <QUrl>
#include <QDir>
#include <QFile>
#include <QPixmap>
#include <QDateTime>

class KGpgUatNodePrivate {
public:
	KGpgUatNodePrivate(const KGpgKeyNode *parent, const unsigned int index, const QStringList &sl);

	const QString m_idx;
	const QPixmap m_pixmap;
	QDateTime m_creation;

private:
	static QPixmap loadImage(const KGpgKeyNode *parent, const QString &index);
};

KGpgUatNodePrivate::KGpgUatNodePrivate(const KGpgKeyNode *parent, const unsigned int index, const QStringList &sl)
	: m_idx(QString::number(index)),
	m_pixmap(loadImage(parent, m_idx))
{
	if (sl.count() < 6)
		return;
	m_creation = QDateTime::fromTime_t(sl.at(5).toUInt());
}

QPixmap
KGpgUatNodePrivate::loadImage(const KGpgKeyNode *parent, const QString &index)
{
	QPixmap pixmap;
#ifdef Q_OS_WIN32	//krazy:exclude=cpp
	const QString pgpgoutput = QLatin1String("cmd /C \"echo %I\"");
#else
	const QString pgpgoutput = QLatin1String("echo %I");
#endif

	GPGProc workProcess;
	workProcess <<
			QLatin1String("--no-greeting") <<
			QLatin1String("--status-fd=2") <<
			QLatin1String("--photo-viewer") << pgpgoutput <<
			QLatin1String("--edit-key") << parent->getFingerprint() <<
			QLatin1String( "uid" ) << index <<
			QLatin1String( "showphoto" ) <<
			QLatin1String( "quit" );

	workProcess.start();
	workProcess.waitForFinished();
	if (workProcess.exitCode() != 0)
		return pixmap;

	QString tmpfile;
	if (workProcess.readln(tmpfile) < 0)
		return pixmap;

    QUrl url(tmpfile);
	pixmap.load(url.path());
	QFile::remove(url.path());
	QDir dir;
    //dir.rmdir(url.directory());

	return pixmap;
}

KGpgUatNode::KGpgUatNode(KGpgKeyNode *parent, const unsigned int index, const QStringList &sl)
	: KGpgSignableNode(parent),
	d_ptr(new KGpgUatNodePrivate(parent, index, sl))
{
}

KGpgUatNode::~KGpgUatNode()
{
	delete d_ptr;
}

QString
KGpgUatNode::getName() const
{
    return tr("Photo id");
}

QString
KGpgUatNode::getSize() const
{
	const Q_D(KGpgUatNode);

	return QString::number(d->m_pixmap.width()) + QLatin1Char( 'x' ) + QString::number(d->m_pixmap.height());
}

QDateTime
KGpgUatNode::getCreation() const
{
	const Q_D(KGpgUatNode);

	return d->m_creation;
}

KGpgKeyNode *
KGpgUatNode::getParentKeyNode() const
{
	return m_parent->toKeyNode();
}

void
KGpgUatNode::readChildren()
{
}

KgpgCore::KgpgItemType
KGpgUatNode::getType() const
{
	return KgpgCore::ITYPE_UAT;
}

KgpgCore::KgpgKeyTrust
KGpgUatNode::getTrust() const
{
	return KgpgCore::TRUST_NOKEY;
}

const QPixmap &
KGpgUatNode::getPixmap() const
{
	const Q_D(KGpgUatNode);

	return d->m_pixmap;
}

QString
KGpgUatNode::getId() const
{
	const Q_D(KGpgUatNode);

	return d->m_idx;
}

KGpgKeyNode *
KGpgUatNode::getKeyNode(void)
{
	return getParentKeyNode()->toKeyNode();
}

const KGpgKeyNode *
KGpgUatNode::getKeyNode(void) const
{
	return getParentKeyNode()->toKeyNode();
}
