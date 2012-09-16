/* Copyright 2008,2009,2010 Rolf Eike Beer <kde@opensource.sf-tec.de>
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
#include "KGpgSignNode.h"

#include "KGpgSignableNode.h"

//#include <KLocale>
#include <QLocale>

class KGpgSignNodePrivate {
public:
	KGpgSignNodePrivate(const QStringList &sl);

	QDateTime m_creation;
	QDateTime m_expiration;
	bool m_local;
	bool m_revocation;
};

KGpgSignNodePrivate::KGpgSignNodePrivate(const QStringList &sl)
{
	m_revocation = (sl.at(0) == QLatin1String("rev"));
	if (sl.count() < 6)
		return;
	m_creation = QDateTime::fromTime_t(sl.at(5).toUInt());
	if (sl.count() < 7)
		return;
	if (!sl.at(6).isEmpty())
		m_expiration = QDateTime::fromTime_t(sl.at(6).toUInt());
	if (sl.count() < 11)
		return;
	m_local = sl.at(10).endsWith(QLatin1Char( 'l' ));
}

KGpgSignNode::KGpgSignNode(KGpgSignableNode *parent, const QStringList &s)
	: KGpgRefNode(parent, s.at(4)),
	d_ptr(new KGpgSignNodePrivate(s))
{
}

KGpgSignNode::~KGpgSignNode()
{
	delete d_ptr;
}

QDateTime
KGpgSignNode::getExpiration() const
{
	const Q_D(KGpgSignNode);

	return d->m_expiration;
}

QDateTime
KGpgSignNode::getCreation() const
{
	const Q_D(KGpgSignNode);

	return d->m_creation;
}

QString
KGpgSignNode::getName() const
{
	const Q_D(KGpgSignNode);
	const QString name = KGpgRefNode::getName();

	if (!d->m_local)
		return name;

    //return tr("%1 [local signature]", &name.constData()->toAscii());
    return "fixme";
}

KgpgCore::KgpgItemType
KGpgSignNode::getType() const
{
	return KgpgCore::ITYPE_SIGN;
}
