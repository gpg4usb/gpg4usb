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
#include "KGpgSubkeyNode.h"

//#include <KLocale>

#include "convert.h"
#include "KGpgKeyNode.h"

KGpgSubkeyNode::KGpgSubkeyNode(KGpgKeyNode *parent, const KgpgKeySub &k)
	: KGpgSignableNode(parent),
	m_skey(k)
{
	Q_ASSERT(parent != NULL);
}
KGpgSubkeyNode::~KGpgSubkeyNode()
{
}

void
KGpgSubkeyNode::readChildren()
{
}

KgpgCore::KgpgItemType
KGpgSubkeyNode::getType() const
{
	return ITYPE_SUB;
}

KgpgCore::KgpgKeyTrust
KGpgSubkeyNode::getTrust() const
{
	return m_skey.trust();
}

QDateTime
KGpgSubkeyNode::getExpiration() const
{
	return m_skey.expirationDate();
}

QDateTime
KGpgSubkeyNode::getCreation() const
{
	return m_skey.creationDate();
}

QString
KGpgSubkeyNode::getId() const
{
	return m_skey.id();
}

KGpgKeyNode *
KGpgSubkeyNode::getKeyNode(void)
{
	return getParentKeyNode()->toKeyNode();
}

const KGpgKeyNode *
KGpgSubkeyNode::getKeyNode(void) const
{
	return getParentKeyNode()->toKeyNode();
}

QString
KGpgSubkeyNode::getName() const
{
    //return i18n("%1 subkey", Convert::toString(m_skey.algorithm()));
    return "TODO";
}

QString
KGpgSubkeyNode::getSize() const
{
	return QString::number(m_skey.size());
}

KGpgKeyNode *
KGpgSubkeyNode::getParentKeyNode() const
{
	return m_parent->toKeyNode();
}
