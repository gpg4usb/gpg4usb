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
#include "KGpgOrphanNode.h"

KGpgOrphanNode::KGpgOrphanNode(KGpgExpandableNode *parent, const KgpgKey &k)
	: KGpgNode(parent),
	m_key(new KgpgKey(k))
{
}

KGpgOrphanNode::~KGpgOrphanNode()
{
	delete m_key;
}

KgpgItemType
KGpgOrphanNode::getType() const
{
	return ITYPE_SECRET;
}

QString
KGpgOrphanNode::getName() const
{
	return m_key->name();
}

QString
KGpgOrphanNode::getEmail() const
{
	return m_key->email();
}

QString
KGpgOrphanNode::getSize() const
{
	return QString::number(m_key->size());
}

QDateTime
KGpgOrphanNode::getExpiration() const
{
	return m_key->expirationDate();
}

QDateTime
KGpgOrphanNode::getCreation() const
{
	return m_key->creationDate();
}

QString
KGpgOrphanNode::getId() const
{
	return m_key->fullId();
}

KgpgCore::KgpgKeyTrust
KGpgOrphanNode::getTrust() const
{
	return m_key->trust();
}

const QString &
KGpgOrphanNode::getFingerprint() const
{
	return m_key->fingerprint();
}
