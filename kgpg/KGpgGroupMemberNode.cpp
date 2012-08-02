/* Copyright 2008,2009 Rolf Eike Beer <kde@opensource.sf-tec.de>
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
#include "KGpgGroupMemberNode.h"

#include <QDateTime>

#include "KGpgGroupNode.h"
#include "KGpgKeyNode.h"

KGpgGroupMemberNode::KGpgGroupMemberNode(KGpgGroupNode *parent, const QString &k)
	: KGpgRefNode(parent, k)
{
}

KGpgGroupMemberNode::KGpgGroupMemberNode(KGpgGroupNode *parent, KGpgKeyNode *k)
	: KGpgRefNode(parent, k)
{
}

KGpgGroupMemberNode::~KGpgGroupMemberNode()
{
}

KgpgKeyTrust
KGpgGroupMemberNode::getTrust() const
{
	if (m_keynode != NULL)
		return m_keynode->getTrust();
	return KgpgCore::TRUST_NOKEY;
}

KgpgItemType
KGpgGroupMemberNode::getType() const
{
	if (m_keynode != NULL)
		return m_keynode->getType() | KgpgCore::ITYPE_GROUP;
	return KgpgCore::ITYPE_PUBLIC | KgpgCore::ITYPE_GROUP;
}

QString
KGpgGroupMemberNode::getSize() const
{
	if (m_keynode != NULL)
		return m_keynode->getSize();
	return QString();
}

QDateTime
KGpgGroupMemberNode::getExpiration() const
{
	if (m_keynode != NULL)
		return m_keynode->getExpiration();
	return QDateTime();
}

QDateTime
KGpgGroupMemberNode::getCreation() const
{
	if (m_keynode != NULL)
		return m_keynode->getCreation();
	return QDateTime();
}

unsigned int
KGpgGroupMemberNode::getSignKeySize() const
{
	if (m_keynode != NULL)
		return m_keynode->getSignKeySize();
	return 0;
}

unsigned int
KGpgGroupMemberNode::getEncryptionKeySize() const
{
	if (m_keynode != NULL)
		return m_keynode->getEncryptionKeySize();
	return 0;
}

KGpgGroupNode *
KGpgGroupMemberNode::getParentKeyNode() const
{
	return m_parent->toGroupNode();
}
