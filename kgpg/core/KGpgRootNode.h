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
#ifndef KGPGROOTNODE_H
#define KGPGROOTNODE_H

#include "KGpgExpandableNode.h"
#include "KGpgKeyNode.h"

class KGpgGroupNode;
class QString;
class QStringList;

/**
 * @brief The parent of all key data objects
 *
 * This object is invisible to the user but acts as the internal base object for
 * everything in the keyring. It is anchestor of all other KGpgNode objects and
 * the only one that will ever return NULL when calling getParentKeyNode() on it.
 *
 * There is only one object of this type around at any time.
 */
class KGpgRootNode : public KGpgExpandableNode
{
	Q_OBJECT

	friend class KGpgGroupNode;

private:
	int m_groups;
	int m_deleting;

protected:
	virtual void readChildren();

public:
	explicit KGpgRootNode(KGpgItemModel *model);
	virtual ~KGpgRootNode();

	virtual KgpgCore::KgpgItemType getType() const;

	/**
	 * Create new group nodes
	 * @param groups list of group names to create
	 */
	void addGroups(const QStringList &groups);
	void addKeys(const QStringList &ids = QStringList());
	void refreshKeys(KGpgKeyNode::List nodes);
	/**
	 * Find a key node with the given id
	 *
	 * This scans the list of primary keys for a key with the given id
	 * and returns the corresponding key node.
	 *
	 * The key id will be matched against the characters given in keyId.
	 * If you give only 8 or 16 byte you will still find the key if it
	 * exists. To be really sure to find the correct node you should pass
	 * the complete fingerprint whenever possible.
	 *
	 * @param keyId the key id to find, any length is permitted
	 * @return pointer to key node or %NULL if no such key
	 */
	KGpgKeyNode *findKey(const QString &keyId);
	/**
	 * Return the child number of the key with the given id
	 *
	 * This scans the list of direct children for a key with the given
	 * key id. It returns the number in the internal list of children
	 * which is identical to the row number in the item model. Since
	 * proxy models may sort the items you should only call this function
	 * from the primary model (i.e. KGpgItemModel).
	 *
	 * The key id will be matched against the characters given in keyId.
	 * If you give only 8 or 16 byte you will still find the key if it
	 * exists. To be really sure to find the correct node you should pass
	 * the complete fingerprint whenever possible.
	 *
	 * @param keyId the key id to find, any length is permitted
	 * @return the child number or -1 if there is no such key
	 */
	int findKeyRow(const QString &keyId);

	/**
	 * Return the child number of the given key
	 * @param key the key to search for
	 *
	 * @overload
	 */
	int findKeyRow(const KGpgKeyNode *key);

	/**
	 * Return the group count
	 * @return the number of group nodes
	 */
	int groupChildren() const;

	/**
	 * Return a pointer to this object or NULL
	 *
	 * This returns a pointer to this object if the object will persist,
	 * i.e. is not currently in destruction. If the object is already
	 * cleaning up NULL is returned.
	 */
	KGpgRootNode *asRootNode();
	/**
	 * @overload
	 */
	const KGpgRootNode *asRootNode() const;

Q_SIGNALS:
	void newKeyNode(KGpgKeyNode *);
};

#endif /* KGPGROOTNODE_H */
