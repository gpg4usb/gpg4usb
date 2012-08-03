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
#ifndef KGPGREFNODE_H
#define KGPGREFNODE_H

#include "KGpgNode.h"

class KGpgKeyNode;
class KGpgRootNode;

/**
 * @brief Class for child objects that are only a reference to a primary key
 *
 * This is the base class for all type of objects that match these criteria:
 * -they can not have child objects
 * -they are only a reference to a primary key (which needs not to be in the
 *  key ring)
 *
 * Do not create instances from this class. Use KGpgGroupMemberNode and
 * KGpgSignNode as those represent the existing objects. This class exists
 * only to get the hierarchy right.
 */
class KGpgRefNode : public KGpgNode
{
	Q_OBJECT

private:
	const QString m_id;
	bool m_selfsig;		///< if this is a reference to it's own parent

protected:
	KGpgKeyNode *m_keynode;

	explicit KGpgRefNode(KGpgExpandableNode *parent, KGpgKeyNode *key);
	explicit KGpgRefNode(KGpgExpandableNode *parent, const QString &keyid);

	KGpgRootNode *getRootNode() const;

public:
	virtual ~KGpgRefNode();

	virtual QString getId() const;
	virtual QString getName() const;
	virtual QString getEmail() const;
	/**
	 * Get the node of the primary key this node references to
	 *
	 * This will return the key node of the primary key this node
	 * references. This may be %NULL if the primary key is not in the key
	 * ring, e.g. if this is a signature of an unknown key.
	 *
	 * @return the node of the primary key or %NULL
	 */
	virtual KGpgKeyNode *getRefNode() const;

	/**
	 * Check if the referenced key exists
	 *
	 * @return if getRefNode() will return %NULL or not
	 */
	bool isUnknown() const;

	/**
	 * Break the current reference
	 * @param root root node
	 *
	 * This is called when the referenced node is going away.
	 *
	 * The root node is passed for two reasons:
	 * @li it doesn't need to be searched again for every ref node which
	 * can be many in case of an important key node get's deleted
	 * @li the ref node may be a child of the deleted node, then we can
	 * not call the parents functions to find the root anymore. This helps
	 * simplifying the code
	 */
	void unRef(KGpgRootNode *root);

private Q_SLOTS:
	void keyUpdated(KGpgKeyNode *);
};

#endif /* KGPGREFNODE_H */
