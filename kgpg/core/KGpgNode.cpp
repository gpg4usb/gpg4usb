/* Copyright 2008,2009,2012 Rolf Eike Beer <kde@opensource.sf-tec.de>
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
#include "KGpgNode.h"

#include "KGpgGroupMemberNode.h"
#include "KGpgGroupNode.h"
#include "KGpgOrphanNode.h"
#include "KGpgRootNode.h"
#include "KGpgSubkeyNode.h"
#include "KGpgUatNode.h"
#include "KGpgUidNode.h"
//#include "model/kgpgitemmodel.h"

//#include <KLocale>

KGpgNode::KGpgNode(KGpgExpandableNode *parent)
	: QObject(), m_parent(parent)
{
/*	if (parent == NULL)
		m_model = NULL;
	else
		m_model = parent->m_model;
*/
}

KGpgNode::~KGpgNode()
{
/*	Q_ASSERT(m_model);
	m_model->invalidateIndexes(this);
*/
	if (m_parent != NULL)
		m_parent->deleteChild(this);
}

QString
KGpgNode::getNameComment() const
{
	if (getComment().isEmpty())
		return getName();
	else
         return tr("Name of uid (comment) %1 (%2)").arg(getName()).arg(getComment());
        //return QString("Name of uid (comment)", "%1 (%2)", getName(), getComment());
}

KGpgExpandableNode *
KGpgNode::toExpandableNode()
{
	Q_ASSERT(((getType() & KgpgCore::ITYPE_GROUP) && !(getType() & KgpgCore::ITYPE_PAIR)) ||
			(getType() & (KgpgCore::ITYPE_PAIR | KgpgCore::ITYPE_SUB | KgpgCore::ITYPE_UID | KgpgCore::ITYPE_UAT)));

	return qobject_cast<KGpgExpandableNode *>(this);
}

const KGpgExpandableNode *
KGpgNode::toExpandableNode() const
{
	Q_ASSERT(((getType() & KgpgCore::ITYPE_GROUP) && !(getType() & KgpgCore::ITYPE_PAIR)) ||
	(getType() & (KgpgCore::ITYPE_PAIR | KgpgCore::ITYPE_SUB | KgpgCore::ITYPE_UID | KgpgCore::ITYPE_UAT)));

	return qobject_cast<const KGpgExpandableNode *>(this);
}

KGpgSignableNode *
KGpgNode::toSignableNode()
{
	Q_ASSERT(getType() & (KgpgCore::ITYPE_PAIR | KgpgCore::ITYPE_SUB | KgpgCore::ITYPE_UID | KgpgCore::ITYPE_UAT));
	
	return qobject_cast<KGpgSignableNode *>(this);
}

const KGpgSignableNode *
KGpgNode::toSignableNode() const
{
	Q_ASSERT(getType() & (KgpgCore::ITYPE_PAIR | KgpgCore::ITYPE_SUB | KgpgCore::ITYPE_UID | KgpgCore::ITYPE_UAT));
	
	return qobject_cast<const KGpgSignableNode *>(this);
}

KGpgKeyNode *
KGpgNode::toKeyNode()
{
	Q_ASSERT(getType() & KgpgCore::ITYPE_PAIR);
	Q_ASSERT(!(getType() & KgpgCore::ITYPE_GROUP));

	return qobject_cast<KGpgKeyNode *>(this);
}

const KGpgKeyNode *
KGpgNode::toKeyNode() const
{
	Q_ASSERT(getType() & KgpgCore::ITYPE_PAIR);
	Q_ASSERT(!(getType() & KgpgCore::ITYPE_GROUP));

	return qobject_cast<const KGpgKeyNode *>(this);
}

KGpgRootNode *
KGpgNode::toRootNode()
{
	Q_ASSERT((m_parent == NULL) && (getType() == 0));

	return static_cast<KGpgRootNode *>(this)->asRootNode();
}

const KGpgRootNode *
KGpgNode::toRootNode() const
{
	Q_ASSERT((m_parent == NULL) && (getType() == 0));

	return static_cast<const KGpgRootNode *>(this)->asRootNode();
}

KGpgUidNode *
KGpgNode::toUidNode()
{
	Q_ASSERT(getType() == KgpgCore::ITYPE_UID);

	return static_cast<KGpgUidNode *>(this);
}

const KGpgUidNode *
KGpgNode::toUidNode() const
{
	Q_ASSERT(getType() == KgpgCore::ITYPE_UID);

	return static_cast<const KGpgUidNode *>(this);
}

