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
#include "KGpgUidNode.h"

#include "KGpgKeyNode.h"
#include "convert.h"

class KGpgUidNodePrivate {
public:
	KGpgUidNodePrivate(const unsigned int index, const QStringList &sl);

	const QString m_index;
	QString m_email;
	QString m_name;
	QString m_comment;
	KgpgCore::KgpgKeyTrust m_trust;
	bool m_valid;
};

KGpgUidNodePrivate::KGpgUidNodePrivate(const unsigned int index, const QStringList &sl)
	: m_index(QString::number(index))
{
	QString fullname(sl.at(9));
	if (fullname.contains(QLatin1Char( '<' )) ) {
		m_email = fullname;

		if (fullname.contains(QLatin1Char( ')' )) )
			m_email = m_email.section(QLatin1Char( ')' ), 1);

		m_email = m_email.section(QLatin1Char( '<' ), 1);
		m_email.truncate(m_email.length() - 1);

		if (m_email.contains(QLatin1Char( '<' ))) {
			// several email addresses in the same key
			m_email = m_email.replace(QLatin1Char( '>' ), QLatin1Char( ';' ));
			m_email.remove(QLatin1Char( '<' ));
		}
	}

	m_name = fullname.section(QLatin1String( " <" ), 0, 0);
	if (fullname.contains(QLatin1Char( '(' )) ) {
		m_name = m_name.section(QLatin1String( " (" ), 0, 0);
		m_comment = fullname.section(QLatin1Char( '(' ), 1, 1);
		m_comment = m_comment.section(QLatin1Char( ')' ), 0, 0);
	}

	m_trust = KgpgCore::Convert::toTrust(sl.at(1));
	m_valid = ((sl.count() <= 11) || !sl.at(11).contains(QLatin1Char( 'D' )));
}


KGpgUidNode::KGpgUidNode(KGpgKeyNode *parent, const unsigned int index, const QStringList &sl)
	: KGpgSignableNode(parent),
	d_ptr(new KGpgUidNodePrivate(index, sl))
{
}

KGpgUidNode::~KGpgUidNode()
{
	delete d_ptr;
}

QString
KGpgUidNode::getName() const
{
	const Q_D(KGpgUidNode);

	return d->m_name;
}

QString
KGpgUidNode::getEmail() const
{
	const Q_D(KGpgUidNode);

	return d->m_email;
}

QString
KGpgUidNode::getId() const
{
	const Q_D(KGpgUidNode);

	return d->m_index;
}

KGpgKeyNode *
KGpgUidNode::getKeyNode(void)
{
	return getParentKeyNode()->toKeyNode();
}

const KGpgKeyNode *
KGpgUidNode::getKeyNode(void) const
{
	return getParentKeyNode()->toKeyNode();
}

KGpgKeyNode *
KGpgUidNode::getParentKeyNode() const
{
	return m_parent->toKeyNode();
}

void
KGpgUidNode::readChildren()
{
}

KgpgCore::KgpgItemType
KGpgUidNode::getType() const
{
	return KgpgCore::ITYPE_UID;
}

KgpgCore::KgpgKeyTrust
KGpgUidNode::getTrust() const
{
	const Q_D(KGpgUidNode);

	return d->m_trust;
}

QString
KGpgUidNode::getComment() const
{
	const Q_D(KGpgUidNode);

	return d->m_comment;
}
