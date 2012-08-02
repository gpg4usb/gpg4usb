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
#ifndef KGPGSIGNABLENODE_H
#define KGPGSIGNABLENODE_H

#include "KGpgExpandableNode.h"
#include "KGpgSignNode.h"

/**
 * @brief An object that may have KGpgSignNode children
 *
 * This class represents an object that may be signed, i.e. key nodes,
 * user ids, user attributes, and subkeys.
 */
class KGpgSignableNode : public KGpgExpandableNode
{
	Q_OBJECT

public:
	typedef QList<KGpgSignableNode *> List;
	typedef QList<const KGpgSignableNode *> const_List;

	KGpgSignableNode(KGpgExpandableNode *parent = NULL);
	virtual ~KGpgSignableNode();

	KGpgSignNode::List getSignatures(void) const;
	/**
	 * @brief count signatures
	 * @return the number of signatures to this object
	 *
	 * This does not include the number of signatures to child objects.
	 */
	virtual QString getSignCount() const;

	bool operator<(const KGpgSignableNode &other) const;
	bool operator<(const KGpgSignableNode *other) const;

	/**
	 * @brief returns the key node this node belongs to
	 * @returns this node if the node itself is a key or it's parent otherwise
	 */
	virtual KGpgKeyNode *getKeyNode(void) = 0;
	virtual const KGpgKeyNode *getKeyNode(void) const = 0;
};

#endif /* KGPGSIGNABLENODE_H */