KGpgSubkeyNode *
KGpgNode::toSubkeyNode()
{
	Q_ASSERT(getType() == KgpgCore::ITYPE_SUB);

	return static_cast<KGpgSubkeyNode *>(this);
}

const KGpgSubkeyNode *
KGpgNode::toSubkeyNode() const
{
	Q_ASSERT(getType() == KgpgCore::ITYPE_SUB);

	return static_cast<const KGpgSubkeyNode *>(this);
}

KGpgUatNode *
KGpgNode::toUatNode()
{
	Q_ASSERT(getType() == KgpgCore::ITYPE_UAT);

	return static_cast<KGpgUatNode *>(this);
}

const KGpgUatNode *
KGpgNode::toUatNode() const
{
	Q_ASSERT(getType() == KgpgCore::ITYPE_UAT);

	return static_cast<const KGpgUatNode *>(this);
}

KGpgGroupNode *
KGpgNode::toGroupNode()
{
	Q_ASSERT(getType() == KgpgCore::ITYPE_GROUP);

	return static_cast<KGpgGroupNode *>(this);
}

const KGpgGroupNode *
KGpgNode::toGroupNode() const
{
	Q_ASSERT(getType() == KgpgCore::ITYPE_GROUP);

	return static_cast<const KGpgGroupNode *>(this);
}

KGpgRefNode *
KGpgNode::toRefNode()
{
	Q_ASSERT(((getType() & KgpgCore::ITYPE_GROUP) && (getType() & KgpgCore::ITYPE_PAIR)) || (getType() & KgpgCore::ITYPE_SIGN));

	return qobject_cast<KGpgRefNode *>(this);
}

const KGpgRefNode *
KGpgNode::toRefNode() const
{
	Q_ASSERT(((getType() & KgpgCore::ITYPE_GROUP) && (getType() & KgpgCore::ITYPE_PAIR)) || (getType() & KgpgCore::ITYPE_SIGN));

	return qobject_cast<const KGpgRefNode *>(this);
}

KGpgGroupMemberNode *
KGpgNode::toGroupMemberNode()
{
	Q_ASSERT((getType() & KgpgCore::ITYPE_GROUP) && (getType() & KgpgCore::ITYPE_PAIR));

	return static_cast<KGpgGroupMemberNode *>(this);
}

const KGpgGroupMemberNode *
KGpgNode::toGroupMemberNode() const
{
	Q_ASSERT((getType() & KgpgCore::ITYPE_GROUP) && (getType() & KgpgCore::ITYPE_PAIR));

	return static_cast<const KGpgGroupMemberNode *>(this);
}

KGpgSignNode *
KGpgNode::toSignNode()
{
	Q_ASSERT(getType() == KgpgCore::ITYPE_SIGN);

	return static_cast<KGpgSignNode *>(this);
}

const KGpgSignNode *
KGpgNode::toSignNode() const
{
	Q_ASSERT(getType() == KgpgCore::ITYPE_SIGN);

	return static_cast<const KGpgSignNode *>(this);
}

KGpgOrphanNode *
KGpgNode::toOrphanNode()
{
	Q_ASSERT(getType() == KgpgCore::ITYPE_SECRET);

	return static_cast<KGpgOrphanNode *>(this);
}

const KGpgOrphanNode *
KGpgNode::toOrphanNode() const
{
	Q_ASSERT(getType() == KgpgCore::ITYPE_SECRET);

	return static_cast<const KGpgOrphanNode *>(this);
}

bool
KGpgNode::hasChildren() const
{
	return false;
}

int
KGpgNode::getChildCount()
{
	return 0;
}

KGpgNode *
KGpgNode::getChild(const int index) const
{
	Q_UNUSED(index);
	return NULL;
}

int
KGpgNode::getChildIndex(KGpgNode *node) const
{
	Q_UNUSED(node);
	return 0;
}

KgpgCore::KgpgKeyTrust
KGpgNode::getTrust() const
{
	return TRUST_NOKEY;
}

QString
KGpgNode::getSize() const
{
	return QString();
}

QString
KGpgNode::getName() const
{
	return QString();
}

QString
KGpgNode::getEmail() const
{
	return QString();
}

QDateTime
KGpgNode::getExpiration() const
{
	return QDateTime();
}

QDateTime
KGpgNode::getCreation() const
{
	return QDateTime();
}

QString
KGpgNode::getId() const
{
	return QString();
}

QString
KGpgNode::getComment() const
{
	return QString();
}

KGpgExpandableNode *
KGpgNode::getParentKeyNode() const
{
	return m_parent;
}

//#include "KGpgNode.moc"
